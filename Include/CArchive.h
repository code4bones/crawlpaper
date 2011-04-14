/*
	CArchive.h
	Classe base per il supporto dei formati compressi.
	Luca Piergentili, 01/12/03
	lpiergentili@yahoo.com
*/
#ifndef _CARCHIVE_H
#define _CARCHIVE_H 1

#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include "win32api.h"
#include "CFindFile.h"
#include "CNodeList.h"
#include "CzLib.h"
#include "CGzw.h"
#include "CZip.h"
#include "CRar.h"

/*
	EXTRACTFILEINFO
	struttura per il passaggio dei parametri per l'estrazione del file dall'archivio
*/
struct EXTRACTFILEINFO {
	LPARAM	lParam;
	LPSTR	lpszInputFileName;
	UINT		cbOutputFileName;
	LPSTR	lpszOutputFileName;
	LPSTR	lpszParentFileName;
	UINT		cbParentFileName;
};

/*
	ARCHIVEINFO
	elemento per la lista dei files contenuti nel file compresso
*/
struct ARCHIVEINFO {
	char name[_MAX_FILEPATH+1];
	QWORD size;
};

/*
	CArchiveInfoList
	classe per la lista dei files contenuti nel file compresso
*/
class CArchiveInfoList : public CNodeList
{
public:
	CArchiveInfoList() : CNodeList() {}
	virtual ~CArchiveInfoList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new ARCHIVEINFO);
	}	
	void* Initialize(void* pVoid)
	{
		ARCHIVEINFO* pData = (ARCHIVEINFO*)pVoid;
		if(!pData)
			pData = (ARCHIVEINFO*)Create();
		if(pData)
		{
			memset(pData->name,'\0',_MAX_FILEPATH+1);
			pData->size = 0L;
		}
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((ARCHIVEINFO*)iter->data)
			delete ((ARCHIVEINFO*)iter->data),iter->data = (ARCHIVEINFO*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(ARCHIVEINFO));}
#ifdef _DEBUG
	const char* Signature(void) {return("CArchiveInfoList");}
#endif
};

// tipi per le funzioni di servizio sul file compresso
typedef int (*PFNLISTARCHIVE)(LPCSTR lpcszArchive,LPCSTR lpcszFileName,LPCSTR lpcszOutputDir,CArchiveInfoList* pArchiveInfoList);
typedef int (*PFNEXTRACTFROMARCHIVE)(LPCSTR lpcszArchive,LPCSTR lpcszFileName,LPCSTR lpcszOutputDir,CArchiveInfoList* pArchiveInfoList);
typedef LPCSTR (*PFNGETERRORCODEDESCRIPTION)(int nErrorCode);

/*
	ARCHIVETYPE
	tipi supportati
*/
typedef enum _ARCHIVETYPE {
	ARCHIVETYPE_GZW,
	ARCHIVETYPE_ZIP,
	ARCHIVETYPE_RAR,
	ARCHIVETYPE_UNSUPPORTED
} ARCHIVETYPE;
 
/*
	ARCHIVEHANDLER
	elemento per la mappa tipo file/funzioni relative
*/
struct ARCHIVEHANDLER {
	ARCHIVETYPE type;
	char ext[_MAX_EXT+1];
	PFNLISTARCHIVE lpfnListArchive;
	PFNEXTRACTFROMARCHIVE lpfnExtractFromArchive;
	PFNGETERRORCODEDESCRIPTION lpfnGetErrorCodeDescription;
};

/*
	CArchiveHandlerList
	classe per la mappa tipo file/funzioni relative
*/
class CArchiveHandlerList : public CNodeList
{
public:
	CArchiveHandlerList() : CNodeList() {}
	virtual ~CArchiveHandlerList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new ARCHIVEHANDLER);
	}	
	void* Initialize(void* pVoid)
	{
		ARCHIVEHANDLER* pData = (ARCHIVEHANDLER*)pVoid;
		if(!pData)
			pData = (ARCHIVEHANDLER*)Create();
		if(pData)
		{
			pData->type = ARCHIVETYPE_UNSUPPORTED;
			memset(pData->ext,'\0',_MAX_EXT+1);
			pData->lpfnListArchive = NULL;
			pData->lpfnExtractFromArchive = NULL;
			pData->lpfnGetErrorCodeDescription = NULL;
		}
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((ARCHIVEHANDLER*)iter->data)
			delete ((ARCHIVEHANDLER*)iter->data),iter->data = (ARCHIVEHANDLER*)NULL;
		return(TRUE);
	}
	int Size(void) {return(-1);}
