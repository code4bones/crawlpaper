/*
	fileio.h
	Luca Piergentili, 06/07/98

	Gestione files.
*/
#ifndef _FILEIO_H
#define _FILEIO_H 1

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

/* headers */
#include <sys/types.h>
#include <sys/stat.h>
#define STRICT 1
#include <windows.h>
#include "wtime.h"

/* macro per attributi */
#define FILE_ATTRIB_NORMAL				0x00
#define FILE_ATTRIB_RDONLY				0x01
#define FILE_ATTRIB_HIDDEN				0x02
#define FILE_ATTRIB_SYSTEM				0x04
#define FILE_ATTRIB_LABEL				0x08
#define FILE_ATTRIB_DIR					0x10
#define FILE_ATTRIB_ARCHIVE				0x20

/* macro per modalita' apertura */
#define FILE_EXCLUSIVE					0L
#define FILE_SHARE						FILE_SHARE_READ|FILE_SHARE_WRITE
#define FILE_SHAREREAD					FILE_SHARE_READ
#define FILE_SHAREWRITE					FILE_SHARE_WRITE

#define FILE_BINMODE					0
#define FILE_TEXTMODE					1

#define FILE_OPENEXIST					OPEN_EXISTING
#define FILE_CREATEIFNOTEXIST				OPEN_ALWAYS
#define FILE_CREATEALWAYS				CREATE_ALWAYS

#define FILE_READ						GENERIC_READ
#define FILE_WRITE						GENERIC_WRITE
#define FILE_READWRITE					GENERIC_READ|GENERIC_WRITE

/* dimensione del buffer per i dati mappati, utilizzata per FILEMAP_STATIC_BUFFER definita */
#define FILEMAP_BUFSIZE					8192

/*
	FILEMAP_STATIC_BUFFER
	
	se la macro viene definita, il campo lpBuffer della struttura FILEMAP viene definito come
	un buffer di FILEMAP_BUFSIZE caratteri ed il campo relativo alla dimensione del buffer 
	(uBufSize) viene impostato automaticamente su FILEMAP_BUFSIZE
	
	se la macro non viene definita, il campo lpBuffer della struttura FILEMAP viene definito
	come un puntatore LPSTR, in tal caso il buffer deve essere allocato dal chiamante 
	(bAllocBuffer=FALSE), o puo' essere allocato dall'API (per bAllocBuffer=TRUE), impostare
	comunque la dimensione del buffer (uBufSize)
*/
#if 0
 #define FILEMAP_STATIC_BUFFER 1
#endif

/*
	FINDFILE
	struttura per ricerca file
*/
typedef struct findfile_t {
	char		name[MAX_PATH];			/* nome (completo di pathname) */
	WORD		attrib;					/* attributo */
	WORD		hour;					/* ora */
	WORD		min;						/* minuti */
	WORD		sec;						/* secondi */
	WORD		year;					/* anno */
	WORD		month;					/* mese */
	WORD		day;						/* giorno */
	DWORD	size;					/* dimensione */
	HANDLE	findhandle;				/* handle per findfirst/next */
} FINDFILE,*LPFINDFILE;

/*
	FILEMAP
	struttura per mappatura file (solo in lettura)
*/
typedef struct filemap_t {
	char		szFileName[MAX_PATH];		/* nome del file da mappare */
	HANDLE	hHandle;					/* handle del file da mappare */
	HANDLE	hMapHandle;				/* handle della mappatura */
	LPVOID	lpMapAddr;				/* puntatore ai dati mappati */
	LPVOID	lpMapView;				/* puntatore alla vista */
	UINT		uBufSize;					/* dimensione del buffer per i dati mappati */
#ifdef FILEMAP_STATIC_BUFFER				/* buffer/puntatore per i dati mappati */
	char		lpBuffer[FILEMAP_BUFSIZE];
#else
	LPSTR	lpBuffer;
	BOOL		bAllocBuffer;				/* flag per allocazione automatica */
#endif
	DWORD	dwFileSize;				/* dimensione del file */
	DWORD	dwRemain;					/* bytes da leggere */
	DWORD	dwRead;					/* bytes letti */
} FILEMAP,*LPFILEMAP;

/*
	FILESTAT
	struttura per status
*/
typedef struct filestat_t {
	struct _stat filestat;
} FILESTAT,*LPFILESTAT;

