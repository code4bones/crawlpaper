/*
	CMail.cpp
	Classe base per interfaccia SMTP/POP3 (SDK).
	Luca Piergentili, 24/09/96
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include <ctype.h>
#include "CDateTime.h"
#include "CMail.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/*
	CMail()
*/
CMail::CMail()
{
	memset(&m_Mail,'\0',sizeof(MAIL));
	Reset();
}

/*
	~CMail()
*/
CMail::~CMail()
{
	Reset();
}

/*
	Reset()

	Inizializza.
*/
void CMail::Reset(void)
{
	// allocati dalle Set...() e/o durante l'invio via script
	if(m_Mail.to)
		delete [] m_Mail.to,m_Mail.to = NULL;
	if(m_Mail.cc)
		delete [] m_Mail.cc,m_Mail.cc = NULL;
	if(m_Mail.bcc)
		delete [] m_Mail.bcc,m_Mail.bcc = NULL;

	// azzera la struttura
	memset(&m_Mail,'\0',sizeof(MAIL));
	
	// imposta i campi interni
	strcpy(m_Mail.mime_version,MIME_VERSION);
	strcpy(m_Mail.x_mailer,X_MAILER);

	// resetta i codici d'errore
	ResetWinsockError();
	ResetMailError();

	// flags
#if defined(_WINDOWS)
	#ifdef _DEBUG
		m_Mail.flag.quiet = 0;
	#else
		m_Mail.flag.quiet = 1;
	#endif
#endif
}

/*
	SetTo()

	Imposta il campo 'To:' dell'email.
*/
void CMail::SetTo(const char* to)
{
	int len = strlen(to) + 1;
	m_Mail.to = new char[len];
	memset(m_Mail.to,'\0',len);
	strcpyn(m_Mail.to,to,len);
}

/*
	SetCc()

	Imposta il campo 'Cc:' dell'email.
*/
void CMail::SetCc(const char* cc)
{
	int len = strlen(cc) + 1;
	m_Mail.cc = new char[len];
	memset(m_Mail.cc,'\0',len);
	strcpyn(m_Mail.cc,cc,len);
}

/*
	SetBcc()

	Imposta il campo 'Bcc:' dell'email.
*/
void CMail::SetBcc(const char* bcc)
{
	int len = strlen(bcc) + 1;
	m_Mail.bcc = new char[len];
	memset(m_Mail.bcc,'\0',len);
	strcpyn(m_Mail.bcc,bcc,len);
}

/*
	SetLog()

	Imposta il nome del file di log.
*/
void CMail::SetLog(const char* log)
{
	// flags (log delle attivita' aggiungendo al file esistente)
	m_Mail.flag.log    = 1;
	m_Mail.flag.append = 1;
	m_Mail.flag.create = 0;

	strcpyn(m_Mail.log,log,sizeof(m_Mail.log));
	m_Mail.hLog = HFILE_ERROR;

	if(m_Mail.flag.append)
	{
		if((m_Mail.hLog = _lopen(m_Mail.log,OF_WRITE))==HFILE_ERROR)
			m_Mail.hLog = _lcreat(m_Mail.log,0);
	}
	else
	{
		m_Mail.hLog = _lcreat(m_Mail.log,0);
	}
	
	if(m_Mail.hLog!=HFILE_ERROR)
		_lclose(m_Mail.hLog);
}

/*
	Receive()
	
	Scarica le email presenti sul server.
	Restituisce 0 o il codice winsock in caso d'errore.
*/
int CMail::Receive(FPCALLBACK /*lpfnCallback*/ /*=NULL*/,LPVOID /*lpVoid*/ /*=NULL*/)
{
	int tot;
	char buffer[MAX_TEXTLINE+1];

	// azzera i codici d'errore
	CSock::SetWSALastError(0);
	ResetWinsockError();
	ResetMailError();

	// apre il socket
	if(CSock::Open())
	{
		// si collega al server
		if(!CSock::Connect(m_Mail.pop3host,m_Mail.pop3port))
		{
			SetWinsockError();
			goto done;
		}

		// controlla se la connessione e' avvenuta correttamente
		if(Ack(buffer,sizeof(buffer))!=0)
		{
			SetMailError(buffer);
			goto done;
		}

		/*
		USER
		*/
		_snprintf(buffer,sizeof(buffer)-1,"USER %s\r\n",m_Mail.user);
		if(SendData(buffer)==SOCKET_ERROR)
		{
			SetWinsockError();
			goto done;
		}
		if(Ack(buffer,sizeof(buffer))!=0)
		{
			SetMailError(buffer);
			goto done;
		}

		/*
		PASS
		*/
		_snprintf(buffer,sizeof(buffer)-1,"PASS %s\r\n",m_Mail.pass);
		if(SendData(buffer)==SOCKET_ERROR)
		{
			SetWinsockError();
			goto done;
		}
		if(Ack(buffer,sizeof(buffer))!=0)
		{
			SetMailError(buffer);
			goto done;
		}

		// controlla se esiste posta in attesa sul server
		if((tot = Stat()) > 0)
		{
			// scarica l'email
			if(ReceiveEmail(buffer,sizeof(buffer),tot)!=0)
				goto done;
		}
		else if(tot==0) // nessun email presente
		{
			;
		}
		else // errore
			goto done;

done:

		/*
		QUIT
		*/
		SendData("QUIT\r\n");

		// aspetta l'ok del server prima di chiudere il socket
		if(Ack(buffer,sizeof(buffer))!=0)
			SetMailError(buffer);

		// chiude il socket
		CSock::Close();
	}

	// restituisce l'errore winsock
	return(CSock::GetWSALastError());
}

