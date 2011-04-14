/*
	CArchive.cpp
	Classe base per il supporto dei formati compressi.
	Luca Piergentili, 01/12/03
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
#include "win32api.h"
#include "CArchive.h"
#include "CzLib.h"
#include "CGzw.h"
#include "CZip.h"
#include "CRar.h"
#include "CWildCards.h"

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

static const char* pArchiveFormatsArray[] = {
	GZW_EXTENSION,
	ZIP_EXTENSION,
	RAR_EXTENSION,
	".arj",
	".lzh",
	".tar",
	".gz",
	".tgz",
	NULL
};

/*
	IsAudioFile()
*/
BOOL CArchiveFile::IsArchiveFile(LPCSTR lpcszFileName)
{
	// formato compresso, non necessariamente supportato dal player
	for(register int i=0; pArchiveFormatsArray[i]!=NULL; i++)
		if(striright(lpcszFileName,pArchiveFormatsArray[i])==0)
			return(TRUE);
	
	return(FALSE);
}


// .gzw

CArchiveInfoList* CArchiveFile::m_pGzwArchiveInfoList = NULL;

/*
	Gzw()
*/
int CArchiveFile::Gzw(LPCSTR lpcszGzwFile,LPCSTR lpcszFileToCompress)
{
	// comprime il file di input nel .gzw di output
	CGzw gzw;
	gzw.SetOperation(GZW_COMPRESS);
	gzw.SetRatio(9);
	gzw.SetAbsolutePathname(TRUE);
	gzw.SetRecursive(TRUE);
	gzw.SetInputFile(lpcszFileToCompress);
	gzw.SetOutputFile(lpcszGzwFile);
	int nRet = gzw.Gzw();
	return(nRet==GZW_SUCCESS ? 0 : nRet);
}

/*
	UnGzw()
*/
int CArchiveFile::UnGzw(LPCSTR lpcszGzwFile,LPCSTR lpcszFileToExtract,LPCSTR lpcszOutputDir,CArchiveInfoList* pArchiveInfoList)
{
	// estrae/lista il file dal .gzw
	if(pArchiveInfoList)
		m_pGzwArchiveInfoList = pArchiveInfoList;
	else
		m_pGzwArchiveInfoList = NULL;

	// se non viene specificato il file da estrarre e la directory di output invece di estrarre lista il contenuto del file compresso
	CGzw gzw;
	if(lpcszFileToExtract && lpcszOutputDir)
	{
		gzw.SetOperation(GZW_UNCOMPRESS);
		gzw.SetOutputFile(lpcszOutputDir);
		gzw.SetCallback(NULL);
	}
	else
	{
		gzw.SetOperation(GZW_LIST);
		gzw.SetOutputFile("callback");
		gzw.SetCallback(GzwCallback);
	}
	gzw.SetAbsolutePathname(0);
	gzw.SetRecursive(FALSE);
	gzw.SetInputFile(lpcszGzwFile);

	// per estrarre il file anche se non viene specificato il pathname
	char szSkeleton[_MAX_FILEPATH+1] = {"*"};
	if(lpcszFileToExtract)
		strcpyn(szSkeleton+1,lpcszFileToExtract,sizeof(szSkeleton)-1);
	// converte da '/' a '\'
	for(int i = 1; szSkeleton[i]; i++)
		if(szSkeleton[i]=='/')
			szSkeleton[i] = '\\';
	gzw.SetWildcardsInclude(szSkeleton,TRUE,TRUE);
	
	// estrae/lista
	int nRet = gzw.Gzw();
	return(nRet==GZW_SUCCESS ? 0 : nRet);
}

/*
	GetUnGzwErrorCodeDescription()
*/
LPCSTR CArchiveFile::GetUnGzwErrorCodeDescription(int /*nErrorCode*/)
{
	// da implementare...
	return("");
}