/* open, create, read, write, close */
#ifdef _DEBUG
 HANDLE	File_OpenFile					(LPCSTR,DWORD,DWORD);
 #define	File_OpenFileReadShare(f)		File_OpenFile(f,GENERIC_READ,FILE_SHARE_READ)
 #define	File_OpenFileReadExcl(f)			File_OpenFile(f,GENERIC_READ,FILE_EXCLUSIVE)
 #define	File_OpenFileWriteShare(f)		File_OpenFile(f,GENERIC_WRITE,FILE_SHARE_WRITE)
 #define	File_OpenFileWriteExcl(f)		File_OpenFile(f,GENERIC_WRITE,FILE_EXCLUSIVE)
 #define	File_OpenFileReadWriteShare(f)	File_OpenFile(f,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE)
 #define	File_OpenFileReadWriteExcl(f)		File_OpenFile(f,GENERIC_READ|GENERIC_WRITE,FILE_EXCLUSIVE)
 HANDLE	File_CreateFile				(LPCSTR);
 HANDLE	File_CreateFileIfNotExist		(LPCSTR);
 BOOL	File_ReadFile					(HANDLE,LPVOID,DWORD,LPDWORD);
 DWORD	File_ReadTextFile				(HANDLE,LPSTR,DWORD);
 BOOL	File_WriteFile					(HANDLE,LPVOID,DWORD,LPDWORD);
 DWORD	File_SetFilePointer				(HANDLE,LONG,DWORD);
 BOOL	File_CloseFile					(HANDLE);
#else /* NO _DEBUG */
 #define	File_OpenFile(f,m,s)			CreateFile(f,m,s,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL)
 #define	File_OpenFileReadShare(f)		File_OpenFile(f,GENERIC_READ,FILE_SHARE_READ)
 #define	File_OpenFileReadExcl(f)			File_OpenFile(f,GENERIC_READ,FILE_EXCLUSIVE)
 #define	File_OpenFileWriteShare(f)		File_OpenFile(f,GENERIC_WRITE,FILE_SHARE_WRITE)
 #define	File_OpenFileWriteExcl(f)		File_OpenFile(f,GENERIC_WRITE,FILE_EXCLUSIVE)
 #define	File_OpenFileReadWriteShare(f)	File_OpenFile(f,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE)
 #define	File_OpenFileReadWriteExcl(f)		File_OpenFile(f,GENERIC_READ|GENERIC_WRITE,FILE_EXCLUSIVE)
 #define	File_CreateFile(file)			CreateFile(file,GENERIC_READ|GENERIC_WRITE,FILE_EXCLUSIVE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL)
 HANDLE	File_CreateFileIfNotExist		(LPCSTR);
 #define	File_ReadFile(h,b,tr,r)			ReadFile(h,b,tr,r,NULL)
 DWORD	File_ReadTextFile				(HANDLE,LPSTR,DWORD);
 #define	File_WriteFile(h,b,tw,w)			WriteFile(h,b,tw,w,NULL)
 #define	File_SetFilePointer(h,o,p)		SetFilePointer(h,o,NULL,p)
 #define	File_CloseFile(h)				CloseHandle(h)
#endif

/* mapped: open, read, seek, close */
HANDLE	File_OpenMappedFile				(LPCSTR,LPFILEMAP);
DWORD	File_ReadMappedFile				(LPFILEMAP);
void		File_SetMappedFilePointer		(LONG,LPFILEMAP);
#define	File_MappedFileEof(f)			(f.dwRemain <= 0L)
BOOL		File_CloseMappedFile			(LPFILEMAP);

/* find */
BOOL		File_FindFirst					(LPCSTR,LPFINDFILE);
BOOL		File_FindNext					(LPFINDFILE);
#define	File_FindIsFile(F)				(F.attrib & FILE_ATTRIB_NORMAL)
#define	File_FindIsReadOnly(F)			(F.attrib & FILE_ATTRIB_RDONLY)
#define	File_FindIsHidden(F)			(F.attrib & FILE_ATTRIB_HIDDEN)
#define	File_FindIsSystemFile(F)			(F.attrib & FILE_ATTRIB_SYSTEM)
#define	File_FindIsLabel(F)				(F.attrib & FILE_ATTRIB_LABEL)
#define	File_FindIsDir(F)				(F.attrib & FILE_ATTRIB_DIR)
#define	File_FindIsCurDir(F)			((F.attrib & FILE_ATTRIB_DIR) && (F.name[0]=='.'))
#define	File_FindIsParentDir(F)			((F.attrib & FILE_ATTRIB_DIR) && (F.name[0]=='.') && (F.name[1]=='.'))
#define	File_FindIsArch(F)				(F.attrib & FILE_ATTRIB_ARCHIVE)
LPSTR	File_FindStrTime				(LPFINDFILE);
LPSTR	File_FindStrDate				(LPFINDFILE);

/* stat */
BOOL		File_Exist					(LPCSTR);
BOOL		File_Stat						(LPCSTR,LPFILESTAT);
#define	File_StatIsDir(F)				(F.filestat.st_mode & _S_IFDIR)
#define	File_StatIsFile(F)				(F.filestat.st_mode & _S_IFREG)

/* date, time */
DWORD	File_Size						(LPCSTR);
BOOL		File_GetGmtDate				(LPCSTR,LPSTR);
BOOL		File_SetMsDosTime				(LPCSTR,WORD,WORD);
BOOL		File_GetMsDosTime				(LPCSTR,LPWORD,LPWORD);

/* pathnames */
UINT		File_StripFile					(LPSTR);
UINT		File_StripPath					(LPSTR);
BOOL		File_UnPath					(LPSTR);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _FILEIO_H */