#ifdef _DEBUG
	const char* Signature(void) {return("CArchiveHandlerList");}
#endif
};

/*
	CArchiveFile
*/
class CArchiveFile
{
public:
	CArchiveFile();
	virtual ~CArchiveFile() {}

	static BOOL			IsArchiveFile				(LPCSTR lpcszFileName); // formato compresso, non necessariamente supportato dal player
	BOOL					IsSupportedFormat			(LPCSTR lpcszFileName) {return(GetHandler(lpcszFileName)!=NULL);} // formato compresso supportato dal player

	ARCHIVEHANDLER*		GetHandler				(LPCSTR lpcszType);
	ARCHIVETYPE			GetArchiveType				(LPCSTR lpcszCompressedFile);
	LPARAM				Map						(LPCSTR lpcszCompressedFile);
	LPCSTR				GetParent					(LPARAM lParam);
	BOOL					List						(LPCSTR lpcszCompressedFile);
	inline CArchiveInfoList*	GetList					(void) {return(&m_listArchiveInfo);}
	LONG					Extract					(LPCSTR lpcszCompressedFile,LPCSTR lpcszFileToExtract,LPCSTR lpcszOutputDir,BOOL bCacheExistingFiles = TRUE);
	LPCSTR				GetErrorCodeDescription		(LONG lRet,LPCSTR lpcszType);
	
private:
	// .gzw (compressione/decompressione)
	static CArchiveInfoList* m_pGzwArchiveInfoList;
	static int			Gzw						(LPCSTR lpcszGzwOutputFile,LPCSTR lpcszFileName);
	static int			UnGzw					(LPCSTR lpcszGzwInputFile,LPCSTR lpcszFileName,LPCSTR lpcszOutputDir,CArchiveInfoList* pArchiveInfoList);
	static LPCSTR			GetUnGzwErrorCodeDescription	(int nErrorCode);
	
	static LONG			GzwCallback				(WPARAM wParam,LPARAM lParam1,LPARAM lParam2);

	// .zip (solo decompressione)
	static CArchiveInfoList* m_pZipArchiveInfoList;
	static int			UnZip					(LPCSTR lpcszZipFile,LPCSTR lpcszFileName,LPCSTR lpcszOutputDir,CArchiveInfoList* pArchiveInfoList);
	static LPCSTR			GetUnZipErrorCodeDescription	(int nErrorCode);
	
	static void WINAPI		UnZipReceiveDllMessage		(unsigned long,unsigned long,unsigned,unsigned,unsigned,unsigned,unsigned,unsigned,char,LPSTR,LPSTR,unsigned long,char);
	static int WINAPI		UnzipDummyPrintOut			(LPSTR,unsigned long);
	static int WINAPI		UnZipReplaceValue			(char *);
	static int WINAPI		UnZipDummyPassword			(char *,int,const char *,const char *);

	// .rar (solo decompressione)
	static CArchiveInfoList* m_pRarArchiveInfoList;
	static int			UnRar					(LPCSTR lpcszRarFile,LPCSTR lpcszFileName,LPCSTR lpcszOutputDir,CArchiveInfoList* pArchiveInfoList);
	static LPCSTR			GetUnRarErrorCodeDescription	(int nErrorCode);

	CItemList				m_listArchiveFiles;			// lista dei files compressi in cui risiedono i files
	CArchiveInfoList		m_listArchiveInfo;			// lista per il contenuto del file compresso
	CArchiveHandlerList		m_listArchiveHandlers;		// lista per le mappe (formato/gestore)
};

#endif