/*
	GzwCallback()
*/
LONG CArchiveFile::GzwCallback(WPARAM /*wParam*/,LPARAM lParam1,LPARAM /*lParam2*/)
{
	if(lParam1)
	{
		GZWHDR* pHeader = (GZWHDR*)lParam1;
		if(pHeader)
			// se non e' a null, significa che e' stata richiesta la lista del (contenuto del) file - vedi UnGzw()
			if(m_pGzwArchiveInfoList)
			{
				ARCHIVEINFO* info = (ARCHIVEINFO*)m_pGzwArchiveInfoList->Add();
				if(info)
				{
					// elimina l'identificativo del drive dal nome file contenuto nel file compresso
					char* p = strrchr(pHeader->pFileName,':');
					if(p)
						p++;
					if(!p)
						p = pHeader->pFileName;
					char szBuffer[_MAX_FILEPATH+1];
					strcpyn(szBuffer,p,sizeof(szBuffer));
					// converte i '\' in '/'
					while((p = strchr(szBuffer,'\\'))!=NULL)
						*p = '/';						
					strcpyn(info->name,szBuffer,_MAX_FILEPATH+1);
					info->size = pHeader->stGzwHdr.dwFileSize;
				}
			}
	}

	return(0L);
}

// .zip

CArchiveInfoList* CArchiveFile::m_pZipArchiveInfoList = NULL;

// il resto dei codici in unzip.h
#define PK_ALLOC_FAILURE		2608
#define PK_DLL_LOAD_FAILURE	1965

struct UNZIP_DLL_ERROR_CODES {
	int code;
	char* desc;
};

static const UNZIP_DLL_ERROR_CODES unzip_dll_error_codes[] = {
	PK_OK,			"no error",
	PK_COOL,			"no error",
	PK_WARN,			"warning",
	PK_ERR,			"error in zipfile",
	PK_BADERR,		"severe error in zipfile",
	PK_MEM,			"insufficient memory (during initialization)",
	PK_MEM2,			"insufficient memory (password failure)",
	PK_MEM3,			"insufficient memory (file decompression)",
	PK_MEM4,			"insufficient memory (memory decompression)",
	PK_MEM5,			"insufficient memory (not yet used)",
	PK_NOZIP,			"zipfile not found",
	PK_PARAM,			"bad or illegal parameters specified",
	PK_FIND,			"no files found",
	PK_DISK,			"disk full",
	PK_EOF,			"unexpected EOF",

	IZ_CTRLC,			"user hit ^C to terminate",
	IZ_UNSUP,			"no files found: all unsup. compr/encrypt.",
	IZ_BADPWD,		"no files found: all had bad password",

	PK_ALLOC_FAILURE,	"system memory allocation failure",
	PK_DLL_LOAD_FAILURE,"unzip dll not found",
	
	-1,				"unknown error"
};

