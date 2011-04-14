/*
	CRegistry.cpp
	Classe base per l'accesso al registro (SDK/MFC).
	Luca Piergentili, 07/08/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "CRegKey.h"
#include "CRegistry.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
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
	RegisterFileType()

	Registra il tipo file nel registro.
*/
BOOL CRegistry::RegisterFileType(LPREGISTERFILETYPE lpRegFileType)
{
	char key[REGKEY_MAX_KEY_NAME+1];
	char value[REGKEY_MAX_KEY_VALUE+1];
	CRegKey regkey;
	LONG reg;
	BOOL flag = TRUE;
	memset(key,'\0',sizeof(key));

	regkey.Attach(HKEY_LOCAL_MACHINE);
	
	// crea la chiave relativa all'estensione
	// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw
	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",lpRegFileType->extension);
	if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))!=ERROR_SUCCESS)
		reg = regkey.Create(HKEY_LOCAL_MACHINE,key);

	if(reg==ERROR_SUCCESS)
	{
		// imposta il valore di default con il nome relativo
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw -> (Default) = gzwfile
		regkey.SetValue(lpRegFileType->name,"");

		// imposta il tipo mime
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw -> Content Type = application/x-gzw-compressed
		regkey.SetValue(lpRegFileType->contenttype,"Content Type");

		regkey.Close();
	}
	else
		flag = FALSE;

	// crea la chiave per il nome relativo all'estensione
	// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\gzwfile
	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",lpRegFileType->name);
	if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))!=ERROR_SUCCESS)
		reg = regkey.Create(HKEY_LOCAL_MACHINE,key);
		
	if(reg==ERROR_SUCCESS)
	{
		// imposta il valore di default con la descrizione del tipo
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile -> (Default) = GZW compressed data
		regkey.SetValue(lpRegFileType->description,"");
		regkey.SetBinaryValue(0x00000000,"EditFlags");

		regkey.Close();

		// crea la chiave per l'icona di default
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile\DefaultIcon
		_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s\\DefaultIcon",lpRegFileType->name);
		if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))!=ERROR_SUCCESS)
			reg = regkey.Create(HKEY_LOCAL_MACHINE,key);
		if(reg==ERROR_SUCCESS)
		{
			// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile\DefaultIcon -> c:\bin\gzwshell.exe,0
			_snprintf(value,sizeof(key)-1,"%s,%d",lpRegFileType->shell,lpRegFileType->defaulticon);
			regkey.SetValue(value,"");

			regkey.Close();
		}
		else
			flag = FALSE;

		// crea la chiave per l'apertura del file tramite la shell
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile\shell\open\command
		_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s\\shell\\open",lpRegFileType->name);
		if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))!=ERROR_SUCCESS)
			reg = regkey.Create(HKEY_LOCAL_MACHINE,key);
		if(reg==ERROR_SUCCESS)
		{
			regkey.SetBinaryValue(0x00000001,"EditFlags");
			regkey.Close();
		}
		else
			flag = FALSE;
		
		_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s\\shell\\open\\command",lpRegFileType->name);
		if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))!=ERROR_SUCCESS)
			reg = regkey.Create(HKEY_LOCAL_MACHINE,key);
		if(reg==ERROR_SUCCESS)
		{
			// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile\shell\open\command -> c:\bin\gzwshell.exe
			_snprintf(value,sizeof(key)-1,"%s %s",lpRegFileType->shell,lpRegFileType->shellopenargs);
			regkey.SetValue(value,"");
			regkey.Close();
		}
		else
			flag = FALSE;
	}
	else
		flag = FALSE;

	regkey.Detach();

	return(flag);
}