/*
	ReceiveHeader()

	Recupera l'header del messaggio email.
	Ritorna 0 o -1 in caso d'errore.
*/
int CMail::ReceiveHeader(int email,char* header,int header_size)
{
	int len;
	int tot_len = 0;
	register char* s;
	register char* p;
	char subject[MAX_SUBJECT_LEN+1];
	char from[MAX_EMAIL_LEN+1];
	char buffer[MAX_TEXTLINE+1];
	
	/*
	TOP
	*/
	_snprintf(header,sizeof(header)-1,"TOP %d 1\r\n",email);
	if(SendData(header)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}

	// cicla (in ricezione) fino a trovare la fine dell'header
	do
	{	
		// riceve l'header dal server
		if(ReceiveData(buffer,sizeof(buffer))==SOCKET_ERROR)
		{
			SetWinsockError();
			if(*buffer)
				SetMailError(buffer);
			return(-1);
		}

		len = strlen(buffer);
		
		// l'header (+ la prima linea dell'email) potrebbe essere piu' lungo del previsto
		if(len + tot_len < header_size)
			strcpy((header+tot_len),buffer);
		
		tot_len += len;
	}
	while(strstr(buffer,"\r\n.\r\n")==NULL);

	// cerca la fine dell'header per controllare se e' riuscito a ricevere tutti i dati
	if(strstr(header,"\r\n.\r\n")!=NULL)
	{
		// cerca il campo Subject:
		if((s = stristr(header,"Subject: "))!=NULL)
		{
			s += 9;
			p = subject;
				
			// copia il contenuto (subject) nel buffer locale
			while(*s && *s!='\r' && (p-subject < sizeof(subject)))
				*p++ = *s++;
					
			*p = '\0';
		}
		else
			strcpy(subject,"");

		// cerca il campo From:
		if((s = stristr(header,"From: "))!=NULL)
		{
			s += 6;
			p = from;
				
			// copia il contenuto (from) nel buffer locale
			while(*s && *s!='\r' && (p-from < sizeof(from)))
				*p++ = *s++;
					
			*p = '\0';
		}
		else
			strcpy(from,"");

		// copia nel buffer di destinazione i campi dell'header (subject/from)
		if((int)(strlen(subject) + strlen(from)) < header_size)
			_snprintf(header,sizeof(header)-1,"\n\"%s\"\nfrom:\n%s",subject,from);
		else
		{
			memset(header,'\0',header_size);
			return(-1);
		}
	}

	return(0);
}