/*
	UnZip()
*/
int CArchiveFile::UnZip(LPCSTR lpcszZipFile,LPCSTR lpcszFileToExtract,LPCSTR lpcszOutputDir,CArchiveInfoList* pArchiveInfoList)
{
	// estrae/lista il file dal .zip
	int infc = 0;
	char **infv = NULL; // array di puntatori per i files da includere (l'ultimo deve essere NULL)
	int exfc = 0;
	char **exfv = NULL; // array di puntatori per i files da escludere (l'ultimo deve essere NULL)
	HANDLE hDCL = NULL;
	LPDCL lpDCL = NULL;
	HANDLE hUF = NULL;
	LPUSERFUNCTIONS lpUserFunctions = NULL;
	int nRet = 0;

	hDCL = ::GlobalAlloc(GPTR,(DWORD)sizeof(DCL));
	if(!hDCL)
	{
		nRet = PK_ALLOC_FAILURE;
		goto done;
	}

	lpDCL = (LPDCL)::GlobalLock(hDCL);
	if(!lpDCL)
	{
		nRet = PK_ALLOC_FAILURE;
		goto done;
	}

	hUF = ::GlobalAlloc(GPTR,(DWORD)sizeof(USERFUNCTIONS));
	if(!hUF)
	{
		nRet = PK_ALLOC_FAILURE;
		goto done;
	}

	lpUserFunctions = (LPUSERFUNCTIONS)::GlobalLock(hUF);
	if(!lpUserFunctions)
	{
		nRet = PK_ALLOC_FAILURE;
		goto done;
	}

	// verifica se estrarre o listare il contenuto del file compresso
	if(pArchiveInfoList)
		m_pZipArchiveInfoList = pArchiveInfoList;
	else
		m_pZipArchiveInfoList = NULL;

	lpUserFunctions->password = UnZipDummyPassword;
	lpUserFunctions->print = UnzipDummyPrintOut;
	lpUserFunctions->sound = NULL;
	lpUserFunctions->replace = UnZipReplaceValue;
	lpUserFunctions->SendApplicationMessage = UnZipReceiveDllMessage;

	lpDCL->ncflag = 0;							// write to stdout if true
	lpDCL->fQuiet = 0;							// we want all messages. 1 = fewer messages, 2 = no messages
	lpDCL->ntflag = 0;							// test zip file if true
	lpDCL->nvflag = pArchiveInfoList ? 1 : 0;		// give a verbose listing if true
	lpDCL->nzflag = 0;							// display a zip file comment if true
	lpDCL->ndflag = 0;							// recreate directories (if==1) != 0, skip "../" if < 2
	lpDCL->naflag = 0;							// do not convert CR to CRLF
	lpDCL->nfflag = 0;							// do not freshen existing files only
	lpDCL->noflag = 1;							// over-write all files if true
	lpDCL->ExtractOnlyNewer = 0;					// do not extract only newer
	lpDCL->PromptToOverwrite = 0;					// "overwrite all" selected -> no query mode
	lpDCL->lpszZipFN = (char*)lpcszZipFile;			// the archive name
	lpDCL->lpszExtractDir = (char*)lpcszOutputDir;	// the directory to extract to. This is set to NULL if you are extracting to the current directory
	
	// files da includere/escludere
	infc = exfc = 0;
	infv = exfv = NULL;
	if(lpcszFileToExtract)
	{
		infv = new char* [2];
		infv[infc] = (char*)lpcszFileToExtract;
		infv[++infc] = NULL;
	}
	
	// unzip
	nRet = Wiz_SingleEntryUnzip(infc,infv,exfc,exfv,lpDCL,lpUserFunctions);
	
	if(infv)
		delete [] infv;

done:

	if(hDCL)
	{
		GlobalUnlock(hDCL);
		GlobalFree(hDCL);
	}
	if(hUF)
	{
		GlobalUnlock(hUF);
		GlobalFree(hUF);
	}

	return(nRet);
}

/*
	GetUnZipErrorCodeDescription()
*/
LPCSTR CArchiveFile::GetUnZipErrorCodeDescription(int nErrorCode)
{
	int i;
	for(i = 0; unzip_dll_error_codes[i].code!=-1; i++)
		if(unzip_dll_error_codes[i].code==nErrorCode)
			break;
	
	return(unzip_dll_error_codes[i].desc);
}

/*
	UnZipReceiveDllMessage()
*/
void WINAPI CArchiveFile::UnZipReceiveDllMessage(	unsigned long	ucsize,
										unsigned long	/*csiz*/,
										unsigned		/*cfactor*/,
										unsigned		/*mo*/,
										unsigned		/*dy*/,
										unsigned		/*yr*/,
										unsigned		/*hh*/,
										unsigned		/*mm*/,
										char			/*c*/,
										LPSTR		filename,
										LPSTR		/*methbuf*/,
										unsigned long	/*crc*/,
										char			/*fCrypt*/
										)
{
	if(m_pZipArchiveInfoList)
	{
		ARCHIVEINFO* info = (ARCHIVEINFO*)m_pZipArchiveInfoList->Add();
		if(info)
		{
			strcpyn(info->name,filename,_MAX_FILEPATH+1);
			info->size = ucsize;
		}
	}
}