/*
	UnregisterFileType()

	Elimina la registrazione per il tipo dal registro (includere il punto nell'estensione).
*/
BOOL CRegistry::UnregisterFileType(LPCSTR lpcszExtension)
{
	char key[REGKEY_MAX_KEY_NAME+1];
	char value[REGKEY_MAX_KEY_VALUE+1];
	DWORD valuesize;
	CRegKey regkey;
	BOOL flag = FALSE;

	regkey.Attach(HKEY_LOCAL_MACHINE);
	
	// cerca la chiave relativa all'estensione
	// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw
	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",lpcszExtension);
	if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
	{
		// ricava il nome relativo
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw -> (Default) = gzwfile
		memset(value,'\0',sizeof(value));
		valuesize = sizeof(value);
		if(regkey.QueryValue(value,"",&valuesize)==ERROR_SUCCESS)
		{
			regkey.Close();

			flag = TRUE;

			// elimina la chiave relativa all'estensione
			if(regkey.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes")==ERROR_SUCCESS)
			{
				if(flag)
					flag = regkey.DeleteKey(lpcszExtension);
				regkey.Close();
			}

			// elimina la chiave relativa al nome
			if(regkey.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes")==ERROR_SUCCESS)
			{
				if(flag)
					flag = regkey.DeleteKey(value);
				regkey.Close();
			}
		}
	}
	
	regkey.Detach();

	return(flag);
}

/*
	GetRegisteredFileType()

	Ricava le informazioni relative al tipo (includere il punto nell'estensione).
*/
/*SHFILEINFO GetTypeIcon(const char* szType)
{
	SHFILEINFO shIcon;
	memset(&shIcon, 0, sizeof(shIcon));
	SHGetFileInfo(szType, FILE_ATTRIBUTE_NORMAL, &shIcon, sizeof(shIcon), SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME | SHGFI_TYPENAME | SHGFI_ICON);
	return shIcon;
}*/
BOOL CRegistry::GetRegisteredFileType(LPCSTR lpcszExtension,LPREGISTERFILETYPE pFileType,BOOL bExtractIcon/* = FALSE*/)
{
	char program[_MAX_PATH+1];
	char index[5];
	int nIconIndex = 0;
	char key[REGKEY_MAX_KEY_NAME+1];
	char value[REGKEY_MAX_KEY_VALUE+1];
	DWORD valuesize;
	CRegKey regkey;

	memset(pFileType,'\0',sizeof(REGISTERFILETYPE));
	strcpyn(pFileType->extension,lpcszExtension,_MAX_EXT+1);

	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",lpcszExtension);

	regkey.Attach(HKEY_LOCAL_MACHINE);
		
	if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
	{
		memset(value,'\0',sizeof(value));
		valuesize = sizeof(value);

		if(regkey.QueryValue(value,"",&valuesize)==ERROR_SUCCESS)
		{
			strcpyn(pFileType->name,value,_MAX_PATH+1);
			
			char content[REGKEY_MAX_KEY_VALUE+1];
			DWORD contentsize;
			memset(content,'\0',sizeof(content));
			contentsize = sizeof(content);
			if(regkey.QueryValue(content,"Content Type",&contentsize)==ERROR_SUCCESS)
				strcpyn(pFileType->contenttype,content,_MAX_PATH+1);

			_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",pFileType->name);
			if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
			{
				memset(content,'\0',sizeof(content));
				contentsize = sizeof(content);
				if(regkey.QueryValue(content,"",&contentsize)==ERROR_SUCCESS)
					strcpyn(pFileType->description,content,_MAX_PATH+1);
			}
			
			_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s\\shell\\open\\command",pFileType->name);
			if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
			{
				memset(content,'\0',sizeof(content));
				contentsize = sizeof(content);
				if(regkey.QueryValue(content,"",&contentsize)==ERROR_SUCCESS)
					strcpyn(pFileType->shell,content,_MAX_PATH+1);
			}

			// cerca l'entrata "DefaultIcon"
			memset(program,'\0',sizeof(program));
			_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s\\DefaultIcon",value);
			if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
			{
				memset(value,'\0',sizeof(value));
				valuesize = sizeof(value);
				
				if(regkey.QueryValue(value,"",&valuesize)==ERROR_SUCCESS)
				{
					strlwr(value);
					int i;
					char* p = value;
					for(i = 0; i < sizeof(program)-1 && *p!=','; i++)
					{
						if(*p!=',')
							program[i] = *p;
						
						p++;
					}
					program[i] = '\0';
					while(*p==',' || *p==' ')
						p++;
					for(i = 0; i < sizeof(index)-1 && *p; i++)
						index[i] = *p++;
					index[i] = '\0';
					nIconIndex = atoi(index);
				}
			}
			// entrata "DefaultIcon" non trovata, sicuramente il tipo specifica il nome della classe
			// relativo al CLSID (es. CorelPhotoPaint.Image.6), quindi, dato che non so come cazzo
			// tirare fuori l'icona associata via OLE, cerca il programma associato all'estensione
			// e recupera la prima icona (quella con indice 0)
			else
			{
				GetProgramForRegisteredFileType(lpcszExtension,program,sizeof(program));
			}

			if(program[0]!='\0' && bExtractIcon && pFileType)
			{	
				pFileType->hicon = ::ExtractIcon(m_hInstance,program,nIconIndex);
			}
		}
		
		regkey.Close();
	}
	
	regkey.Detach();

	return(TRUE);
}

/*
	SetIconForRegisteredFileType()

	Imposta l'icona di default per il tipo file.
*/
BOOL CRegistry::SetIconForRegisteredFileType(LPREGISTERFILETYPE lpRegFileType)
{
	char key[REGKEY_MAX_KEY_NAME+1];
	char value[REGKEY_MAX_KEY_VALUE+1];
	CRegKey regkey;
	LONG reg;
	BOOL flag = TRUE;

	regkey.Attach(HKEY_LOCAL_MACHINE);
	
	// crea la chiave relativa all'estensione
	// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw
	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",lpRegFileType->extension);
	if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))!=ERROR_SUCCESS)
		reg = regkey.Create(HKEY_LOCAL_MACHINE,key);

	if(reg==ERROR_SUCCESS)
	{
		// imposta il valore di default con il nome relativo
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw -> (Default) = gzwfile
		regkey.SetValue(lpRegFileType->name,"");

		regkey.Close();
	}
	else
		flag = FALSE;

	// crea la chiave per il nome relativo all'estensione
	// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\gzwfile
	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",lpRegFileType->name);
	if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))!=ERROR_SUCCESS)
		reg = regkey.Create(HKEY_LOCAL_MACHINE,key);
		
	if(reg==ERROR_SUCCESS)
	{
		// imposta il valore di default con la descrizione del tipo
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile -> (Default) = GZW compressed data
		regkey.SetValue(lpRegFileType->description,"");
		regkey.SetBinaryValue(0x00000000,"EditFlags");

		regkey.Close();

		// crea la chiave per l'icona di default
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile\DefaultIcon
		_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s\\DefaultIcon",lpRegFileType->name);
		if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))!=ERROR_SUCCESS)
			reg = regkey.Create(HKEY_LOCAL_MACHINE,key);
		if(reg==ERROR_SUCCESS)
		{
			// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile\DefaultIcon -> c:\bin\gzwshell.exe,0
			_snprintf(value,sizeof(value)-1,"%s,%d",lpRegFileType->shell,lpRegFileType->defaulticon);
			regkey.SetValue(value,"");

			regkey.Close();
		}
		else
			flag = FALSE;
	}
	else
		flag = FALSE;

	regkey.Detach();

	return(flag);
}

