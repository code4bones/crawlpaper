/*
	CSmtp.h
	Classe derivata per servizio SMTP.
	Luca Piergentili, 12/01/00
	lpiergentili@yahoo.com
*/
#ifndef _CSMTP_H
#define _CSMTP_H 1

#include "typedef.h"
#include <stdlib.h>
#include "strcpyn.h"
#include "CNodeList.h"
#include "CBinFile.h"
#include "CSock.h"
#include "CConnectSock.h"
#include "CListenSock.h"
#include "CMail.h"
#include "CMailService.h"

// flags
#define SMTP_FLAG_NONE				0x0000	// nessun azione
#define SMTP_FLAG_VALIDATE_MAILFROM	0x0002	// mittente
#define SMTP_FLAG_VALIDATE_RCPTTO		0x0004	// destinatario

// stati SMTP
enum SMTPSTATE {
	SMTP_HELO,
	SMTP_MAIL_FROM,
	SMTP_RCPT_TO,
	SMTP_DATA,
	SMTP_RSET,
	SMTP_NOOP,
	SMTP_QUIT,
	SMTP_READY,
	SMTP_START,
	SMTP_STOP
};

/*
	CSmtp
*/
class CSmtp : public CMailService
{
public:
	CSmtp(const char* pDomain,const char* pMailpath,const char* pDatapath,const char* pLogpath,unsigned long lFlags = SMTP_FLAG_NONE);
	virtual ~CSmtp();
	
	// servizio
	virtual const char*	StartService		(CSock* pSocket);
	virtual void		ResetService		(CSock* pSocket = NULL);
	virtual const char*	ParseCommand		(SMTPSTATE& current_smtpstate,const char* pCommand,int nLen);
	
	// protocollo
	const char*		ParseEmailAddress	(const char*,char*,int);
	void				GetMailSubject		(const char*);

private:
	// struttura per il colloquio SMTP
	struct SMTPMESSAGE {
		char			from		[MAX_USER_LEN + 1];			// mittente
		char			to		[MAX_USER_LEN + 1];			// destinatario
		char			recvdate	[MAX_DATE_STRING];			// data/ora ricezione (GMT)
		char			senddate	[MAX_DATE_STRING];			// data/ora invio (GMT)
		char			object	[MAX_SUBJECT_LEN+1];		// oggetto
		unsigned long	mailnumber;						// numero ultimo messaggio
		char			filename	[_MAX_PATH + _MAX_FNAME + 1];	// nome file x messaggio
		unsigned long	filesize;							// dimensione messaggio
		CBinFile		message;							// file x messaggio

		inline void Reset(void)
		{
			memset(from,'\0',sizeof(from));
			memset(to,'\0',sizeof(to));
			memset(recvdate,'\0',sizeof(recvdate));
			memset(senddate,'\0',sizeof(senddate));
			memset(object,'\0',sizeof(object));
			mailnumber = 0;
			memset(filename,'\0',sizeof(filename));
			filesize = 0;
		}
	};

	SMTPMESSAGE	smtpmessage;						// colloquio SMTP
	SMTPSTATE		smtpstate;						// status corrente colloquio SMTP
	unsigned long	smtpflags;						// flag per operazioni SMTP
	char			smtpresponse	[MAX_TEXTLINE + 1];		// risposta SMTP
	char			current		[WSA_BUF_SIZE + 1];		// dati ricevuti (per <CRLF>.<CRLF>)
	char			previous		[(WSA_BUF_SIZE*2) + 1];	// dati ricevuti (per <CRLF>.<CRLF>)
};


/*
	CConnectedSmtpSocket

	Classe derivata per la gestione delle connessioni SMTP (lato client).
	Deriva da CConnectSock e da CSmtp per poter gestire l'oggetto come una connessione di tipo SMTP, in
	tal modo l'oggetto generico connessione (CConnectSock) assume la capacita' (CSmtp) di rispondere ai
	comandi SMTP, potendo gestire le transazioni relative al servizio.
	Gli oggetti di tale tipo devono essere gestiti (attraverso una lista) dalla classe CListeningSmtpSocket.
*/
class CConnectedSmtpSocket : public CConnectSock, public CSmtp
{
public:
	CConnectedSmtpSocket(
					const char*	domain,
					const char*	mailpath,
					const char*	datapath,
					const char*	logpath,
					unsigned long	flags = 0L
					)
					: CSmtp(domain,mailpath,datapath,logpath,flags) {}
	
	~CConnectedSmtpSocket() {}
};

/*
	CListeningSmtpSocket

	Classe derivata per la gestione delle connessioni SMTP (lato server).
	Deriva da CListenSock per ereditare i membri base per la manipolazione della lista e per la creazione
	del socket d'ascolto.
	Le connessioni, di tipo CConnectedSmtpSocket, vengono gestite attraverso una lista.
*/
class CListeningSmtpSocket : public CListenSock
{
public:
	CListeningSmtpSocket	(
						CWnd*		wnd,
						UINT			wsa,
						UINT			port,
						const char*	d,
						const char*	mp,
						const char*	dp,
						const char*	lp,
						unsigned long	f = 0L
						)
						: CListenSock(wnd,wsa,port)
						{
						strcpyn(domain,d,sizeof(domain)),
						strcpyn(mailpath,mp,sizeof(mailpath)),
						strcpyn(datapath,dp,sizeof(datapath)),
						strcpyn(logpath,lp,sizeof(logpath)),
						flags = f;
						}
	
	~CListeningSmtpSocket() {CNodeList::DeleteAll();}

	// gestione degli elementi della lista
	CConnectedSmtpSocket*	Insert		(WORD);
	CConnectedSmtpSocket*	FindSocket	(SOCKET);
	CConnectedSmtpSocket*	FindThread	(DWORD);
	void					Remove		(SOCKET);
	BOOL					PreDelete	(ITERATOR);

private:
	char			domain[(HOSTNAME_SIZE*2) + 1];	// dominio SMTP
	char			mailpath[_MAX_PATH + 1];			// pathname x mail
	char			datapath[_MAX_PATH + 1];			// pathname x base dati
	char			logpath[_MAX_PATH + 1];			// pathname x log
	unsigned long	flags;						// flags SMTP
};

#endif // _CSMTP_H