/*
	UnzipDummyPrintOut()
*/
int WINAPI CArchiveFile::UnzipDummyPrintOut(LPSTR /*buf*/,unsigned long size)
{
	return((unsigned int)size);
}

/*
	UnZipReplaceValue()
*/
int WINAPI CArchiveFile::UnZipReplaceValue(char * /*filename*/)
{
	// this is where you decide if you want to replace, rename etc existing files
	return(1);
}

/*
	UnZipDummyPassword()
*/
int WINAPI CArchiveFile::UnZipDummyPassword(char * /*p*/,int /*n*/,const char * /*m*/,const char * /*name*/)
{
	// password entry routine - see password.c in the wiz directory for how this is actually implemented in WiZ
	return(1);
}

// .rar

CArchiveInfoList* CArchiveFile::m_pRarArchiveInfoList = NULL;

// il resto dei codici in unrar.h
#define RAR_SUCCESS			0

struct UNRAR_DLL_ERROR_CODES {
	int code;
	char* desc;
};

static const UNRAR_DLL_ERROR_CODES unrar_dll_error_codes[] = {
	RAR_SUCCESS,		"no error",
	ERAR_END_ARCHIVE,	"end of archive",
	ERAR_NO_MEMORY,	"not enough memory to initialize data structures",
	ERAR_BAD_DATA,		"broken data",
	ERAR_BAD_ARCHIVE,	"file is not valid RAR archive",
	ERAR_UNKNOWN_FORMAT,"unknown archive format",
	ERAR_EOPEN,		"file open error",
	ERAR_ECREATE,		"file create error",
	ERAR_ECLOSE,		"file create error",
	ERAR_EREAD,		"read error",
	ERAR_EWRITE,		"write error",
	ERAR_SMALL_BUF,	"buffer too small",
	ERAR_UNKNOWN,		"unknown",
	-1,				"unknown error"
};

