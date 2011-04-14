/*
	CMailService.h
	Implementazione delle funzioni di base per l'accesso alle tabelle per i servizi SMTP/POP3.
	Le derivate devono accedere ai dati attraverso i suoi membri e non direttamente.
	Luca Piergentili, 23/02/00
	lpiergentili@yahoo.com
*/
#ifndef _CMAILSERVICE_H
#define _CMAILSERVICE_H 1

#include <stdio.h>
#include <stdlib.h>
#include "strcpyn.h"
#ifdef _WINDOWS
	#include "window.h"
	#ifdef WIN32_MFC
		#include "CMutexEx.h"
	#endif
#else
	#include "typedef.h"
#endif
#include "CBinFile.h"
#include "CDateTime.h"
#include "CSock.h"
#include "CMail.h"
#include "CMailDatabase.h"

class CMailService
{
public:
	CMailService(LPCSTR pDomainName,LPCSTR pMailPath,LPCSTR pDataPath,LPCSTR pLogPath);
	virtual ~CMailService();

	// gestione accounts/utenti	
	unsigned long	GetLastMailNumber	(LPCSTR);
	void			PutLastMailNumber	(LPCSTR,unsigned long);
	CMailboxTable*	OpenMailbox		(LPCSTR);	
	BOOL			ValidateEmail		(LPCSTR);
	BOOL			ValidateUser		(LPCSTR);
	BOOL			ValidatePass		(LPCSTR,LPCSTR);
	BOOL			ValidateAccount	(LPCSTR);
	BOOL			CreateAccount		(LPCSTR,LPCSTR,LPCSTR);
	BOOL			DeleteAccount		(LPCSTR);
	LPCSTR		ParseAccount		(LPCSTR,char*,int);
	int			CountAccounts		(void);

protected:
	void			Log				(LPCSTR,BOOL = TRUE);

#ifdef WIN32_MFC
	BOOL			Lock				(unsigned int timeout = 30);
	BOOL			Unlock			(void);
#endif

	CSock*		m_pRemoteSocket;						// socket connesso
	CSock		m_LocalSocket;							// socket locale
	char			m_szDomain		[(HOSTNAME_SIZE*2) + 1];	// dominio
	char			m_szMailpath		[_MAX_PATH + 1];		// pathname x mail
	char			m_szMailboxpath	[(_MAX_PATH*2) + 1];	// pathname x mailbox
	char			m_szDatapath		[_MAX_PATH + 1];		// pathname x base dati
	char			m_szLogpath		[_MAX_PATH + 1];		// nome file x log
	CBinFile		m_CLogfile;							// file di log
	char			m_szLog			[_MAX_PATH + 1];		// buffer x log
	CDateTime		m_CLocaldate;							// data/ora
	CMailUsersTable* mailusers;							// tabella per utenti
	CMailboxTable*	mailbox;								// tabella per mailbox
#ifdef WIN32_MFC
	CMutexEx*		m_pLogMutex;							// per sincronizzare l'accesso al file di log
	char			m_szLogMutexName	[_MAX_PATH + 1];		// nome del mutex
#endif
};

#endif // _CMAILSERVICE_H
