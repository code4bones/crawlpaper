/*
	CBrowser.h
	Classe per accesso al browser.
	Luca Piergentili, 08/09/00
	lpiergentili@yahoo.com

	La parte relativa alla comunicazione DDE e' stata ripresa dal codice della
	"VWCL - The Virtual Windows Class Library" di Todd Osborne (http://www.vwcl.org).
*/
#ifndef _CBROWSER_H
#define _CBROWSER_H 1

#include "window.h"

// id per tipo browser
typedef enum _BROWSER_TYPE {
	NETSCAPE,
	IEXPLORE,
	MOZILLA,
	OPERA,
	CHROME,
	DEFAULT_INTERNET_BROWSER,
	UNKNOWN_INTERNET_BROWSER
} BROWSER_TYPE;

// nomi per il servizio DDE
#define NETSCAPE_SERVICE_NAME			"NETSCAPE"
#define IEXPLORER_SERVICE_NAME		"IEXPLORE"
#define MOZILLA_SERVICE_NAME			"FIREFOX"
#define OPERA_SERVICE_NAME			"OPERA"
#define CHROME_SERVICE_NAME			"CHROME"

#define NETSCAPE_CLIPBOARDFORMAT_NAME	"Netscape Bookmark"
#define IEXPLORER_CLIPBOARDFORMAT_NAME	"UniformResourceLocator"

/*
	CBrowser
*/
class CBrowser
{
public:
	CBrowser(LPCSTR lpcszBrowserName = NULL);
	virtual ~CBrowser() {}

	// browser
	BROWSER_TYPE	SetPreferredBrowser		(LPCSTR lpcszBrowserName);
	BOOL			GetDefaultBrowser		(LPSTR lpszProgram,UINT nSize);
	BOOL			GetBrowserProgramFilename(BROWSER_TYPE type,LPSTR lpszProgram,UINT nSize);
	BOOL			Browse				(LPCSTR lpcszUrl,BOOL bForceNewInstance = TRUE);

private:
	BROWSER_TYPE	m_browserType;
};

#endif // _CBROWSER_H