/*
	GetIconForRegisteredFileType()

	Ricava l'handle relativo all'icona per il tipo file.
*/
HICON CRegistry::GetIconForRegisteredFileType(LPCSTR lpcszExtension,LPREGISTERFILETYPE pFileType/* = NULL*/,UINT nID/* = 0*/)
{
	HICON hIcon = (HICON)NULL;
	REGISTERFILETYPE registerfiletype = {0};
	
	if(nID==0)
	{
		hIcon = GetRegisteredFileType(lpcszExtension,pFileType ? pFileType : &registerfiletype,TRUE) ? (pFileType ? pFileType->hicon : registerfiletype.hicon) : NULL;
	}
	else
	{
		GetRegisteredFileType(lpcszExtension,pFileType ? pFileType : &registerfiletype,FALSE);
		hIcon = ::LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(nID));
		if(pFileType)
			pFileType->hicon = hIcon;
	}
	
	return(hIcon);
}


/*
	GetSafeIconForRegisteredFileType()

	Ricava l'handle relativo all'icona per il tipo file registrato, restituendo l'icona di sistema
	se non trova quella richiesta (includere il punto nell'estensione).
*/
HICON CRegistry::GetSafeIconForRegisteredFileType(LPCSTR lpcszExtension,LPREGISTERFILETYPE pFileType/* = NULL*/,UINT nID/* = 0*/)
{
	HICON hIcon;

	if((hIcon = GetIconForRegisteredFileType(lpcszExtension,pFileType,nID))==(HICON)NULL)
		hIcon = ::LoadIcon(NULL,IDI_WINLOGO);

	return(hIcon);
}

