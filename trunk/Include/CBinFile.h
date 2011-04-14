/*
	CBinFile.h
	Classe base per l'accesso a files binari (SDK/MFC).
	Luca Piergentili, 31/08/98
	lpiergentili@yahoo.com
*/
#ifndef _CBINFILE_H
#define _CBINFILE_H 1

#include "window.h"
#include "CDateTime.h"
#include "CSync.h"

// access type
#define GENERIC_QUERY	0 // Specifies device query access to the object. An application can query device attributes without accessing the device.
//GENERIC_READ			Specifies read access to the object. Data can be read from the file and the file pointer can be moved. Combine with GENERIC_WRITE for read-write access.
//GENERIC_WRITE		Specifies write access to the object. Data can be written to the file and the file pointer can be moved. Combine with GENERIC_READ for read-write access.

// share mode
//FILE_SHARE_DELETE		Windows NT only: Subsequent open operations on the object will succeed only if delete access is requested. 
//FILE_SHARE_READ		Subsequent open operations on the object will succeed only if read access is requested. 
//FILE_SHARE_WRITE		Subsequent open operations on the object will succeed only if write access is requested.
// W95
#define FILE_SHARE		(FILE_SHARE_READ|FILE_SHARE_WRITE)
// WNT
//#define FILE_SHARE	(FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE)
#define FILE_EXCLUSIVE	0
 
// create mode
//CREATE_NEW			Creates a new file. The function fails if the specified file already exists.
//CREATE_ALWAYS		Creates a new file. The function overwrites the file if it exists.
//OPEN_EXISTING		Opens the file. The function fails if the file does not exist. See the Remarks section for a discussion of why you should use the OPEN_EXISTING flag if you are using the CreateFile function for devices, including the console.
//OPEN_ALWAYS			Opens the file, if it exists. If the file does not exist, the function creates the file as if dwCreationDistribution were CREATE_NEW.
//TRUNCATE_EXISTING		Opens the file. Once opened, the file is truncated so that its size is zero bytes. The calling process must open the file with at least GENERIC_WRITE access. The function fails if the file does not exist.

// attribute
//FILE_ATTRIBUTE_ARCHIVE		The file should be archived. Applications use this attribute to mark files for backup or removal.
//FILE_ATTRIBUTE_COMPRESSED	The file or directory is compressed. For a file, this means that all of the data in the file is compressed. For a directory, this means that compression is the default for newly created files and subdirectories.
//FILE_ATTRIBUTE_HIDDEN		The file is hidden. It is not to be included in an ordinary directory listing.
//FILE_ATTRIBUTE_NORMAL		The file has no other attributes set. This attribute is valid only if used alone.
//FILE_ATTRIBUTE_OFFLINE		The data of the file is not immediately available. Indicates that the file data has been physically moved to offline storage.
//FILE_ATTRIBUTE_READONLY	The file is read only. Applications can read the file but cannot write to it or delete it.
//FILE_ATTRIBUTE_SYSTEM		The file is part of or is used exclusively by the operating system.
//FILE_ATTRIBUTE_TEMPORARY	The file is being used for temporary storage. File systems attempt to keep all of the data in memory for quicker access rather than flushing the data back to mass storage. A temporary file should be deleted by the application as soon as it is no longer needed.

// ofs:pos (lseek)
#define FILE_EOF	((DWORD)-1L)
#define FILE_EEOF	((QWORD)-1L)
//FILE_BEGIN		The starting point is zero or the beginning of the file. If FILE_BEGIN is specified, DistanceToMove is interpreted as an unsigned location for the new file pointer.
//FILE_CURRENT		The current value of the file pointer is the starting point.
//FILE_END		The current end-of-file position is the starting point.

/*
	CBinFile
*/
class CBinFile
{
public:
	CBinFile();
	virtual ~CBinFile();

	inline BOOL	IsOpen				(void) {return((m_hHandle!=INVALID_HANDLE_VALUE));}
	virtual BOOL	Open					(LPCSTR lpcszFileName,BOOL bCreateIfNotExist = TRUE,DWORD dwAccessMode = GENERIC_READ|GENERIC_WRITE,DWORD dwShareMode = FILE_SHARE);
	inline BOOL	OpenExistingReadOnly	(LPCSTR lpcszFileName) {return(Open(lpcszFileName,FALSE,GENERIC_READ));}
	inline BOOL	OpenExistingReadWrite	(LPCSTR lpcszFileName) {return(Open(lpcszFileName,FALSE,GENERIC_READ|GENERIC_WRITE));}
	
	BOOL			Create				(LPCSTR lpcszFileName,DWORD dwAccessMode = GENERIC_READ|GENERIC_WRITE,DWORD dwShareMode = FILE_SHARE,DWORD dwAttribute = FILE_ATTRIBUTE_NORMAL);
	virtual BOOL	Close				(void);

	DWORD		Read					(LPVOID lpBuffer,DWORD dwToRead);
	virtual DWORD	Write				(LPCVOID lpcBuffer,DWORD dwToWrite);
	
	virtual DWORD	Seek					(LONG lOffset,DWORD dwOrigin);
	inline DWORD	GetFileOffset			(void) {return(CBinFile::Seek(0L,0L));}

	virtual DWORD	GetFileSize			(void);
	BOOL			GetFileTime			(CDateTime& datetime,BOOL bConvertToLocalTime = FALSE);

	inline DWORD	GetLastErrorCode		(void) {return(m_dwError);}
	inline LPCSTR	GetLastErrorString		(void) {return(m_szError);}
	inline void	ShowErrors			(BOOL bFlag) {m_bShowErrors = bFlag;}

protected:
	void			SetLastErrorCode		(DWORD dwError);

	BOOL			m_bShowErrors;
	HANDLE		m_hHandle;
	DWORD		m_dwError;
	char			m_szFileName[_MAX_PATH+1];
	char			m_szError[256];
};

/*
	CBinFileEx
*/
class CBinFileEx : public CBinFile
{
public:
	virtual QWORD		GetFileSizeEx	(void);
	virtual LONGLONG	SeekEx		(LONGLONG llOffset,DWORD dwOrigin);
	virtual QWORD		WriteEx		(LPCVOID lpcBuffer,QWORD qwToWrite);
};

/*
	CBinFileLock
*/
class CBinFileLock : public CBinFile, public CSyncThreads
{
public:
	virtual BOOL Open(	LPCSTR	lpcszFileName,
					BOOL		bCreateIfNotExist = FALSE,
					DWORD	dwAccessMode = GENERIC_READ|GENERIC_WRITE,
					DWORD	dwShareMode = FILE_SHARE
					);
};

#endif // _CBINFILE
