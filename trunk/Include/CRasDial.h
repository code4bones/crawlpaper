/*
	CRasDial.h
	Classe base per interfaccia API RAS (WIN32).
	Luca Piergentili, 12/08/98
	l.pierge@teleline.es
*/
#ifndef _CRASDIAL_H
#define _CRASDIAL_H 1

// ambiente
#ifndef _WINDOWS
  #error _WINDOWS not defined
#endif

// headers
#define STRICT 1
#include <windows.h>
#include <lmcons.h>		// API LAN manager, per macro ??LEN
#include <ras.h>		// API Ras

// defines
#define RAS_ENTRYNAME_LEN	(RAS_MaxEntryName+1)// entrata phonebook
#define RAS_DOMAIN_LEN		(DNLEN+1)			// dominio
#define RAS_USERNAME_LEN		(UNLEN+1)			// nome utente
#define RAS_PASSWORD_LEN		(PWLEN+1)			// password
#if (WINVER >= 0x400)						// tipo/nome modem
 #define RAS_DEVICETYPE_LEN	(RAS_MaxDeviceType+1)
 #define RAS_DEVICENAME_LEN	(RAS_MaxDeviceName+1)
#else
 #define RAS_DEVICETYPE_LEN	128
 #define RAS_DEVICENAME_LEN	128
#endif 

/*
	RAS_CONN
	struttura per la connessione
*/
struct RAS_CONN {
	HRASCONN	hRasConn;						// handle per connessione
	int		iRetry;						// numero tentativi
	int		iDelay;						// pausa per tentativo successivo (in secondi)
	char		szEntryName[RAS_ENTRYNAME_LEN];	// nome connessione nel phonebook, 256 per WINVER >= 0x400
	char		szDomain[RAS_DOMAIN_LEN];		// dominio ('*'=default), 15
	char		szUserName[RAS_USERNAME_LEN];		// nome utente per login, 256
	char		szPassword[RAS_PASSWORD_LEN];		// password per login, 256
	char		szDeviceType[RAS_DEVICETYPE_LEN];	// tipo modem
	char		szDeviceName[RAS_DEVICENAME_LEN];	// nome modem
	char		szIPaddr[16];					// indirizzo IP
	int		iHour,iMin;					//$ durata connessione (da implementare con un timer)
};

class CRasDial
{
public:
	CRasDial(LPCSTR,LPCSTR,LPCSTR,LPCSTR,int = 5,int = 5);
	~CRasDial();

	BOOL		Connect			(void);
	BOOL		Disconnect		(void);
	DWORD	Dial				(void);
	DWORD	HangUp			(void);
	BOOL		CheckConn			(void);
	BOOL		GetConnectStatus	(void);
	DWORD	GetErrorCode		(void) {return(dwRasErrorCode);}
	LPCSTR	GetErrorString		(void) {return(szRasErrorCode);}

	LPCSTR	GetPhoneBookName(void) {return(ras_conn.szEntryName);}
	LPCSTR	GetDomain(void) {return(ras_conn.szDomain);}
	LPCSTR	GetUsr(void) {return(ras_conn.szUserName);}
	LPCSTR	GetPsw(void) {return(ras_conn.szPassword);}
	LPCSTR	GetIP(void) {return(ras_conn.szIPaddr);}

private:
	// tipi per i puntatori a funzione per il caricamento dinamico dell'API
	typedef DWORD (WINAPI *FP_RASDIAL)			(LPRASDIALEXTENSIONS,LPTSTR,LPRASDIALPARAMS,DWORD,LPVOID,LPHRASCONN);
	typedef DWORD (WINAPI *FP_RASHANGUP)		(HRASCONN);
	typedef DWORD (WINAPI *FP_RASGETERRSTRING)	(UINT,LPTSTR,DWORD);
	typedef DWORD (WINAPI *FP_RASGETCONNSTAT)	(HRASCONN,LPRASCONNSTATUS);
	typedef DWORD (WINAPI *FP_RASENUMCONN)		(LPRASCONN,LPDWORD,LPDWORD);
	typedef DWORD (WINAPI *FP_RASGETPROJINFO)	(HRASCONN,RASPROJECTION,LPVOID,LPDWORD);

	// handle per la dll e puntatori a funzione
	HINSTANCE			hRasDll;
	FP_RASDIAL		lpfnRasDial;
	FP_RASHANGUP		lpfnRasHangUp;
	FP_RASGETERRSTRING	lpfnRasGetErrorString;
	FP_RASGETCONNSTAT	lpfnRasGetConnectStatus;
	FP_RASENUMCONN		lpfnRasEnumConnections;
	FP_RASGETPROJINFO	lpfnRasGetProjectionInfo;

	// struttura interna e codici d'errore
	RAS_CONN			ras_conn;
	DWORD			dwRasErrorCode;
	char				szRasErrorCode[256];
};

#endif // _CRASDIAL_H