/*
	UnRarCallback()
*/
int CALLBACK UnRarCallback(UINT msg,LONG UserData,LONG P1,LONG P2)
{
	enum {EXTRACT,TEST,PRINT};
	
	switch(msg)
	{
		case UCM_CHANGEVOLUME:
			if(P2==RAR_VOL_ASK)
			{
				char buffer[128];
				_snprintf(buffer,sizeof(buffer)-1,"Insert disk with %s to continue.",(char*)P1);
				return(::MessageBox(NULL,buffer,"UnRar()",MB_OKCANCEL|MB_ICONWARNING|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST)==IDOK ? 0 : -1);
			}
			return(0);
		
		case UCM_PROCESSDATA:
			if(UserData!=0 && *(int*)UserData==PRINT)
			{
				fflush(stdout);
				fwrite((char *)P1,1,P2,stdout);
				fflush(stdout);
			}
			return(0);
		
		case UCM_NEEDPASSWORD:
			::MessageBox(NULL,"Password required.","UnRar()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
			return(-1);
	}
	
	return(0);
}

/*
	UnRar()
*/
int CArchiveFile::UnRar(LPCSTR lpcszRarFile,LPCSTR lpcszFileToExtract,LPCSTR lpcszOutputDir,CArchiveInfoList* pArchiveInfoList)
{
	// estrae/lista il file dal .rar
	int nOpenMode = 0;

	// verifica se estrarre o listare il contenuto del file compresso
	if(pArchiveInfoList)
		m_pRarArchiveInfoList = pArchiveInfoList;
	else
		m_pRarArchiveInfoList = NULL;

	if(lpcszFileToExtract && lpcszOutputDir)
		nOpenMode = RAR_OM_EXTRACT;
	else
		nOpenMode = RAR_OM_LIST;

	HANDLE hArcData = NULL;
	BOOL bNeedToSkip = TRUE;
	int PFCode = ERAR_UNKNOWN;
	char CmtBuf[16384] = {0};
	struct RARHeaderDataEx HeaderData = {0};
	struct RAROpenArchiveDataEx OpenArchiveData = {0};

	OpenArchiveData.ArcName = (char*)lpcszRarFile;
	OpenArchiveData.CmtBuf = CmtBuf;
	OpenArchiveData.CmtBufSize = sizeof(CmtBuf);
	OpenArchiveData.OpenMode = nOpenMode;
	hArcData = RAROpenArchiveEx(&OpenArchiveData);
	if(OpenArchiveData.OpenResult!=0)
		return(ERAR_EOPEN);

	RARSetCallback(hArcData,UnRarCallback,0);

	HeaderData.CmtBuf = CmtBuf;
	HeaderData.CmtBufSize = sizeof(CmtBuf);

	PFCode = ERAR_END_ARCHIVE;
	
	while(RARReadHeaderEx(hArcData,&HeaderData)==0)
	{
		bNeedToSkip = TRUE;

		QWORD UnpSize = HeaderData.UnpSize + (((QWORD)HeaderData.UnpSizeHigh) << 32);
		if(UnpSize > 0)
		{			
			if(nOpenMode==RAR_OM_EXTRACT)
			{
				// rar usa \ e non / come winzip
				char szBuffer[_MAX_FILEPATH+1];
				char szFileName[_MAX_FILEPATH+1];
				strcpyn(szFileName,lpcszFileToExtract,sizeof(szFileName));
				while(substr(szFileName,"/","\\",szBuffer,sizeof(szBuffer)))
				{
					strcpyn(szFileName,szBuffer,sizeof(szFileName));
					memset(szBuffer,'\0',sizeof(szBuffer));
				}

				CWildCards wildCards;
				if(wildCards.Match(szFileName,HeaderData.FileName))
				{
					char szOutputDir[_MAX_FILEPATH+1];
					strcpyn(szOutputDir,lpcszOutputDir,sizeof(szOutputDir));
					::EnsureBackslash(szOutputDir,sizeof(szOutputDir));
					char szOutputFile[_MAX_FILEPATH+1];
					char* pFileName = strrchr(HeaderData.FileName,'\\');
					if(pFileName)
						pFileName++;
					else
						pFileName = HeaderData.FileName;
					_snprintf(szOutputFile,sizeof(szOutputFile)-1,"%s%s",szOutputDir,pFileName);
					if(RARProcessFile(hArcData,RAR_EXTRACT,NULL,szOutputFile)!=0)
					{
						PFCode = ERAR_END_ARCHIVE;
						break;
					}
					else
						PFCode = RAR_SUCCESS;

					bNeedToSkip = FALSE;
				}
			}
			else if(nOpenMode==RAR_OM_LIST)
			{
				if(m_pRarArchiveInfoList)
				{
					ARCHIVEINFO* info = (ARCHIVEINFO*)m_pRarArchiveInfoList->Add();
					if(info)
					{
						// rar usa '\' e non '/' come winzip
						char szBuffer[_MAX_FILEPATH+1];
						char szFileName[_MAX_FILEPATH+1];
						strcpyn(szFileName,HeaderData.FileName,sizeof(szFileName));
						while(substr(szFileName,"\\","/",szBuffer,sizeof(szBuffer)))
						{
							strcpyn(szFileName,szBuffer,sizeof(szFileName));
							memset(szBuffer,'\0',sizeof(szBuffer));
						}

						strcpyn(info->name,szFileName,_MAX_FILEPATH+1);
						info->size = UnpSize;
						PFCode = RAR_SUCCESS;
					}
				}
			}
		}

		if(bNeedToSkip)
			if(RARProcessFile(hArcData,RAR_SKIP,NULL,NULL)!=0)
			{
				PFCode = ERAR_END_ARCHIVE;
				break;
			}
	}

	RARCloseArchive(hArcData);

	return(PFCode==RAR_SUCCESS ? 0 : PFCode);
}

/*
	GetUnRarErrorCodeDescription()
*/
LPCSTR CArchiveFile::GetUnRarErrorCodeDescription(int nErrorCode)
{
	int i = 0;
	for(; unrar_dll_error_codes[i].code!=-1; i++)
		if(unrar_dll_error_codes[i].code==nErrorCode)
			break;
	
	return(unrar_dll_error_codes[i].desc);
}

// classe di supporto

/*
	CArchiveFile()
*/
CArchiveFile::CArchiveFile()
{
	// se la mappa per i formati/gestori e' vuota inserisce i formati gestiti
	ARCHIVEHANDLER* handler;
	
	// .gzw
	handler = (ARCHIVEHANDLER*)m_listArchiveHandlers.Add();
	if(handler)
	{
		handler->type = ARCHIVETYPE_GZW;
		strcpyn(handler->ext,GZW_EXTENSION,_MAX_EXT+1);
		handler->lpfnListArchive = UnGzw;
		handler->lpfnExtractFromArchive = UnGzw;
		handler->lpfnGetErrorCodeDescription = GetUnGzwErrorCodeDescription;
	}

	// .rar
	handler = (ARCHIVEHANDLER*)m_listArchiveHandlers.Add();
	if(handler)
	{
		handler->type = ARCHIVETYPE_RAR;
		strcpyn(handler->ext,RAR_EXTENSION,_MAX_EXT+1);
		handler->lpfnListArchive = UnRar;
		handler->lpfnExtractFromArchive = UnRar;
		handler->lpfnGetErrorCodeDescription = GetUnRarErrorCodeDescription;
	}
	
	// .zip
	handler = (ARCHIVEHANDLER*)m_listArchiveHandlers.Add();
	if(handler)
	{
		handler->type = ARCHIVETYPE_ZIP;
		strcpyn(handler->ext,ZIP_EXTENSION,_MAX_EXT+1);
		handler->lpfnListArchive = UnZip;
		handler->lpfnExtractFromArchive = UnZip;
		handler->lpfnGetErrorCodeDescription = GetUnZipErrorCodeDescription;
	}
}

/*
	GetArchiveType()

	Restituisce il tipo di formato relativo al file di input (.gzw, .zip, .rar, etc.).
*/
ARCHIVETYPE CArchiveFile::GetArchiveType(LPCSTR lpcszCompressedFile)
{
	ITERATOR iter = NULL;
	ARCHIVEHANDLER* handler = NULL;

	// per poter ricevere sia l'estensione (con il punto) sia il nome file
	char* pExt = (char*)strrchr(lpcszCompressedFile,'.');
	if(!pExt)
		pExt = (char*)lpcszCompressedFile;

	// cerca il gestore associato al formato in base all'estensione del file
	if((iter = m_listArchiveHandlers.First())!=(ITERATOR)NULL)
	{
		do
		{
			handler = (ARCHIVEHANDLER*)iter->data;
			
			if(handler)
				if(stricmp(pExt,handler->ext)==0)
					return(handler->type);

			iter = m_listArchiveHandlers.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	return(ARCHIVETYPE_UNSUPPORTED);
}

/*
	GetHandler()

	Restituisce il puntatore al gestore per il formato (.gzw, .zip, .rar, etc.) specificato.
*/
ARCHIVEHANDLER* CArchiveFile::GetHandler(LPCSTR lpcszType)
{
	ITERATOR iter = NULL;
	ARCHIVEHANDLER* handler = NULL;

	// cerca il gestore associato al formato in base all'estensione del file
	char* pExt;
	if((pExt = (char*)strrchr(lpcszType,'.'))!=NULL)
		if((iter = m_listArchiveHandlers.First())!=(ITERATOR)NULL)
		{
			do
			{
				handler = (ARCHIVEHANDLER*)iter->data;
				
				if(handler)
					if(stricmp(pExt,handler->ext)==0)
						return(handler);

				iter = m_listArchiveHandlers.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}

	return(NULL);
}

/*
	Map()

	Inserisce il file compresso nella lista interna, restituendo l'indice da usare come lparam
	per associare al file compresso un qualsiasi elemento.
	Il nome del file compresso associato all'indice viene recuperato passando l'lparam a GetParent().
*/
LPARAM CArchiveFile::Map(LPCSTR lpcszCompressedFile)
{
	ITERATOR iter;
	ITEM* item;
	int nIndex = -1;
	int nCount = 0;

	// se non trova il file compresso nella lista lo inserisce
	if((iter = m_listArchiveFiles.First())!=(ITERATOR)NULL)
	{
		do
		{
			item = (ITEM*)iter->data;
			
			if(item)
			{
				if(nCount <= item->index)
					nCount = item->index+1;

				if(strcmp(lpcszCompressedFile,item->item)==0)
				{
					nIndex = item->index;
					break;
				}
			}

			iter = m_listArchiveFiles.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	if(nIndex==-1)
	{
		item = (ITEM*)m_listArchiveFiles.Add();
		if(item)
		{
			strcpyn(item->item,lpcszCompressedFile,_MAX_ITEM+1);
			nIndex = item->index = nCount;
		}
	}

	// restituisce l'indice associato al file compresso, da usare come lparam per l'elemento
	return((LPARAM)nIndex);
}

/*
	GetParent()

	Restituisce il nome del file compresso associato all'indice (l'lparam dell'elemento).
	Il nome del file compresso deve essere stato inserito tramite Map().
*/
LPCSTR CArchiveFile::GetParent(LPARAM lParam)
{
	ITEM* item;
	int nIndex = (int)lParam;

	if((item = (ITEM*)m_listArchiveFiles.GetAt(nIndex))!=(ITEM*)NULL)
		return(item->item);

	return(NULL);
}

/*
	List()

	Lista il contenuto del file compresso nella lista interna.
*/
BOOL CArchiveFile::List(LPCSTR lpcszCompressedFile)
{
	BOOL bFound = FALSE;

	// cerca il gestore associato al formato
	ARCHIVEHANDLER* handler = GetHandler(lpcszCompressedFile);

	// lista il contenuto del file compresso
	if(handler)
	{
		m_listArchiveInfo.DeleteAll();
		if(handler)
			bFound = handler->lpfnListArchive(lpcszCompressedFile,NULL,NULL,&m_listArchiveInfo)==0;
	}

	return(bFound);
}

/*
	Extract()

	Estrae il file specificato dal file compresso nella direcory di output.
*/
LONG CArchiveFile::Extract(LPCSTR lpcszCompressedFile,LPCSTR lpcszFileToExtract,LPCSTR lpcszOutputDir,BOOL bCacheExistingFiles/* = TRUE*/)
{
	LONG lRet = (LONG)-1L;

	// cerca il gestore associato al formato
	ARCHIVEHANDLER* handler = GetHandler(lpcszFileToExtract);

	// se esiste un gestore associato al formato, estrae il file
	if(handler)
	{
		// se il file gia' esiste e non viene indicato altrimenti non ri-estrae
		BOOL bNeedToExtract = TRUE;
		
		if(bCacheExistingFiles)
		{
			char szExistingFile[_MAX_FILEPATH+1];
			_snprintf(szExistingFile,sizeof(szExistingFile)-1,"%s%s",lpcszOutputDir,lpcszCompressedFile);
			if(::FileExist(szExistingFile))
				bNeedToExtract = FALSE;
		}

		if(bNeedToExtract)
			lRet = handler->lpfnExtractFromArchive(lpcszFileToExtract,lpcszCompressedFile,lpcszOutputDir,NULL);
		else
			lRet = 0L;
	}

	return(lRet);
}

/*
	GetErrorCodeDescription()

	Ricava la descrizione relativa al codice d'errore.
*/
LPCSTR CArchiveFile::GetErrorCodeDescription(LONG lRet,LPCSTR lpcszType)
{
	LPSTR pRet = "";

	// cerca il gestore associato al formato
	ARCHIVEHANDLER* handler = GetHandler(lpcszType);

	// se esiste un gestore associato al formato, ricava la descrizione dell'errore
	if(handler)
		pRet = (char*)handler->lpfnGetErrorCodeDescription(lRet);

	return(pRet);
}