/*
	GetSystemIcon()

	Ricava l'handle relativo all'icona di sistema.
*/
HICON CRegistry::GetSystemIcon(IDI_PREDEFINED_ICON id)
{
	HICON hIcon;

	switch(id)
	{
		case IDI_APPLICATION_ICON:
			hIcon = ::LoadIcon(NULL,IDI_APPLICATION);
			break;
		case IDI_ASTERISK_ICON:
			hIcon = ::LoadIcon(NULL,IDI_ASTERISK);
			break;
		case IDI_EXCLAMATION_ICON:
			hIcon = ::LoadIcon(NULL,IDI_EXCLAMATION);
			break;
		case IDI_HAND_ICON:
			hIcon = ::LoadIcon(NULL,IDI_HAND);
			break;
		case IDI_QUESTION_ICON:
			hIcon = ::LoadIcon(NULL,IDI_QUESTION);
			break;
		case IDI_WINLOGO_ICON:
		default:
			hIcon = ::LoadIcon(NULL,IDI_WINLOGO);
			break;
	}

	return(hIcon);
}

/*
	GetProgramForRegisteredFileType()

	Ricava l'applicazione associata al tipo file registrato.
	Il nome del programma viene restituito eliminando gli eventuali parametri/opzioni presenti nel registro.

	LPCSTR lpcszFileName	nome del file (estensione inclusa o solo estensione) per il tipo
	LPSTR lpszProgram		buffer dove copiare il nome dell'eseguibile associato
	int nSize				dimensione del buffer
*/
BOOL CRegistry::GetProgramForRegisteredFileType(LPCSTR lpcszFileName,LPSTR lpszProgram,int nSize)
{
	char ext[_MAX_EXT+1];
	char key[REGKEY_MAX_KEY_NAME+1];
	char value[REGKEY_MAX_KEY_VALUE+1];
	DWORD valuesize;
	CRegKey regkey;
		
	// ricava l'estensione del file
	int len = strlen(lpcszFileName)-1;
	int i=0;
	for(; i < sizeof(ext)+2 && lpcszFileName[len-i]!='.'; i++) // sizeof-2 per '.' e '\0'
		ext[i] = lpcszFileName[len-i];
	ext[i++] = '.';
	ext[i] = '\0';
	strrev(ext);

	// programma associato al tipo file
	memset(lpszProgram,'\0',nSize);

	// ricava l'associazione dal registro
	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",ext);

	regkey.Attach(HKEY_LOCAL_MACHINE);
		
	if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
	{
		memset(value,'\0',sizeof(value));
		valuesize = sizeof(value);

		if(regkey.QueryValue(value,"",&valuesize)==ERROR_SUCCESS)
		{
			_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s\\shell\\open\\command",value);
			if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
			{
				memset(value,'\0',sizeof(value));
				valuesize = sizeof(value);
				
				if(regkey.QueryValue(value,"",&valuesize)==ERROR_SUCCESS)
				{
					strlwr(value);

					char* p = value;
					while(*p)
					{
						if(*p=='"')
							*p = ' ';
						p++;
					}

					p = value;
					while(*p==' ')
						p++;
					for(i = 0; i < nSize+1; i++)
					{
						if(*p==' ')
							if(stristr(lpszProgram,".exe"))
								break;

						lpszProgram[i] = *p++;
					}
					
					lpszProgram[i] = '\0';
				}
			}
		}
		
		regkey.Close();
	}
	
	regkey.Detach();

	return(lpszProgram[0]!='\0');
}

