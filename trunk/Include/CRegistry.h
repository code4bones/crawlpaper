/*
	CRegistry.h
	Classe base per l'accesso al registro (SDK/MFC).
	Luca Piergentili, 07/08/00
	lpiergentili@yahoo.com
*/
#ifndef _CREGISTRY_H
#define _CREGISTRY_H 1

#include "window.h"

// id per le icone predefinite (windows.h)
enum IDI_PREDEFINED_ICON {
	IDI_APPLICATION_ICON,
	IDI_ASTERISK_ICON,
	IDI_EXCLAMATION_ICON,
	IDI_HAND_ICON,
	IDI_QUESTION_ICON,
	IDI_WINLOGO_ICON
};

// struttura per la registrazione del tipo file
struct REGISTERFILETYPE {
	char	extension[_MAX_EXT+1];		// estensione (.gzw)
	char	name[_MAX_PATH+1];			// nome del tipo (gzwfile)
	char	description[_MAX_PATH+1];	// descrizione (GZW compressed data)
	char	shell[_MAX_PATH+1];			// comando associato (c:\bin\gzwshell.exe)
	char	shellopenargs[_MAX_PATH+1];	// argomenti (%1)
	int	defaulticon;				// indice (base 0) per l'icona di default (0)
	HICON hicon;					// handle all'icona di default
	char	contenttype[_MAX_PATH+1];	// tipo mime (application/x-gzw-compressed)
};
typedef REGISTERFILETYPE* LPREGISTERFILETYPE;

/*
	CRegistry
*/
class CRegistry
{
public:
#if defined(_AFX) || defined(_AFX_DLL)
	CRegistry(HINSTANCE hInstance = AfxGetInstanceHandle())
#else
	CRegistry(HINSTANCE hInstance)
#endif
	{
		m_hInstance = hInstance;
	}
	virtual ~CRegistry() {}

	// tipo
	BOOL			RegisterFileType				(LPREGISTERFILETYPE lpRegFileType);
	BOOL			UnregisterFileType				(LPCSTR lpcszExtension);
	BOOL			GetRegisteredFileType			(LPCSTR lpcszExtension,LPREGISTERFILETYPE pFileType,BOOL bExtractIcon = FALSE);

	// icone per il tipo
	BOOL			SetIconForRegisteredFileType		(LPREGISTERFILETYPE lpRegFileType);
	HICON		GetIconForRegisteredFileType		(LPCSTR lpcszExtension,LPREGISTERFILETYPE pFileType = NULL,UINT nID = 0);
	HICON		GetSafeIconForRegisteredFileType	(LPCSTR lpcszExtension,LPREGISTERFILETYPE pFileType = NULL,UINT nID = 0);
	HICON		GetSystemIcon					(IDI_PREDEFINED_ICON id);
	
	// programma per il tipo (solo eseguibile, senza parametri ne opzioni)
	BOOL			GetProgramForRegisteredFileType	(LPCSTR lpcszFileName,LPSTR lpszProgram,int nSize);
	BOOL			ExecuteFileType				(LPCSTR lpcszFileName);

	// programma per il tipo e operazioni basiche
	BOOL			GetCommandForRegisteredFileType	(LPCSTR lpcszCommand,LPCSTR lpcszFileName,LPSTR lpszProgram,int nSize);
	BOOL			ShellFileType					(LPCSTR lpcszCommand,LPCSTR lpcszFileName);
#if 0
	inline BOOL	OpenFileType					(LPCSTR lpcszFileName) {return((int)::ShellExecute(NULL,"open",lpcszFileName,NULL,NULL,SW_SHOW) > 32);}
#else
	inline BOOL	OpenFileType					(LPCSTR lpcszFileName) {return(ShellFileType("open",lpcszFileName));}
#endif

	// estensione relativa al tipo mime
	LPSTR		GetContentTypeExtension			(LPCSTR lpcszContentType,LPSTR lpszExt,UINT nExtSize);

	// menu contestuale della shell
	BOOL			AddMenuEntryForRegisteredFileType	(LPCSTR lpcszExtension,LPCSTR lpcszMenuText,LPCSTR lpcszCommand);
	BOOL			RemoveMenuEntryForRegisteredFileType(LPCSTR lpcszExtension,LPCSTR lpcszMenuText);

private:
	HINSTANCE		m_hInstance;
};

#endif // _CREGISTRY_H