/*
	ReceiveEmail()

	Riceve i messaggi email.
	Restituisce 0 o il codice winsock in caso d'errore.
*/
int CMail::ReceiveEmail(char* buffer,int size,int tot)
{
	int		i;
#if defined(_WINDOWS)
	int		prompt = IDNO;
#endif
	int		current = 0;
	char*	crlf = NULL;
	int		crlf_len = (size * 2) + 1;
	char		header[(MAX_TEXTLINE*4)+1];
	CDateTime	date(GMT);

	// azzera il codice winsock
	CSock::SetWSALastError(0);
	
#if defined(_WINDOWS)
	// visualizza il totale di email presenti nella casella di posta
	if(!m_Mail.flag.quiet)
	{
		_snprintf(buffer,sizeof(buffer)-1,"%d messages into mailbox.",tot);
		::MessageBox(NULL,buffer,"CMail::ReceiveEmail()",MB_OK|MB_ICONINFORMATION|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
		prompt = ::MessageBox(NULL,"Do you want to confirm each download ?","CMail::ReceiveEmail()",MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
	}
#endif

	// per ogni email
	for(i = 0; i < tot; i++)
	{
		// email corrente
		current++;
		    
#if defined(_WINDOWS)
		// chiede (a seconda del flag) la conferma per il download dell'email
		if(prompt==IDYES)
		{
			// se la ricezione dell'header non riesce, visualizza solo il totale delle email presenti
			if(ReceiveHeader(current,header,sizeof(header))==0)
			{
				_snprintf(buffer,sizeof(buffer)-1,"Do you want get message %d of %d:%s ?",current,tot,header);
				prompt = ::MessageBox(NULL,buffer,"CMail::ReceiveEmail()",MB_YESNOCANCEL|MB_ICONQUESTION|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
			}

			switch(prompt)
			{
				case IDCANCEL:
					CSock::SetWSALastError(0);
					goto done;

				case IDNO:
					continue;
					
				case IDYES:
				default:
					break;
			}
		}
#endif

		// crea l'intestazione per il mailer e la registra nella mailbox
		_snprintf(buffer,sizeof(buffer)-1,"\r\nFrom ???@??? %s\r\n",date.GetFormattedDate());
		if(*(m_Mail.inmbx))
			InBox(buffer,strlen(buffer));

		/*
		RETR
		*/
		_snprintf(buffer,sizeof(buffer)-1,"RETR %d\r\n",current);
		if(SendData(buffer)==SOCKET_ERROR)
		{
			SetWinsockError();
			goto done;
		}

		// alloca ed azzera i buffer per la ricezione dell'email
		crlf = new char[crlf_len];
		memset(crlf,'\0',crlf_len);
		memset(buffer,'\0',size);
		
		// scarica l'email ciclando (in ricezione) fino a trovarne la fine (\r\n.\r\n)
		do
		{	
			// copia nel buffer di controllo i dati ricevuti
			strcpyn(crlf,buffer,sizeof(crlf));
			
			// ricezione dati
			if(ReceiveData(buffer,size)==SOCKET_ERROR)
			{
				SetWinsockError();
				if(*buffer)
					SetMailError(buffer);
				goto done;
			}
				
			// registra l'email nella mailbox
			if(*(m_Mail.inmbx))
				InBox(buffer,strlen(buffer));

			// aggiunge al buffer di controllo i dati ricevuti
			strcat(crlf,buffer);
		}
		while(strstr(buffer,"\r\n.\r\n")==NULL && strstr(crlf,"\r\n.\r\n")==NULL);
				
		/*
		DELE
		*/
		if(!m_Mail.flag.leavemail)
		{
			// invia il comando per eliminare l'email
			_snprintf(buffer,sizeof(buffer)-1,"DELE %d\r\n",current);
			if(SendData(buffer)==SOCKET_ERROR)
			{
				SetWinsockError();
				goto done;
			}

			// chiede risposta al server
			if(ReceiveData(buffer,size)==SOCKET_ERROR)
			{
				SetWinsockError();
				if(*buffer)
					SetMailError(buffer);
				goto done;
			}
		}
	}

done:

	if(crlf)
		delete [] crlf;

	// restituisce l'errore winsock
	return(CSock::GetWSALastError());
}

/*
	Stat()
	
	Ricava il numero di email presenti nella casella di posta.
	Ritorna >=0 o -1 in caso d'errore.
*/
int CMail::Stat(void)
{
	char			tot[16];
	register char*	p_tot = tot;
	char			buffer[MAX_TEXTLINE+1];
	register char*	p_buf = buffer;

	/*
	STAT
	*/
	strcpy(buffer,"STAT\r\n");
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
	if(Ack(buffer,sizeof(buffer))!=0)
	{
		SetMailError(buffer);
		return(-1);
	}

	// estrae dalla risposta del server il numero di email presenti nella casella di posta
	while(!isdigit(*p_buf++))
		;

	--p_buf;

	while(*p_buf && isdigit(*p_buf) && (p_tot-tot) < sizeof(tot))
		*p_tot++ = *p_buf++;

	*p_tot = (char)NULL;

	// restituisce il numero di email presenti
	return(atoi(tot));
}

/*
	Send()

	Invia l'email al server.
	Restituisce 0 o il codice winsock in caso d'errore.
*/
int CMail::Send(FPCALLBACK lpfnCallback/* = NULL */,LPVOID lpVoid/* = NULL */)
{
	int	error = 0;
	char	buffer[MAX_TEXTLINE+1];

	MAILSTATUS MailStatus;

	// azzera il codice d'errore winsock
	CSock::SetWSALastError(0);
	ResetWinsockError();
	ResetMailError();

	if(lpfnCallback)
	{
		MailStatus.mailstatus = MAIL_STATUS_OPEN;
		lpfnCallback(&MailStatus,lpVoid);
	}

	// apre il socket
	if(CSock::Open())
	{
		if(lpfnCallback)
		{
			MailStatus.mailstatus = MAIL_STATUS_CONNECT;
			lpfnCallback(&MailStatus,lpVoid);
		}

		// si connette al server
		if(!CSock::Connect(m_Mail.smtphost,m_Mail.smtpport))
		{
			SetWinsockError();
			goto done;
		}

		// controlla se la connessione e' avvenuta correttamente
		if(Ack(buffer,sizeof(buffer))!=0)
		{
			SetMailError(buffer);
			goto done;
		}

		/*
		HELO
		*/
		_snprintf(buffer,sizeof(buffer)-1,"HELO %s\r\n",CSock::GetLocalHostName());
		if(SendData(buffer)==SOCKET_ERROR)
		{
			SetWinsockError();
			goto done;
		}
		if(Ack(buffer,sizeof(buffer))!=0)
		{
			SetMailError(buffer);
			goto done;
		}

		// invia l'email
		do
		{
			if(lpfnCallback)
			{
				MailStatus.mailstatus = MAIL_STATUS_PREPARING;
				lpfnCallback(&MailStatus,lpVoid);
			}

			// se non viene specificato un destinatario legge dallo script
			if(m_Mail.to && *(m_Mail.to))
			{
				if((m_Mail.message && *(m_Mail.message)) || (m_Mail.attach && *(m_Mail.attach)))
				{
					// crea il testo dell'email a partire dai file testo/attachment
					if(CreateMailBody()!=0)
						goto done;

					// invia l'header dell'email
					if(SendHeader(lpfnCallback,lpVoid)!=0)
						goto done;
			
					// invia il testo dell'email
					if(SendEmail(lpfnCallback,lpVoid)!=0)
						goto done;
				}
			}
		}
		while(ReadScript(error)!=0);

		/*
		QUIT
		*/
		if(lpfnCallback)
		{
			MailStatus.mailstatus = MAIL_STATUS_CLOSE;
			lpfnCallback(&MailStatus,lpVoid);
		}

		SendData("QUIT\r\n");

		// aspetta l'ok del server prima di chiudere il socket
		if(Ack(buffer,sizeof(buffer))!=0)
			SetMailError(buffer);
done:
		// chiude il socket
		CSock::Close();
	}

	if(lpfnCallback)
	{
		MailStatus.mailstatus = MAIL_STATUS_DONE;
		lpfnCallback(&MailStatus,lpVoid);
	}

	// restituisce l'errore winsock
	return(CSock::GetWSALastError());
}

/*
	SendHeader()
	
	Invia l'header dell'email al server.
*/
int CMail::SendHeader(FPCALLBACK /*lpfnCallback*/ /* = NULL*/,LPVOID /*lpVoid*/ /* = NULL*/)
{
	char*	p;
	char		rcpt[MAX_EMAIL_LEN+1];
	char		buffer[MAX_TEXTLINE+1];
	CDateTime	date(GMT);

	/*
	MAIL FROM
	*/
	_snprintf(buffer,sizeof(buffer)-1,"MAIL FROM:<%s>\r\n",m_Mail.from);
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
	if(Ack(buffer,sizeof(buffer))!=0)
	{
		SetMailError(buffer);
		return(-1);
	}

	/*
	RCPT TO (To:)
	*/
	do
	{
		p = ParseRCPT(m_Mail.to,rcpt,sizeof(rcpt));

		_snprintf(buffer,sizeof(buffer)-1,"RCPT TO:<%s>\r\n",rcpt);
		if(SendData(buffer)==SOCKET_ERROR)
		{
			SetWinsockError();
			return(-1);
		}
		if(Ack(buffer,sizeof(buffer))!=0)
		{
			SetMailError(buffer);
			return(-1);
		}
	}
	while(p!=NULL);
    
	/*
	RCPT TO (Cc:)
	*/
	if(m_Mail.cc)
	{
		do
		{
			p = ParseRCPT(m_Mail.cc,rcpt,sizeof(rcpt));

			_snprintf(buffer,sizeof(buffer)-1,"RCPT TO:<%s>\r\n",rcpt);
			if(SendData(buffer)==SOCKET_ERROR)
			{
				SetWinsockError();
				return(-1);
			}
			if(Ack(buffer,sizeof(buffer))!=0)
			{
				SetMailError(buffer);
				return(-1);
			}
	    }
	    while(p!=NULL);
	}

	/*
	RCPT TO (Bcc:)
	*/
	if(m_Mail.bcc)
	{
		do
		{
			p = ParseRCPT(m_Mail.cc,rcpt,sizeof(rcpt));

			_snprintf(buffer,sizeof(buffer)-1,"RCPT TO:<%s>\r\n",rcpt);
			if(SendData(buffer)==SOCKET_ERROR)
			{
				SetWinsockError();
				return(-1);
			}
			if(Ack(buffer,sizeof(buffer))!=0)
			{
				SetMailError(buffer);
				return(-1);
			}
	    }
	    while(p!=NULL);
	}
		
	/*
	DATA
	*/
	_snprintf(buffer,sizeof(buffer)-1,"DATA\r\n");
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
	if(Ack(buffer,sizeof(buffer))!=0)
	{
		SetMailError(buffer);
		return(-1);
	}

	// crea l'intestazione per il mailer e la registra nella mailbox
	_snprintf(buffer,sizeof(buffer)-1,"\r\nFrom ???@??? %s\r\n",date.GetFormattedDate());
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	Date: (se non impostato dal chiamante)
	*/
	if(m_Mail.date[0]=='\0')
		_snprintf(buffer,sizeof(buffer)-1,"Date: %s\r\n",date.GetFormattedDate());
	else
		_snprintf(buffer,sizeof(buffer)-1,"Date: %s\r\n",m_Mail.date);

	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
	
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	From:
	*/
	if(*m_Mail.realname)
		_snprintf(buffer,sizeof(buffer)-1,"From: %s <%s>\r\n",m_Mail.realname,m_Mail.from);
	else
		_snprintf(buffer,sizeof(buffer)-1,"From: %s\r\n",m_Mail.from);
	
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
			
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	Sender:
	*/
	_snprintf(buffer,sizeof(buffer)-1,"Sender: %s\r\n",m_Mail.sender);
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
		
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	Reply-To:
	*/
	_snprintf(buffer,sizeof(buffer)-1,"Reply-To: %s\r\n",m_Mail.reply);
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
			
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	To:
	*/
	_snprintf(buffer,sizeof(buffer)-1,"To: %s\r\n",m_Mail.to);
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
			
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	Cc:
	*/
	if(m_Mail.cc)
	{
		_snprintf(buffer,sizeof(buffer)-1,"CC: %s\r\n",m_Mail.cc);
		if(SendData(buffer)==SOCKET_ERROR)
		{
			SetWinsockError();
			return(-1);
		}

		if(*(m_Mail.outmbx))
			OutBox(buffer,strlen(buffer));
	}

	/*
	Bcc:
	*/
	if(m_Mail.bcc)
	{
		_snprintf(buffer,sizeof(buffer)-1,"BCC: %s\r\n",m_Mail.bcc);
		if(SendData(buffer)==SOCKET_ERROR)
		{
			SetWinsockError();
			return(-1);
		}
					
		if(*(m_Mail.outmbx))
			OutBox(buffer,strlen(buffer));
	}
    
	/*
	Subject:
	*/
	_snprintf(buffer,sizeof(buffer)-1,"Subject: %s\r\n",m_Mail.subject);
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
			
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	MIME-Version:
	*/
	_snprintf(buffer,sizeof(buffer)-1,"MIME-Version: %s\r\n",m_Mail.mime_version);
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
			
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	X-Mailer:
	*/
	_snprintf(buffer,sizeof(buffer)-1,"X-Mailer: %s\r\n",m_Mail.x_mailer);
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
		
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	X-Message:
	*/
	_snprintf(buffer,sizeof(buffer)-1,"X-Comment: message: %s\r\n",(m_Mail.flag.message ? m_Mail.message : "[none]"));
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
			
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	X-Attachment:
	*/
	_snprintf(buffer,sizeof(buffer)-1,"X-Comment: attachment: %s\r\n",(m_Mail.flag.attach ? m_Mail.attach : "[none]"));
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
			
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	/*
	X-Script:
	*/
	_snprintf(buffer,sizeof(buffer)-1,"X-Comment: script: %s\r\n",(m_Mail.flag.script ? m_Mail.script : "[none]"));
	if(SendData(buffer)==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}
			
	if(*(m_Mail.outmbx))
		OutBox(buffer,strlen(buffer));

	return(0);
}

/*
	SendEmail()
	
	Apre il file contenente il testo dell'email e lo invia al server.
*/
int CMail::SendEmail(FPCALLBACK lpfnCallback/* = NULL*/,LPVOID lpVoid/* = NULL*/)
{
	FILE*	fp;
	char		buffer[MAX_TEXTLINE+1];
	char		crlf[(MAX_TEXTLINE*2)+1];
	MAILSTATUS MailStatus = {MAIL_STATUS_SEND,0};
	long		lOfs = 0L;
	long		lSize = 0L;
	
	// apre il file contenente l'email (modalita' testo)
	if((fp = fopen(BODYFILE,"r"))!=(FILE*)NULL)
	{
		fseek(fp,0,SEEK_END);
		lSize = ftell(fp);
		fseek(fp,0,SEEK_SET);

		// legge dal file (modalita' testo) ed invia al server (modalita' binaria)
		while(fgets(buffer,sizeof(buffer)-1,fp))
		{
			lOfs = ftell(fp);
			MailStatus.progress = (lOfs * 100) / lSize;

			// trasla il \n in \r\n
			TranslateCrLf(buffer,crlf);
			
			// invia i dati al server
			if(SendData(crlf)==SOCKET_ERROR)
			{
				fclose(fp);
				SetWinsockError();
				return(-1);
			}
			
			if(lpfnCallback)
				if(lpfnCallback(&MailStatus,lpVoid)==IDCANCEL)
				{
					fclose(fp);
					remove(BODYFILE);
					return(-1);
				}

			// aggiunge i dati alla mailbox di output
			if(*(m_Mail.outmbx))
				OutBox(crlf,strlen(crlf));
		}

		// chiude ed elimina il file	
		fclose(fp);
		remove(BODYFILE);
	}
	else
		return(-1);

	// invia al server la sequenza indicante la fine del messaggio
	if(SendData("\r\n.\r\n")==SOCKET_ERROR)
	{
		SetWinsockError();
		return(-1);
	}

	// chiede conferma al server sulla ricezione del messaggio
	if(Ack(buffer,sizeof(buffer))!=0)
	{
		SetMailError(buffer);
		return(-1);
	}

	return(0);
}

/*
	ReadScript()
	
	Estrae dal file script le email da inviare.
	Restituisce 0 se sta' leggendo dal file script, !=0 per eof
*/
int CMail::ReadScript(int& error)
{
	#define BUFF_SIZE (MAX_TEXTLINE+1)

	FILE*		fp;
	static long	fl = 0L;
	static char*	buffer = (char*)NULL;
	int			len;

	// necessario per il ciclo chiamante (non deve leggere nessuno script se utilizzato da linea di comando)
	if(!(m_Mail.flag.script))
		return(0);
		
	// apre il file script
	if((fp = fopen(m_Mail.script,"rb"))!=(FILE*)NULL)
	{
		// se apre il file per la prima volta alloca i buffer per la lettura e la traslazione dello script
		if(fl==0L)
		{
			buffer = new char[BUFF_SIZE];

			if(!buffer)
			{
				buffer = (char*)NULL;
				fclose(fp);
				error = WSA_FAILURE;
				return(0);
			}
		}
		
		// inizializza i campi gestiti via script
		memset(m_Mail.message,'\0',sizeof(m_Mail.message));
		memset(m_Mail.attach,'\0',sizeof(m_Mail.attach));

		// (ri)posiziona sul file script
		fseek(fp,fl,SEEK_SET);
			    	    
		// legge una linea dal file script, il ciclo serve solo per saltare le righe vuote o non valide
		while(TRUE)
		{
			if(fgets(buffer,BUFF_SIZE-1,fp)!=NULL)
			{
				len = strlen(buffer);

				fl += (long)len; // aggiorna il puntatore nel file
				
				if(buffer[len-2]=='\r' && buffer[len-1]=='\n')
					buffer[len-2] = '\0';
				
				// in modo script gestisce solo soggetto/destinatario(to,cc,bcc)/messaggio/attachment
				if(GetScriptOpt(buffer) < 3)
					continue;
			}
			else // eof sul file script
			{
				// rilascia i buffer allocati
				if(buffer)
					delete [] buffer,buffer = NULL;

				// (ri)inizializza per le chiamate successive
				fl = 0L;
			}
		    	
			break; // il ciclo serve solo per saltare le righe vuote o non valide
		}
			
		fclose(fp);
		
		return(fl!=0L);
	}
	else
	{
		error = WSA_FAILURE;
		return(0);
	}
}

/*
	CreateMailBody()
	
	Crea il testo dell'email distinguendo tra: testo+attachment, solo testo, solo attachment.
*/
int CMail::CreateMailBody(void)
{
	int error = -1;

	if(m_Mail.flag.message && m_Mail.flag.attach)		// testo+attach
		error = CreateMixedBody();
	else if(m_Mail.flag.message && !m_Mail.flag.attach)	// solo testo
		error = CreateTextBody();
	else if(!m_Mail.flag.message && m_Mail.flag.attach)	// solo attach
		error = CreateAttachBody();

	return(error);
}

/*
	CreateMixedBody()
	
	Crea il testo dell'email per i messaggi misti (testo+attachment).
*/
int CMail::CreateMixedBody(void)
{
	FILE* fp_body;
	FILE* fp_text;
	FILE* fp_attach;
	char buffer[MAX_TEXTLINE+1];
	char crlf[(MAX_TEXTLINE*2)+1];
	char attach[_MAX_PATH+1];

	// crea il file per il testo dell'email (modalita' binaria)
	if((fp_body = fopen(BODYFILE,"wb"))==(FILE*)NULL)
		return(-1);
		
	// apre il file contenente il messaggio da inviare (modalita' testo)
	if((fp_text = fopen(m_Mail.message,"r"))==(FILE*)NULL)
	{
		fclose(fp_body);
		return(-1);
	}
	
	// codifica il file da inviare come attachment
	if(Encode(m_Mail.attach,m_Mail.encoded)!=0)
	{
		fclose(fp_body);
		fclose(fp_text);
		return(-1);
	}
		
	// apre il file attachment convertito (modalita' testo)
	if((fp_attach = fopen(m_Mail.encoded,"r"))==(FILE*)NULL)
	{
		fclose(fp_body);
		fclose(fp_text);
		return(-1);
	}

	// scrive l'header nel testo dell'email (multipart/mixed)
	fprintf(fp_body,"Content-Type: multipart/mixed; boundary=\"%s\"\r\n",BOUNDARY);
	fputs("\r\n",fp_body);
	fputs("This is a multi-part message in MIME format.\r\n",fp_body);
	fputs("\r\n",fp_body);

	// scrive l'header realtivo alla prima parte dell'email (file testo)
	fprintf(fp_body,"--%s\r\n",BOUNDARY);

#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): warning: absolute content type/encoding")
#if 1
	fputs("Content-Type: text/plain; charset=us-ascii\r\n",fp_body);
	fputs("Content-Transfer-Encoding: 7bit\r\n",fp_body);
#else
	fputs("Content-Type: text/plain; charset=ISO-8859-1\r\n",fp_body);
	fputs("Content-Transfer-Encoding: 8bit\r\n",fp_body);
#endif

	fputs("\r\n",fp_body);

	// trascrive il messaggio nel testo dell'email
	while(fgets(buffer,sizeof(buffer)-1,fp_text))
	{
		TranslateCrLf(buffer,crlf);
		fputs(crlf,fp_body);
	}

	fputs("\r\n",fp_body);

	// scrive l'header realtivo alla seconda parte dell'email (file attachment)
	fprintf(fp_body,"--%s\r\n",BOUNDARY);
	StripPath(m_Mail.attach,attach);
	{
		fputs("Content-Type: application/octet-stream\r\n",fp_body);
		fputs("Content-Transfer-Encoding: base64\r\n",fp_body);
		fprintf(fp_body,"Content-Disposition: attachment; filename=\"%s\"\r\n",attach);
	}
	fputs("\r\n",fp_body);
		
	// trascrive il file attachment nel testo dell'email
	while(fgets(buffer,sizeof(buffer)-1,fp_attach))
	{
		TranslateCrLf(buffer,crlf);
		fputs(crlf,fp_body);
	}

	// fine messaggio email
	fprintf(fp_body,"--%s--\r\n",BOUNDARY);

	fclose(fp_body);
	fclose(fp_text);
	fclose(fp_attach);
     
	remove(m_Mail.encoded);
     
	return(0);
}

/*
	CreateTextBody()
	
	Crea il testo dell'email (solo testo).
*/
int CMail::CreateTextBody(void)
{
	FILE*	fp_body;
	FILE*	fp_text;
	char		buffer[MAX_TEXTLINE+1];
	char		crlf[(MAX_TEXTLINE*2)+1];
	
	// crea il file per il testo dell'email (modalita' binaria)
	if((fp_body = fopen(BODYFILE,"wb"))==(FILE*)NULL)
		return(-1);

	// apre il file contenente il messaggio da inviare (modalita' testo)
	if((fp_text = fopen(m_Mail.message,"r"))==(FILE*)NULL)
	{
		fclose(fp_body);
		return(-1);
	}
	
	// scrive l'header nel testo dell'email
#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): warning: absolute content type/encoding")
#if 1
	fputs("Content-Type: text/plain; charset=us-ascii\r\n",fp_body);
	fputs("Content-Transfer-Encoding: 7bit\r\n",fp_body);
#else
	fputs("Content-Type: text/plain; charset=ISO-8859-1\r\n",fp_body);
	fputs("Content-Transfer-Encoding: 8bit\r\n",fp_body);
#endif
	fputs("\r\n",fp_body);

	// trascrive il messaggio nel testo dell'email
	while(fgets(buffer,sizeof(buffer)-1,fp_text))
	{
		TranslateCrLf(buffer,crlf);
		fputs(crlf,fp_body);
	}

	fputs("\r\n",fp_body);

	fclose(fp_body);
	fclose(fp_text);
     
	return(0);
}

/*
	CreateAttachBody()
	
	Crea il testo dell'email (solo attachment).
*/
int CMail::CreateAttachBody(void)
{
	FILE*	fp_body;
	FILE*	fp_attach;
	char		buffer[MAX_TEXTLINE+1];
	char		crlf[(MAX_TEXTLINE*2)+1];
	char		attach[_MAX_PATH+1];
	
	// crea il file per il testo dell'email (modalita' binaria)
	if((fp_body = fopen(BODYFILE,"wb"))==(FILE*)NULL)
		return(-1);

	// converte il file da inviare come attachment in formato MIME base 64
	if(Encode(m_Mail.attach,m_Mail.encoded)!=0)
	{
		fclose(fp_body);
		return(-1);
	}
		
	// apre il file attachment convertito (modalita' testo)
	if((fp_attach = fopen(m_Mail.encoded,"r"))==(FILE*)NULL)
	{
		fclose(fp_body);
		return(-1);
	}

	// scrive l'header nel testo dell'email (octet-stream/x-zip-compressed)
	StripPath(m_Mail.attach,attach);
	{
		fputs("Content-Type: application/octet-stream\r\n",fp_body);
		fputs("Content-Transfer-Encoding: base64\r\n",fp_body);
		fprintf(fp_body,"Content-Disposition: attachment; filename=\"%s\"\r\n",attach);
	}
	fputs("\r\n",fp_body);

	// trascrive il file attachment nel testo dell'email
	while(fgets(buffer,sizeof(buffer)-1,fp_attach))
	{
		TranslateCrLf(buffer,crlf);
		fputs(crlf,fp_body);
	}

     fclose(fp_body);
     fclose(fp_attach);
     
	remove(m_Mail.encoded);
     
	return(0);
}

/*
	ReceiveData()
	
	Utilizzata per ricevere i dati dal server.
	Restituisce il numero di bytes ricevuti o SOCKET_ERROR in caso d'errore.
*/
int CMail::ReceiveData(char* buffer,int size)
{
	int recv = 0;

	// azzera il buffer di input e riceve i dati
	memset(buffer,'\0',size);
	char* p = (char*)CSock::Receive(&recv,size-1);

	// errore
	if(recv==SOCKET_ERROR)
	{
		memset(buffer,'\0',size);
	}
	// nessun dato da ricevere o connessione chiusa
	else if(recv==0)
	{
		memset(buffer,'\0',size);
	}
	// dati ricevuti
	else if(recv > 0)
	{
		// passa nel buffer di input i dati ricevuti
		memcpy(buffer,p,recv);
				
		// scarica sul log quanto ricevuto
		Log(buffer,recv);

		// controlla il codice di ritorno (POP3)
		if(memcmp(buffer,"-ERR",4)==0)
		{
			recv = SOCKET_ERROR;
		}
		else if(memcmp(buffer,"+OK",3)==0) // elimina la risposta del server presente in testa all'email (POP3)
		{
			register char* p = buffer;

			while(*p!='\r')
				p++;
			
			p += 2;

			memmove(buffer,p,recv - (p-buffer));
			buffer[recv - (p-buffer)] = '\0';
		}
	}

	return(recv);
}

/*
	SendData()
	
	Utilizzata per inviare i dati al server.
	Restituisce il numero di bytes inviati o SOCKET_ERROR in caso d'errore.
*/
int CMail::SendData(const char* data)
{
	int len = strlen(data);

	Log((char*)data,len);

	return(CSock::Send(data,len));
}

/*
	Ack()
	
	Utilizzata per ricevere la risposta del server.
*/
int CMail::Ack(char* ack,int size)
{
	int recv = 0;
	char* p;

//#if defined(_WINDOWS)
//	::Sleep(5000);
//#endif

	memset(ack,'\0',size);

	// riceve la risposta dal server
	if((p = (char*)CSock::Receive(&recv,size))!=NULL)
	{
		memcpy(ack,p,recv);
		Log(ack,recv);
	}

	// errore
	if(recv==SOCKET_ERROR)
	{
		return(CSock::GetWSALastError());
	}
	// nessun dato da ricevere o connessione chiusa
	else if(recv==0)
	{
		return(CSock::GetWSALastError());
	}
	// nessun errore, controlla comunque il codice di ritorno
	else if(recv > 0)
	{
		// SMTP
		if(isdigit(ack[0]))
		{
			if(ack[0] > '3')
			{
				CSock::SetWSALastError(WSAECONNREFUSED);
				return(CSock::GetWSALastError());
			}
		}
		// POP3
		else
		{
			if(memcmp(ack,"-ERR",4)==0)
			{
				CSock::SetWSALastError(WSAECONNREFUSED);
				return(CSock::GetWSALastError());
			}
		}
	}

	return(0);
}

/*
	Encode()
	
	Converte il file in formato MIME base64.
*/
int CMail::Encode(char* attach,char* base64)
{
	char* p;
	
	// costruisce il nome per il file di destinazione (nome file sorgente + .b64)
	strcpy(base64,attach);
	
	if((p = strchr(base64,'.'))!=NULL)
		*p = '\0';

	strcat(base64,".b64");
	
	return(CBase64::Encode(attach,base64));
}

/*
	Log()
	
	Effettua il log dei dati spediti/ricevuti.
*/
void CMail::Log(const char* log,int len)
{
	if(!m_Mail.flag.log)
		return;
		
	if((!log || !*log) || len <= 0)
		return;
		
	// apre il file per il log
	if((m_Mail.hLog = _lopen(m_Mail.log,OF_WRITE))!=HFILE_ERROR)
	{
		static char buffer[MAX_TEXTLINE+1];
		char* p_buf = buffer;
		char* p_log = (char*)log;
		int i,index = 0;

		// posiziona alla fine del file
		_llseek(m_Mail.hLog,0L,2);
		_lwrite(m_Mail.hLog,"[",1);

		memset(buffer,'\0',sizeof(buffer));

		// trasla i \r\n in #13#10 nel buffer locale
		while(*p_log && (p_log-log) < len)
		{
			if(*p_log=='\r' || *p_log=='\n')
			{
				for(i = 0; i < 3; i++)
				{
					// scarica il buffer sul file
					if(index > (sizeof(buffer)-1))
					{
						_lwrite(m_Mail.hLog,buffer,index);
						memset(buffer,'\0',sizeof(buffer));
						p_buf = buffer;
						index = 0;
					}

					*p_buf++ = (char)(i==0 ? '#' : (i==1 ? '1' : (i==2 ? (*p_log=='\r' ? '3' : '0') : '0')));
					index++;
				}
				
				p_log++;
				continue;
			}
			
			// scarica il buffer sul file
			if(index > (sizeof(buffer)-1))
			{
				_lwrite(m_Mail.hLog,buffer,index);
				memset(buffer,'\0',sizeof(buffer));
				p_buf = buffer;
				index = 0;
			}

			*p_buf++ = *p_log++;
			index++;
		}

		// scarica il buffer sul file
		if(index!=0)
		{
			_lwrite(m_Mail.hLog,buffer,index);
			memset(buffer,'\0',sizeof(buffer));
			p_buf = buffer;
			index = 0;
		}
			
		_lwrite(m_Mail.hLog,"]\r\n",3);
		_lclose(m_Mail.hLog);
	}
}

/*
	MailBox()

	Registra i dati inviati/ricevuti nelle mailbox di output/input.
*/
int CMail::MailBox(LPCSTR buffer,int iBufLen,int iMbx)
{
	HFILE hFile;

	// apre la mailbox
	if((hFile = _lopen((iMbx==0 ? m_Mail.inmbx : m_Mail.outmbx),OF_WRITE))==HFILE_ERROR)
		hFile = _lcreat((iMbx==0 ? m_Mail.inmbx : m_Mail.outmbx),0);

	if(hFile!=HFILE_ERROR)
	{
		// posiziona alla fine del file e trascrive i dati
		_llseek(hFile,0L,2);
		_lwrite(hFile,buffer,iBufLen);
		_lclose(hFile);
	}

	return(0);
}

/*
	TranslateCrLf()
	
	Trasla i \n in \r\n. Il buffer di destinazione deve avere (nella peggiore delle ipotesi,
	ossia nell'eventualita' in cui sia costituito unicamente da \n) una dimensione pari al 
	doppio + 1 della stringa da traslare.
*/
void CMail::TranslateCrLf(const char* string,char* buffer)
{
	register char* p_string = (char*)string;
	register char* p_buffer = buffer;

	// scorre la stringa sorgente ed effettua la traslazione
	while(*p_string)
	{
		if(*p_string=='\n')
		{
			*p_buffer++ = '\r';
			*p_buffer++ = '\n';
			p_string++;
			continue;
		}
		
		*p_buffer++ = *p_string++;
	}
	
	*p_buffer = '\0';
}

/*
	StripPath()

	Elimina il pathname dal nome file.
*/
void CMail::StripPath(const char* filename,char* buffer)
{
	char* p;
	
	strcpy(buffer,filename);
	
	if((p = strchr(buffer,'\\'))!=NULL)
	{
		strrev(buffer);
		*strchr(buffer,'\\') = '\0';
		strrev(buffer);
	}
}

/*
	GetScriptOpt()
	
	Carica le opzioni dal file script.
*/
int CMail::GetScriptOpt(char* buffer)
{
	int i = 0;
	register char* p;

	m_Mail.flag.message = m_Mail.flag.attach = 0;
	
	// analizza la linea di comando
	for(p = strtok(buffer," "); p && *p; p = strtok(NULL," "))
	{
		// trovata un opzione
		if(*p=='-')
		{
			switch(*++p)
			{    
				// subject dell'email
				case 's':
					if((*p+1))
					{
						strcpyn(m_Mail.subject,p+1,sizeof(m_Mail.subject));
						i++;
					}
					break;

				// destinatario dell'email
				case 'e':
					if((*p+1))
					{
						int len = strlen(p+1);

						if(m_Mail.to)
							delete [] m_Mail.to,m_Mail.to = NULL;
						
						m_Mail.to = new char[len+1];

						if(m_Mail.to)
						{
							memset(m_Mail.to,'\0',len+1);
							strcpy(m_Mail.to,p+1);
							i++;
						}
						else
							return(0);
					}
					break;

				// cc
				case 'c':
					if((*p+1))
					{
						int len = strlen(p+1);

						if(m_Mail.cc)
							delete [] m_Mail.cc,m_Mail.cc = NULL;

						m_Mail.cc = new char[len+1];
						
						if(m_Mail.cc)
						{
							memset(m_Mail.cc,'\0',len+1);
							strcpy(m_Mail.cc,p+1);
						}
						else
							return(0);
					}
					break;

				// bcc
				case 'b':
					if((*p+1))
					{
						int len = strlen(p+1);

						if(m_Mail.bcc)
							delete [] m_Mail.bcc,m_Mail.bcc = NULL;

						m_Mail.bcc = new char[len+1];

						if(m_Mail.bcc)
						{
							memset(m_Mail.bcc,'\0',len+1);
							strcpy(m_Mail.bcc,p+1);
						}
						else
							return(0);
					}
					break;

				// testo
				case 'm':
					if((*p+1))
					{
						m_Mail.flag.message = 1;
						strcpyn(m_Mail.message,p+1,sizeof(m_Mail.message));
						i++;
					}
					break;

				// attachment
				case 'a':
					if((*p+1))
					{
						m_Mail.flag.attach = 1;
						strcpyn(m_Mail.attach,p+1,sizeof(m_Mail.attach));
						i++;
					}
					break;

				// opzione non prevista
				default:
					return(0);
					break;
			}
		}
	}

	// imposta su script
	m_Mail.flag.script = 1;
	
	return(i);
}

/*
	ParseRCPT()
	
	Restituisce gli indirizzi email presenti nella stringa.
	Assume che come separatore venga utilizzato il carattere ',' (virgola).
*/
char* CMail::ParseRCPT(char* line,char* buffer,int size)
{
	static char* p = NULL;
	char* b = buffer;
	
	if(!p)
		p = line;
		
	while((*p) && (*p!=',') && ((b-buffer) < size))
		*b++ = *p++;
	
	*b = '\0';
	
	if(!*p)
		p = '\0';
	else
		*p++;
	
	return(p);
}