/*
	ExecuteFileType()

	Esegue l'applicazione associata al tipo file registrato.
	Ricava l'eseguibile relativo al tipo file e lo esegue passandogli come unico parametro
	il nome del file, ignorando gli eventuali parametri/opzioni presenti nel registro.

	LPCSTR lpcszFileName	nome del file (estensione inclusa) per il tipo
*/
BOOL CRegistry::ExecuteFileType(LPCSTR lpcszFileName)
{
	BOOL flag = FALSE;
	char program[_MAX_PATH+1];

	if(GetProgramForRegisteredFileType(lpcszFileName,program,sizeof(program)))
	{
		char cmd[(_MAX_PATH*2)+1] = {0};
		_snprintf(cmd,sizeof(cmd)-1,"%s %s",program,lpcszFileName);
		STARTUPINFO si = {0};
		si.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pi = {0};

		if(::CreateProcess(NULL,cmd,NULL,NULL,FALSE,0L,NULL,NULL,&si,&pi))
		{
			flag = TRUE;
			::CloseHandle(pi.hProcess);
		}
	}

	return(flag);
}

/*
	GetCommandForRegisteredFileType()

	Ricava l'applicazione associata al tipo file/comando registrati.
	Il nome del programma viene restituito includendo gli eventuali parametri/opzioni presenti nel registro.

	LPCSTR lpcszCommand		comando registrato ("open", "print", etc.)
	LPCSTR lpcszFileName	nome del file (estensione inclusa o solo estensione) per il tipo
	LPSTR lpszProgram		buffer dove copiare il nome dell'eseguibile associato
	int nSize				dimensione del buffer
*/
BOOL CRegistry::GetCommandForRegisteredFileType(LPCSTR lpcszCommand,LPCSTR lpcszFileName,LPSTR lpszProgram,int nSize)
{
	char ext[_MAX_EXT+1];
	char key[REGKEY_MAX_KEY_NAME+1];
	char value[REGKEY_MAX_KEY_VALUE+1];
	DWORD valuesize;
	CRegKey regkey;
		
	// ricava l'estensione del file
	int len = strlen(lpcszFileName)-1;
	int i = 0;
	for(; i < sizeof(ext)+2 && lpcszFileName[len-i]!='.'; i++) // sizeof-2 per '.' e '\0'
		ext[i] = lpcszFileName[len-i];
	ext[i++] = '.';
	ext[i] = '\0';
	strrev(ext);

	// programma associato al tipo file
	memset(lpszProgram,'\0',nSize);

	// ricava l'associazione dal registro
	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",ext);

	regkey.Attach(HKEY_LOCAL_MACHINE);
		
	if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
	{
		memset(value,'\0',sizeof(value));
		valuesize = sizeof(value);

		if(regkey.QueryValue(value,"",&valuesize)==ERROR_SUCCESS)
		{
			_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s\\shell\\%s\\command",value,lpcszCommand);
			if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
			{
				memset(value,'\0',sizeof(value));
				valuesize = sizeof(value);
				
				if(regkey.QueryValue(value,"",&valuesize)==ERROR_SUCCESS)
					strcpyn(lpszProgram,value,nSize);
			}
		}
		
		regkey.Close();
	}
	
	regkey.Detach();

	return(lpszProgram[0]!='\0');
}

