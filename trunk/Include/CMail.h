/*
	CMail.h
	Classe base per interfaccia SMTP/POP3 (SDK).
	Luca Piergentili, 24/09/96
	lpiergentili@yahoo.com
*/
#ifndef _CMAIL_H
#define _CMAIL_H 1

#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "CBase64.h"
#include "CUuenc.h"
#include "CSock.h"
#include "CDateTime.h"

// porte
#define SMTP_PORT			25
#define POP3_PORT			110

// interne
#define X_MAILER			"CMail"
#define MIME_VERSION		"1.0"
#define BOUNDARY			"------------18C822BA25A4"
#define BODYFILE			"CMail.tmp"

#define MAX_TEXTLINE		1000		// dimensione della linea di testo
#define MAX_EMAIL_LEN		128		// indirizzo email
#define MAX_SUBJECT_LEN		128		// oggetto email

// struttura bit-field per flags
struct MAILFLAGS {
	unsigned int smtp:1;
	unsigned int pop3:1;
	unsigned int leavemail:1;
	unsigned int log:1;
	unsigned int append:1;
	unsigned int create:1;
#if defined(_WINDOWS)
	unsigned int quiet:1;
#endif
	unsigned int message:1;
	unsigned int attach:1;
	unsigned int script:1;
};

// struttura per interfaccia
struct MAIL {
	char			smtphost		[HOSTNAME_SIZE+1];	// nome host SMTP
	unsigned int	smtpport;						// porta SMTP
	char			pop3host		[HOSTNAME_SIZE+1];	// nome host POP3
	unsigned int	pop3port;						// porta POP3
	char			realname		[MAX_USER_LEN+1];	// <Real Name>
	char			user			[MAX_USER_LEN+1];	// USER
	char			pass			[MAX_USER_LEN+1];	// PASS
	char			from			[MAX_EMAIL_LEN+1];	// From:
	char			sender		[MAX_EMAIL_LEN+1];	// Sender:
	char			reply		[MAX_EMAIL_LEN+1];	// Reply-To:
	char*		to;							// To:
	char*		cc;							// Cc:
	char*		bcc;							// Bcc:
	char			date			[MAX_DATE_STRING+1];// Date:
	char			subject		[_MAX_PATH+1];		// Subject:
	char			comments		[_MAX_PATH+1];		// X-Comment:
/**/	char			mime_version	[5];				// MIME-Version:
/**/	char			x_mailer		[64];			// X-Mailer:
	char			message		[_MAX_PATH+1];		// nome file testo (X-Comment...)
	char			attach		[_MAX_PATH+1];		// nome file attachment (X-Comment...)
/**/	char			encoded		[_MAX_PATH+1];		// nome file attachment convertito in base64
	char			script		[_MAX_PATH+1];		// nome file script
	char			inmbx		[_MAX_PATH+1];		// nome file mailbox di input
	char			outmbx		[_MAX_PATH+1];		// nome file mailbox di output
	char			log			[_MAX_PATH+1];		// nome file di log
	HFILE		hLog;
	MAILFLAGS		flag;
};

// struttura per messaggio
struct MAILMESSAGE {
	char			email		[MAX_EMAIL_LEN+1];
	char			date			[MAX_DATE_STRING+1];
	char			object		[MAX_SUBJECT_LEN+1];
	char			filename		[_MAX_PATH+1];
	char			attach		[_MAX_PATH+1];
};

// status corrente
enum MAIL_STATUS {
	MAIL_STATUS_VOID		= 0,
	MAIL_STATUS_PREPARING	= 1,
	MAIL_STATUS_OPEN		= 2,
	MAIL_STATUS_CONNECT		= 3,
	MAIL_STATUS_SEND		= 4,
	MAIL_STATUS_CLOSE		= 5,
	MAIL_STATUS_DONE		= 6,
	MAIL_STATUS_UNKNOW		= 15
};

enum MAILSTATUSLEN {
	MAIL_STATUS_LEN = ((int)MAIL_STATUS_UNKNOW+1)
};

struct MAILSTATUS {
	MAIL_STATUS mailstatus;
	int progress;
};

/*
	CMail()
*/
class CMail : public CSock, public CBase64, public CUuenc
{
public:
	CMail();
	virtual ~CMail();
	
	void				Reset		(void);

