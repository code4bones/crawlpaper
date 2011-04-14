/*
	CPop3.h
	Classe derivata per servizio POP3.
	Luca Piergentili, 12/01/00
	lpiergentili@yahoo.com
*/
#ifndef _CPOP3_H
#define _CPOP3_H 1

#include "typedef.h"
#include <stdlib.h>
#include "strcpyn.h"
#include "CNodeList.h"
#include "CTextFile.h"
#include "CSock.h"
#include "CConnectSock.h"
#include "CListenSock.h"
#include "CMail.h"
#include "CMailService.h"

// stati POP3
enum POP3STATE {
	POP3_USER,
	POP3_PASS,
	POP3_STAT,
	POP3_TOP,
	POP3_LIST,
	POP3_RETR,
	POP3_DELE,
	POP3_RSET,
	POP3_NOOP,
	POP3_QUIT,
	POP3_READY,
	POP3_START,
	POP3_STOP
};

/*
	CPop3 (deriva da CMailService per l'accesso ai dati).
*/
class CPop3 : public CMailService
{
public:
	CPop3(const char* pDomain,const char* pMailpath,const char* pDatapath,const char* pLogpath);
	virtual ~CPop3();
	
	// servizio
	virtual const char*	StartService	(CSock* pSocket);
	virtual void		ResetService	(CSock* pSocket = NULL);
	virtual const char*	ParseCommand	(POP3STATE& current_pop3state,const char* pCommand,int nLen);

	// protocollo
	BOOLEAN			GetMailboxData	(long& nMessages,unsigned long& nSize);
	const char*		ReadMessage	(const char* pFilename);
	BOOLEAN			UpdateMessage	(const char* pFilename);
	BOOLEAN			DeleteMessages	(void);

private:
	// struttura (POP3STATLIST) e classe (CPop3StatList) per i dati relativi al messaggio da inviare
	// (comandi STAT/RETR/DELE/QUIT)
	// la struttura viene usata come elemento per la lista (statlist) presente in POP3MESSAGE
	// (ogni colloquio fa riferimento agli <n> messaggi presenti presso il server)
	struct POP3STATLIST {
		int			number;							// numero progressivo del messaggio
		unsigned long	size;							// dimensione del file
		char			buffer	[MAX_TEXTLINE + 2 + 1];		// buffer per lettura linea + crlf + null
		char			filename	[_MAX_PATH + _MAX_FNAME + 1];	// nome del file
		CTextFile		message;							// file
		int			deleted;							// flag per eliminazione
		
		inline void Reset(void)
		{
			number = 0;
			size = 0L;
			memset(buffer,'\0',sizeof(buffer));
			memset(filename,'\0',sizeof(filename));
			deleted = 0;
		}
	};

	class CPop3StatList : public CNodeList
	{
	public:
		CPop3StatList() {}
		virtual ~CPop3StatList() {CNodeList::DeleteAll();}
		BOOL PreDelete(ITERATOR iter)
		{
			delete ((POP3STATLIST*)iter->data);
			iter->data = (POP3STATLIST*)NULL;
			return(TRUE);
		}
		int Size(void) {return(sizeof(POP3STATLIST));}
	#ifdef _DEBUG
		const char* Signature(void) {return("CPop3StatList");}
	#endif
	};

	// struttura per il colloquio POP3
	struct POP3MESSAGE {
		char	usr	[MAX_USER_LEN + 1];	// utente
		char	psw	[MAX_USER_LEN + 1];	// password
		
		CPop3StatList*	statlist;		// lista per i messaggi da inviare

		inline void Init(void)
		{
			memset(usr,'\0',sizeof(usr));
			memset(psw,'\0',sizeof(psw));
			statlist = NULL;
		}
		
		inline void Reset(void)
		{
			memset(usr,'\0',sizeof(usr));
			memset(psw,'\0',sizeof(psw));
			if(statlist)
				delete statlist;
			statlist = NULL;
		}
	};

	POP3MESSAGE	pop3message;						// colloquio POP3
	POP3STATE		pop3state;						// status corrente colloquio POP3
	char			pop3response	[MAX_TEXTLINE + 1];		// risposta POP3

protected:
	const char*	GetPop3MessageUsr	(void)			{return((pop3message.usr));}
	const char*	GetPop3MessagePsw	(void)			{return((pop3message.psw));}
	void			SetPop3MessageUsr	(const char* s)	{strcpyn(pop3message.usr,s,sizeof(pop3message.usr));}
	void			SetPop3MessagePsw	(const char* s)	{strcpyn(pop3message.psw,s,sizeof(pop3message.psw));}
	
	const POP3STATE GetPop3State		(void)			{return(pop3state);}
	void			SetPop3State		(POP3STATE i)		{pop3state = i;}
	
	const char*	GetPop3Response	(void)			{return(pop3response);}
	const char*	SetPop3Response	(const char* s)	{strcpyn(pop3response,s,sizeof(pop3response)); return(pop3response);}
};

/*
	CConnectedPop3Socket

	Classe derivata per la gestione delle connessioni POP3 (lato client).
	Deriva da CConnectSock e da CPop3 per poter gestire l'oggetto come una connessione di tipo POP3, in
	tal modo l'oggetto generico connessione (CConnectSock) assume la capacita' (CPop3) di rispondere ai
	comandi POP3, potendo gestire le transazioni relative al servizio.
	Gli oggetti di tale tipo devono essere gestiti (attraverso una lista) dalla classe CListeningSmtpSocket.
*/
class CConnectedPop3Socket : public CConnectSock, public CPop3
{
public:
	CConnectedPop3Socket(
					const char*	domain,
					const char*	mailpath,
					const char*	datapath,
					const char*	logpath
					)
					: CPop3(domain,mailpath,datapath,logpath) {}
	
	~CConnectedPop3Socket() {}
};

/*
	CListeningPop3Socket

	Classe derivata per la gestione delle connessioni POP3 (lato server).
	Deriva da CListenSock per ereditare i membri base per la manipolazione della lista e per la creazione
	del socket d'ascolto.
	Le connessioni, di tipo CConnectedPop3Socket, vengono gestite attraverso una lista.
*/
class CListeningPop3Socket : public CListenSock
{
public:
	CListeningPop3Socket	(
						CWnd*		wnd,
						UINT			wsa,
						UINT			port,
						const char*	d,
						const char*	mp,
						const char*	dp,
						const char*	lp
						)
						: CListenSock(wnd,wsa,port)
						{
						strcpyn(domain,d,sizeof(domain)),
						strcpyn(mailpath,mp,sizeof(mailpath)),
						strcpyn(datapath,dp,sizeof(datapath)),
						strcpyn(logpath,lp,sizeof(logpath));
						}
	
	~CListeningPop3Socket() {CNodeList::DeleteAll();}

	// gestione degli elementi della lista
	CConnectedPop3Socket*	Insert		(WORD);
	CConnectedPop3Socket*	FindSocket	(SOCKET);
	CConnectedPop3Socket*	FindThread	(DWORD);
	void					Remove		(SOCKET);
	BOOL					PreDelete		(ITERATOR);

private:
	char			domain[(HOSTNAME_SIZE*2) + 1];	// dominio POP3
	char			mailpath[_MAX_PATH + 1];			// pathname x mail
	char			datapath[_MAX_PATH + 1];			// pathname x base dati
	char			logpath[_MAX_PATH + 1];			// pathname x log
	unsigned long	flags;						// flags POP3
};

#endif // _CPOP3_H