/*
	ShellFileType()

	Esegue l'applicazione associata al tipo file/comando registrati.
	Ricava l'eseguibile relativo al tipo file e lo esegue passandogli come parametro (cerca il '%1')
	il nome del file, includendo gli eventuali parametri/opzioni presenti nel registro.

	LPCSTR lpcszCommand		comando registrato ("open", "print", etc.)
	LPCSTR lpcszFileName	nome del file (estensione inclusa) per il tipo
*/
BOOL CRegistry::ShellFileType(LPCSTR lpcszCommand,LPCSTR lpcszFileName)
{
	BOOL flag = FALSE;
	char program[_MAX_PATH+1];
	char filename[_MAX_PATH+1];

	strcpyn(filename,lpcszFileName,sizeof(filename));
	if(memcmp(filename,"http://",7)==0 && !stristr(filename,".htm"))
		strcpy(filename,".htm");

	if(GetCommandForRegisteredFileType(lpcszCommand,filename,program,sizeof(program)))
	{
		char cmd[(_MAX_PATH*2)+1] = {0};
		char* ext = strrchr(filename,'.');
		if(!ext)
			ext = "";

		if(stricmp(ext,".exe")!=0 && stricmp(ext,".com")!=0)
		{
			char* p = strstr(program,"%1");

			// per media player
			if(!p)
				p = strstr(program,"%L");

			if(p)
			{
				int i = p-program;
				strcpyn(cmd,program,sizeof(cmd));
				cmd[i] = '\0';
				strcat(cmd,lpcszFileName);
				
				p = strstr(program,"%1");
				// per media player
				if(!p)
					p = strstr(program,"%L");

				if(*(p+2))
					strcat(cmd+i+strlen(lpcszFileName),p+2);
			}
			else
				_snprintf(cmd,sizeof(cmd)-1,"%s \"%s\"",program,lpcszFileName);

			while((p = strchr(cmd,'%'))!=NULL)
			{
				char buffer[(_MAX_PATH*2)+1] = {0};
				char var[_MAX_PATH+1] = {0};
				char value[_MAX_PATH+1] = {0};
				int i = p-cmd+1;
				int n = 0;
				for(; n < sizeof(var)-2 && cmd[i]; n++,i++)
				{
					if(cmd[i]=='%')
						break;
					var[n] = cmd[i];
				}
				var[n] = '\0';

				if((p = getenv(var))!=NULL)
				{
					strcpyn(value,p,sizeof(value));
				
				strcpyn(buffer,cmd,sizeof(buffer));
				p = strchr(buffer,'%');
				strcpyn(p,value,sizeof(buffer)-(p-buffer));
				p = strchr(cmd,'%');
				strcat(buffer,p+strlen(var)+2);

				strcpyn(cmd,buffer,sizeof(cmd));
				}
				else
					break;
			}
		}
		else
			strcpyn(cmd,filename,sizeof(cmd));

		STARTUPINFO si = {0};
		si.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pi = {0};

		if(::CreateProcess(NULL,cmd,NULL,NULL,FALSE,0L,NULL,NULL,&si,&pi))
		{
			flag = TRUE;
			::CloseHandle(pi.hProcess);
		}
	}

	return(flag);
}

/*
	GetContentTypeExtension()

	Ricava l'estensione relativa al tipo mime.
*/
LPSTR CRegistry::GetContentTypeExtension(LPCSTR lpcszContentType,LPSTR lpszExt,UINT nExtSize)
{
	char* p = NULL;
	char key[REGKEY_MAX_KEY_NAME+1];
	char value[REGKEY_MAX_KEY_VALUE+1];
	DWORD valuesize;
	CRegKey regkey;
		
	memset(lpszExt,'\0',nExtSize);

	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\MIME\\Database\\Content Type\\%s",lpcszContentType);

	regkey.Attach(HKEY_LOCAL_MACHINE);
		
	if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
	{
		memset(value,'\0',sizeof(value));
		valuesize = sizeof(value);

		if(regkey.QueryValue(value,"Extension",&valuesize)==ERROR_SUCCESS)
		{
			strcpyn(lpszExt,value,nExtSize);
			p = lpszExt;
		}

		regkey.Close();
	}
	
	regkey.Detach();

	return(p);
}

