/*
	CGzw.h
	Classe base per l'interfaccia con la zLib.
	*** da riscrivere ***

	Luca Piergentili, 31/08/96
	lpiergentili@yahoo.com
*/
#ifndef _CGZW_H
#define _CGZW_H 1

#include "window.h"
#include "win32api.h"
#include "CFindFile.h"
#include "CNodeList.h"
#include "CWildCards.h"
#include "CzLib.h"

/*
	FPGZWCALLBACK
	tipo per la callback
*/
typedef LONG (*FPGZWCALLBACK)(WPARAM,LPARAM,LPARAM);

/*
	GZW_OPERATION
	enum per le operazioni supportate
*/
enum GZW_OPERATION {
	GZW_COMPRESS,
	GZW_MOVE,
	GZW_UNCOMPRESS,
	GZW_EXTRACT,
	GZW_LIST,
	GZW_UNKNOW
};

/*
	GZW_FLAGS
	struttura bit-fields per le operazioni/opzioni supportate
*/
struct GZW_FLAGS {
	unsigned int compress:1;
	unsigned int move:1;
	unsigned int uncompress:1;
	unsigned int extract:1;
	unsigned int list:1;
	unsigned int ratio;
	unsigned int pathname:1;
	unsigned int path:1;
	unsigned int recurse:1;
};

/*
	GZW
	struttura per l'interfaccia con l'API
*/
struct GZW {
	HWND			hWnd;					// finestra del chiamante
	FPGZWCALLBACK	fpCallBack;				// callback, impostare a NULL se non si utilizza
	char			szInputFile[_MAX_PATH+1];	// file di input
	int			nInputFileLen;				// lunghezza nome file di input
	char			szOutputFile[_MAX_PATH+1];	// file di output
	int			nOutputFileLen;			// lunghezza nome file di output
	char			szWildcardsIncl[_MAX_PATH+1];	// skeleton per le wildcards
	int			nWildcardsInclLen;			// lunghezza dello skeleton
	char			szWildcardsExcl[_MAX_PATH+1];	// skeleton per le wildcards
	int			nWildcardsExclLen;			// lunghezza dello skeleton
	char			szPsw[GZW_PSW_MAX+1];		// password
	int			nPswLen;					// lunghezza password
	GZW_FLAGS		wFlag;					// flags
};

/* codici di ritorno */
enum {
	GZWE_UNKNOW_OPTION,
	GZWE_WRONG_PARAMETERS,
	GZWE_COMPRESS_FAILURE,
	GZWE_UNCOMPRESS_FAILURE,
	GZWE_LIST_FAILURE,
	GZWE_VIEW_FAILURE,
	GZWE_INVALID_FILENAME,
	GZWE_NOSUCHFILE,
	GZWE_NOSUCHWILD,
	GZWE_INVALID_FORMAT,
	GZWE_WRONG_PASSWORD,
	GZWE_MALLOC_FAILURE,
	GZWE_SEARCH_FAILURE,
	GZWE_SCRIPT_FAILURE,
	GZWE_CHECK_FAILURE,
	GZWE_OPEN_FAILURE,
	GZWE_CREATE_FAILURE,
	GZWE_CLOSE_FAILURE,
	GZWE_SEEK_FAILURE,
	GZWE_READ_FAILURE,
	GZWE_WRITE_FAILURE,
	GZWE_MKDIR_FAILURE,
	GZWE_UPDATE_FAILURE,
	GZWE_UNKNOW_ERROR,
	GZW_FAILURE,
	GZW_SUCCESS,
};

/* id callback */
enum {
	GZW_CALLBACK_COMPRESS_BEGIN,
	GZW_CALLBACK_COMPRESS_END,
	
	GZW_CALLBACK_UNCOMPRESS_BEGIN,
	GZW_CALLBACK_UNCOMPRESS_END,
	
	GZW_CALLBACK_LIST_BEGIN,
	GZW_CALLBACK_LIST,
	GZW_CALLBACK_LIST_END,
	
	GZW_CALLBACK_BEGIN,
	GZW_CALLBACK_END
};

/*
	CGzw
*/
class CGzw : public CzLib
{
public:
	CGzw(HWND hWnd = NULL);
	virtual ~CGzw() {}

	BOOL			SetOperation		(GZW_OPERATION);
	GZW_OPERATION	GetOperation		(void);
	
	void			SetRatio			(int n);
	int			GetRatio			(void);

	void			SetAbsolutePathname	(BOOL bFlag);
	void			SetRelativePathname	(BOOL bFlag);
	void			SetRecursive		(BOOL bFlag);

	void			SetPassword		(LPCSTR lpcszPsw);
	void			SetWildcardsInclude	(LPCSTR lpcszWildcards = NULL,BOOL bIgnoreCase = FALSE,BOOL bIgnoreSpaces = FALSE);
	void			SetWildcardsExclude	(LPCSTR lpcszWildcards = NULL,BOOL bIgnoreCase = FALSE,BOOL bIgnoreSpaces = FALSE);

	void			SetInputFile		(LPCSTR lpcszFile);
	void			SetOutputFile		(LPCSTR lpcszFile);

#ifdef _WINDOWS
	void			SetCallback		(FPGZWCALLBACK lpFn);
#endif

	/*
		compressione:
		-p SetAbsolutePathname(TRUE), registra il pathname assoluto (completo)
		-P SetRelativePathname(TRUE), registra solo il pathname relativo
		none (default), SetAbsolutePathname(FALSE) e SetRelativePathname(FALSE), registra solo il nome file

		decompressione:
		-p SetAbsolutePathname(TRUE), ricrea il pathname assoluto (completo), ignorando la directory di output
		-P SetRelativePathname(TRUE), ricrea il pathname relativo a partire dalla directory corrente o da quella specificata
		none (default), SetAbsolutePathname(FALSE) e SetRelativePathname(FALSE), ricrea il file nella directory corrente o in quella specificata
	*/
	UINT		Gzw				(void);

private:
	UINT		Compress			(void);
	UINT		CompressFile		(LPCSTR,LPCSTR,LPCSTR,GZWHDR*);
	UINT		Uncompress		(void);
	UINT		UncompressFile		(void);
	UINT		List				(void);
	UINT		ListFile			(void);
	UINT		CheckHeader		(LPCSTR);
	LPCSTR	ExtractFile		(FILE*,GZWHDR*,DWORD,UINT*);
	LPCSTR	ExtractHeader		(FILE*,GZWHDR*,DWORD,UINT*);

	LPCSTR	ParseScript		(LPCSTR lpszFileSpec,UINT* wGzwError);
	UINT		StripPath			(LPSTR,UINT);
	UINT		AddAbsolutePath	(LPSTR,UINT);
	UINT		AddRelativePath	(LPSTR,UINT);
	UINT		Diff				(LPSTR,LPSTR);
	LPSTR	TmpName			(void);

	static LONG __stdcall FindFileCallBack(WPARAM wparam,LPARAM lparam);

	GZW		m_Gzw;
	CFindFile	m_FindFile;
	CWildCards m_WildCardsInclude;
	CWildCards m_WildCardsExclude;
	PFNFINDFILECALLBACK m_lpfnFindFileCallBack;
};

#endif // _CGZW_H