	inline void		SetPop3Host	(const char* host)			{strcpyn(m_Mail.pop3host,host,HOSTNAME_SIZE+1);}
	inline void		SetPop3Port	(const unsigned int port)	{m_Mail.pop3port = port;}
	inline void		SetSmtpHost	(const char* host)			{strcpyn(m_Mail.smtphost,host,HOSTNAME_SIZE+1);}
	inline void		SetSmtpPort	(const unsigned int port)	{m_Mail.smtpport = port;}
	inline void		SetRealName	(const char* name)			{strcpyn(m_Mail.realname,name,MAX_USER_LEN+1);}
	inline void		SetUser		(const char* user)			{strcpyn(m_Mail.user,user,MAX_USER_LEN+1);}
	inline void		SetPass		(const char* pass)			{strcpyn(m_Mail.pass,pass,MAX_USER_LEN+1);}
	inline void		SetFrom		(const char* from)			{strcpyn(m_Mail.from,from,MAX_EMAIL_LEN+1);}
	inline void		SetSender		(const char* sender)		{strcpyn(m_Mail.sender,sender,MAX_EMAIL_LEN+1);}
	inline void		SetReplyTo	(const char* reply)			{strcpyn(m_Mail.reply,reply,MAX_EMAIL_LEN+1);}
	void				SetTo		(const char* to);			// non specificare il destinatario (To) se si utilizza uno script
	void				SetCc		(const char* cc);
	void				SetBcc		(const char* bcc);
	inline void		SetDate		(const char* date)			{strcpyn(m_Mail.date,date,MAX_DATE_STRING+1);}
	inline void		SetSubject	(const char* subject)		{strcpyn(m_Mail.subject,subject,_MAX_PATH+1);}
	inline void		SetComments	(const char* comments)		{strcpyn(m_Mail.comments,comments,_MAX_PATH+1);}
	inline void		SetMessage	(const char* message)		{strcpyn(m_Mail.message,message,_MAX_PATH+1),m_Mail.flag.message = 1;}
	inline void		SetAttach		(const char* attach)		{strcpyn(m_Mail.attach,attach,_MAX_PATH+1),m_Mail.flag.attach = 1;}
	inline void		SetScript		(const char* script)		{strcpyn(m_Mail.script,script,_MAX_PATH+1),m_Mail.flag.script = 1;}
	inline void		SetInBox		(const char* inbox)			{strcpyn(m_Mail.inmbx,inbox,_MAX_PATH+1);}
	inline void		SetOutBox		(const char* outbox)		{strcpyn(m_Mail.outmbx,outbox,_MAX_PATH+1);}
	inline void		SetLog		(const char*);

	inline BOOL		ReceiveMail	(FPCALLBACK lpfnCallback = NULL,LPVOID lpVoid = NULL) {return(Receive(lpfnCallback,lpVoid)==0);}
	inline BOOL		SendMail		(FPCALLBACK lpfnCallback = NULL,LPVOID lpVoid = NULL) {return(Send(lpfnCallback,lpVoid)==0);}
	
	inline const char*	GetWinsockError(void)					{return(*m_szWsaError ? m_szWsaError : NULL);}
	inline const char*	GetMailError	(void)					{return(*m_szMailError ? m_szMailError : NULL);}

private:
	int				Receive		(FPCALLBACK lpfnCallback = NULL,LPVOID lpVoid = NULL);
	int				ReceiveHeader	(int,char*,int);
	int				ReceiveEmail	(char*,int,int);
	int				Stat			(void);

	int				Send			(FPCALLBACK lpfnCallback = NULL,LPVOID lpVoid = NULL);
	int				SendHeader	(FPCALLBACK lpfnCallback = NULL,LPVOID lpVoid = NULL);
	int				SendEmail		(FPCALLBACK lpfnCallback = NULL,LPVOID lpVoid = NULL);
	int				ReadScript	(int&);
	int				CreateMailBody	(void);
	int				CreateMixedBody(void);
	int				CreateTextBody	(void);
	int				CreateAttachBody(void);

	int				ReceiveData	(char*,int);
	int				SendData		(const char*);
	int				Ack			(char*,int);

	int				Encode		(char*,char*);

	void				Log			(const char*,int);
	int				MailBox		(const char*,int,int);
	inline int		InBox		(const char* s,int l) {return(MailBox(s,l,0));}
	inline int		OutBox		(const char* s,int l) {return(MailBox(s,l,1));}

	void				TranslateCrLf	(const char*,char*);
	void				StripPath		(const char*,char*);
	int				GetScriptOpt	(char*);
	char*			ParseRCPT		(char*,char*,int);

	inline void		ResetWinsockError(void)			{memset(m_szWsaError,'\0',sizeof(m_szWsaError));}
	inline void		ResetMailError	(void)			{memset(m_szMailError,'\0',sizeof(m_szMailError));}
	inline void		SetWinsockError(void)			{strcpyn(m_szWsaError,CSock::GetWSAErrorString(),sizeof(m_szWsaError));}
	inline void		SetMailError	(const char* error)	{strcpyn(m_szMailError,error,sizeof(m_szMailError));}
	
	char				m_szWsaError[128];
	char				m_szMailError[128];
	MAIL				m_Mail;
};

#endif // _CMAIL_H