/*
	AddMenuEntryForRegisteredFileType()

	Aggiunge l'entrata al menu contestuale della shell per il tipo file registrato.

	LPCSTR lpcszExtension	estensione del tipo (punto incluso)
	LPCSTR lpcszMenuText	testo per il menu contestuale della shell
	LPCSTR lpcszCommand		comando da associare all'entrata del menu (specificare gli
						eventuali parametri, ad es. '%1', ed opzioni
*/
BOOL CRegistry::AddMenuEntryForRegisteredFileType(LPCSTR lpcszExtension,LPCSTR lpcszMenuText,LPCSTR lpcszCommand)
{
	char key[REGKEY_MAX_KEY_NAME+1];
	char value[REGKEY_MAX_KEY_VALUE+1];
	CRegKey regkey;
	LONG reg;
	BOOL flag = TRUE;

	regkey.Attach(HKEY_LOCAL_MACHINE);
	
	// cerca la chiave relativa all'estensione
	// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw
	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",lpcszExtension);
	if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
	{
		// ricava il nome relativo
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw -> (Default) = gzwfile
		memset(value,'\0',sizeof(value));
		DWORD valuesize = sizeof(value);
		if(regkey.QueryValue(value,"",&valuesize)==ERROR_SUCCESS)
		{
			regkey.Close();

			// apre la chiave per il nome relativo all'estensione
			// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\gzwfile
			_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",value);
			if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))==ERROR_SUCCESS)
			{
				// crea la chiave per l'apertura del file tramite la shell
				// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile\shell\open\command
				_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s\\shell\\%s\\command",value,lpcszMenuText);
				if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))!=ERROR_SUCCESS)
					reg = regkey.Create(HKEY_LOCAL_MACHINE,key);
				if(reg==ERROR_SUCCESS)
				{
					// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile\shell\open\command -> c:\bin\gzwshell.exe
					_snprintf(value,sizeof(key)-1,"%s",lpcszCommand);
					regkey.SetValue(value,"");
					regkey.Close();
				}
				else
					flag = FALSE;
			}
			else
				flag = FALSE;
		}
	}

	regkey.Detach();

	return(flag);
}

/*
	RemoveMenuEntryForRegisteredFileType()

	Elimina l'entrata dal menu contestuale della shell per il tipo file registrato.

	LPCSTR lpcszExtension	estensione del tipo (punto incluso)
	LPCSTR lpcszMenuText	testo per il menu contestuale della shell
*/
BOOL CRegistry::RemoveMenuEntryForRegisteredFileType(LPCSTR lpcszExtension,LPCSTR lpcszMenuText)
{
	char key[REGKEY_MAX_KEY_NAME+1];
	char value[REGKEY_MAX_KEY_VALUE+1];
	CRegKey regkey;
	LONG reg;
	BOOL flag = TRUE;

	regkey.Attach(HKEY_LOCAL_MACHINE);
	
	// cerca la chiave relativa all'estensione
	// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw
	_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",lpcszExtension);
	if(regkey.Open(HKEY_LOCAL_MACHINE,key)==ERROR_SUCCESS)
	{
		// ricava il nome relativo
		// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\.gzw -> (Default) = gzwfile
		memset(value,'\0',sizeof(value));
		DWORD valuesize = sizeof(value);
		if(regkey.QueryValue(value,"",&valuesize)==ERROR_SUCCESS)
		{
			regkey.Close();

			// apre la chiave per il nome relativo all'estensione
			// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\gzwfile
			_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s",value);
			if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))==ERROR_SUCCESS)
			{
				// crea la chiave per l'apertura del file tramite la shell
				// es. HKEY_LOCAL_MACHINE\SOFTWARE\Classes\extfile\shell\open\command
				_snprintf(key,sizeof(key)-1,"SOFTWARE\\Classes\\%s\\shell",value);
				if((reg = regkey.Open(HKEY_LOCAL_MACHINE,key))==ERROR_SUCCESS)
				{
					regkey.DeleteKey(lpcszMenuText);
					regkey.Close();
				}
				else
					flag = FALSE;
			}
			else
				flag = FALSE;
		}
	}

	regkey.Detach();

	return(flag);
}
