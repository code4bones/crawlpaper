/*
	CGzw.cpp
	Classe base per l'interfaccia con la zLib.
	*** da riscrivere ***

	Luca Piergentili, 31/08/96
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <ctype.h>
#include <direct.h>
#include <errno.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include <time.h>
#include "window.h"
#include "win32api.h"
#include "CFindFile.h"
#include "CNodeList.h"
#include "CzLib.h"
#include "CWildCards.h"
#include "CGzw.h"

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

LONG __stdcall CGzw::FindFileCallBack(WPARAM /*wparam*/,LPARAM lparam)
{
	FINDFILE* f = (FINDFILE*)lparam;
	char dir[_MAX_PATH+1];
	if(f)
	{
		strcpyn(dir,f->name,sizeof(dir));
		char* p = strrchr(dir,'\\');
		if(p)
			*p = '\0';
	}

	return(0);
}

CGzw::CGzw(HWND hWnd/* = NULL*/)
{
	memset(&m_Gzw,'\0',sizeof(GZW));

	if(hWnd)
		m_Gzw.hWnd = hWnd;

	m_lpfnFindFileCallBack = &FindFileCallBack;
	m_FindFile.SetCallback(m_lpfnFindFileCallBack);
}

BOOL CGzw::SetOperation(GZW_OPERATION op)
{
	memset(&(m_Gzw.wFlag),'\0',sizeof(GZW_FLAGS));

	switch(op)
	{
		case GZW_COMPRESS:
			m_Gzw.wFlag.compress = 1;
			break;
		case GZW_MOVE:
			m_Gzw.wFlag.move = 1;
			break;
		case GZW_UNCOMPRESS:
			m_Gzw.wFlag.uncompress = 1;
			break;
		case GZW_EXTRACT:
			m_Gzw.wFlag.extract = 1;
			break;
		case GZW_LIST:
			m_Gzw.wFlag.list = 1;
			break;
		default:
			return(FALSE);
	}

	return(TRUE);
}

GZW_OPERATION CGzw::GetOperation(void)
{
	GZW_OPERATION op = GZW_UNKNOW;

	if(m_Gzw.wFlag.compress==1)
		op = GZW_COMPRESS;
	else if(m_Gzw.wFlag.move==1)
		op = GZW_MOVE;
	else if(m_Gzw.wFlag.uncompress==1)
		op = GZW_UNCOMPRESS;
	else if(m_Gzw.wFlag.extract==1)
		op = GZW_EXTRACT;
	else if(m_Gzw.wFlag.list==1)
		op = GZW_LIST;

	return(op);
}

void CGzw::SetRatio(int n)
{
	m_Gzw.wFlag.ratio = (n >= 0 && n < 9) ? n : 9;
}

int CGzw::GetRatio(void)
{
	return(m_Gzw.wFlag.ratio);
}

void CGzw::SetAbsolutePathname(BOOL bFlag)
{
	m_Gzw.wFlag.pathname = bFlag ? 1 : 0;
}

void CGzw::SetRelativePathname(BOOL bFlag)
{
	m_Gzw.wFlag.path = bFlag ? 1 : 0;
}

void CGzw::SetRecursive(BOOL bFlag)
{
	m_Gzw.wFlag.recurse = bFlag ? 1 : 0;
}

void CGzw::SetWildcardsInclude(LPCSTR lpcszWildcards/* = NULL*/,BOOL bIgnoreCase/* = FALSE*/,BOOL bIgnoreSpaces/* = FALSE*/)
{
	if(lpcszWildcards)
	{
		strcpyn(m_Gzw.szWildcardsIncl,lpcszWildcards,sizeof(m_Gzw.szWildcardsIncl));
		m_Gzw.nWildcardsInclLen = strlen(lpcszWildcards);
	}
	else
	{
		memset(m_Gzw.szWildcardsIncl,'\0',sizeof(m_Gzw.szWildcardsIncl));
		m_Gzw.nWildcardsInclLen = 0;
	}
	
	m_WildCardsInclude.SetIgnoreCase(bIgnoreCase);
	m_WildCardsInclude.SetIgnoreSpaces(bIgnoreSpaces);
}

void CGzw::SetWildcardsExclude(LPCSTR lpcszWildcards/* = NULL*/,BOOL bIgnoreCase/* = FALSE*/,BOOL bIgnoreSpaces/* = FALSE*/)
{
	if(lpcszWildcards)
	{
		strcpyn(m_Gzw.szWildcardsExcl,lpcszWildcards,sizeof(m_Gzw.szWildcardsExcl));
		m_Gzw.nWildcardsExclLen = strlen(lpcszWildcards);
	}
	else
	{
		memset(m_Gzw.szWildcardsExcl,'\0',sizeof(m_Gzw.szWildcardsExcl));
		m_Gzw.nWildcardsExclLen = 0;
	}
	
	m_WildCardsExclude.SetIgnoreCase(bIgnoreCase);
	m_WildCardsExclude.SetIgnoreSpaces(bIgnoreSpaces);
}

void CGzw::SetInputFile(LPCSTR lpcszFile)
{
	strcpyn(m_Gzw.szInputFile,lpcszFile,sizeof(m_Gzw.szInputFile));
	m_Gzw.nInputFileLen = strlen(lpcszFile);
}

void CGzw::SetOutputFile(LPCSTR lpcszFile)
{
	strcpyn(m_Gzw.szOutputFile,lpcszFile,sizeof(m_Gzw.szOutputFile));
	m_Gzw.nOutputFileLen = strlen(lpcszFile);
}

void CGzw::SetPassword(LPCSTR lpcszPsw)
{
	strcpyn(m_Gzw.szPsw,lpcszPsw,sizeof(m_Gzw.szPsw));
	m_Gzw.nPswLen = strlen(lpcszPsw);
}

void CGzw::SetCallback(FPGZWCALLBACK lpFn)
{
	m_Gzw.fpCallBack = lpFn;
}

UINT CGzw::Gzw(void)
{
	int nRet = GZW_FAILURE;

	if(m_Gzw.wFlag.compress==1)
		nRet = Compress();
	else if(m_Gzw.wFlag.move==1)
		nRet = Compress();
	else if(m_Gzw.wFlag.uncompress==1)
		nRet = Uncompress();
	else if(m_Gzw.wFlag.extract==1)
		nRet = Uncompress();
	else if(m_Gzw.wFlag.list==1)
		nRet = List();

	return(nRet);
}

/*
	Compress()

	Comprime lo skeleton di input nel .gzw.
	Per ognuno dei files di input espansi dallo skeleton, chiama CompressFile().
	Nel ciclo di elaborazione dello skeleton di input, se il file non puo' essere aperto salta a quello
	successivo, senza generare errore.	Non include nel .gzw i file a dimensione 0.
	In input puo' essere specificato un nome file, uno skeleton o un file script (preceduto dal carattere
	'@'). Il file script e' un normale file ASCII contenente i nomi dei file di input, uno per linea.
	I nomi dei file presenti nello script possono contenere wildcards.
	Con l'opzione -p viene memorizzato il pathname assoluto del file, mentre con l'opzione -P memorizza il
	pathname relativo. L'opzione -p genera un pathname soltanto se lo skeleton  di input non ne contiene gia'
	uno, mentre l'opzione -P per generare un pathname richiede la presenza dell'opzione -r.
	Con l'opzione -r ricerca (ricorsivamente) i file nelle subdirectory, generando (nella directory puntata
	dalla variabile d'ambiente TMP/TEMP) un file script contenente tutti i nomi dei file trovati. Tale file
	script viene poi sostituito alla specifica di input passata nella struttura.
	L'opzione  -w  permette di specificare, in compressione, uno skeleton da escludere, distinguendo tra
	maiuscole e minuscole, per cui *.c e' diverso da *.C.
*/
UINT CGzw::Compress(void)
{
	GZWHDR		stHeader;
	char			szOpenFlag[5];
	LPSTR		lpszInFile = NULL;
	char			szInputFileName[_MAX_PATH+1];
	char			szInputFile[_MAX_PATH+1];
	char			szOutputFile[_MAX_PATH+1];
	char			szPathName[_MAX_PATH+1];
	char			szSkeleton[_MAX_PATH+1] = {0};
	char			szPsw[GZW_PSW_MAX+1];
	BOOL			bCreate = TRUE;
	HFILE		hHandle;
	DWORD		dwTotFileNameLen = 0;
	int			nTotFiles = 0;
	DWORD		dwCurrentData = 0L;
	DWORD		dwLastData = 0L;
	DWORD		dwTotData = 0L;
	DWORD		dwTotSize = 0L;
	DWORD		dwFileSize =0L;
	long			lOffset = 0L;
	WORD			wFileDate;
	WORD			wFileTime;
	UINT			nRet = GZW_SUCCESS;

/*
	if(strchr(m_Gzw.szInputFile,'\\'))
	{
		char chr = m_Gzw.szInputFile[strlen(m_Gzw.szInputFile)-1];

		if(chr!='@' && chr!='\\' && chr!='/' && chr!='*' && chr!='?')
		{
			DWORD dwAttribute = ::GetFileAttributes(m_Gzw.szInputFile);
			if(dwAttribute & FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(m_Gzw.szInputFile,"\\*.*");
				m_Gzw.nInputFileLen += 4;
			}
		}
	}
*/
/*
	DWORD dwAttribute = ::GetFileAttributes(m_Gzw.szInputFile);
	if(dwAttribute!=(DWORD)-1)
	{
		if(dwAttribute & FILE_ATTRIBUTE_DIRECTORY)
		{
			strcat(m_Gzw.szInputFile,"\\*.*");
			m_Gzw.nInputFileLen += 4;
		}
	}
*/
	// distingue tra file script e skeleton
	lpszInFile = m_Gzw.szInputFile[0]=='@' ? (LPSTR)ParseScript(m_Gzw.szInputFile+1,&nRet) : m_Gzw.szInputFile;

	while(lpszInFile)
	{
		// espande lo skeleton e comprime i files di input nel .gzw di output
		m_FindFile.SplitPathName(lpszInFile,szPathName,sizeof(szPathName),szSkeleton,sizeof(szSkeleton));

		while((lpszInFile = (LPSTR)m_FindFile.FindEx(szPathName,szSkeleton,m_Gzw.wFlag.recurse))!=NULL)
		{
			// pseudo-multitasking
			::Sleep(1);

			// se e' stata specificata l'opzione -w, controlla che il nome del file di input soddisfi lo skeleton
			if(m_Gzw.szWildcardsIncl[0]!='\0')
				if(!m_WildCardsInclude.Match(m_Gzw.szWildcardsIncl,lpszInFile))
					continue;
			if(m_Gzw.szWildcardsExcl[0]!='\0')
				if(m_WildCardsExclude.Match(m_Gzw.szWildcardsExcl,lpszInFile))
					continue;

			// inizializza la struttura
			memset(&stHeader,'\0',sizeof(GZWHDR));
		
			// imposta il campo dell'header con la versione corrente
			memcpy(stHeader.stGzwHdr.szSignature,GZW_SIGNATURE,GZW_SIGN_LEN);
		
			// inizializza i buffer per i nomi dei file
			memset(szInputFile,'\0',sizeof(szInputFile));
			if(bCreate)
				memset(szOutputFile,'\0',sizeof(szOutputFile));

			// controlla se deve usare la password
			if(m_Gzw.nPswLen > 0)
			{
				// copia la password nell'header
				m_Gzw.nPswLen = (m_Gzw.nPswLen >= GZW_PSW_MAX ? GZW_PSW_MAX : m_Gzw.nPswLen);
				strcpyn(stHeader.stGzwHdr.szPsw,m_Gzw.szPsw,sizeof(stHeader.stGzwHdr.szPsw));
				stHeader.stGzwHdr.nPswLen = m_Gzw.nPswLen;
				
				// copia la password nel buffer
				memset(szPsw,'\0',sizeof(szPsw));
				strcpyn(szPsw,stHeader.stGzwHdr.szPsw,sizeof(szPsw));
				
				// la mette in xor con se stessa
				memnxor(szPsw,stHeader.stGzwHdr.szPsw,stHeader.stGzwHdr.nPswLen);
			
				// la inserisce nell'header
				memcpy(stHeader.stGzwHdr.szPsw,szPsw,stHeader.stGzwHdr.nPswLen);
			}
		
			// copia il nome del file di input nel buffer
			strcpyn(szInputFile,lpszInFile,sizeof(szInputFile));
			stHeader.stGzwHdr.nFileNameLen = strlen(szInputFile);
			
			// imposta il puntatore al nome file di input della struttura sul buffer interno
			memset(szInputFileName,'\0',sizeof(szInputFileName));
			stHeader.pFileName = szInputFileName;
			
			// copia il nome del file di input nel campo dell'header
			memcpy(stHeader.pFileName,szInputFile,stHeader.stGzwHdr.nFileNameLen);

			if(m_Gzw.wFlag.pathname) // -p inserisce il pathname assoluto
			{
				if((stHeader.stGzwHdr.nFileNameLen = AddAbsolutePath(stHeader.pFileName,stHeader.stGzwHdr.nFileNameLen)) < 0)
					continue;
			}
			else if(m_Gzw.wFlag.path) // -P inserisce il pathname relativo
			{
				if((stHeader.stGzwHdr.nFileNameLen = AddRelativePath(stHeader.pFileName,stHeader.stGzwHdr.nFileNameLen)) < 0)
					continue;
			}
			else // inserisce solo il nome del file
			{
				if((stHeader.stGzwHdr.nFileNameLen = StripPath(stHeader.pFileName,stHeader.stGzwHdr.nFileNameLen)) < 0)
					continue;
			}

			// salva il totale dei bytes utilizzati per il nomefile di input
			dwTotFileNameLen += (DWORD)stHeader.stGzwHdr.nFileNameLen;
			
			// non deve includere il .gzw di output
			if(Diff(szInputFile,szOutputFile)==0)
			{
				dwTotFileNameLen -= (DWORD)stHeader.stGzwHdr.nFileNameLen;
				continue;
			}

			// ricava data e ora del file di input
			m_FindFile.GetFileTime(szInputFile,&wFileDate,&wFileTime);

			// ricava la dimensione del file di input e la registra nell'header
			stHeader.stGzwHdr.dwFileSize = dwFileSize = (DWORD)::GetFileSizeExt(szInputFile);
			
			// controlla che il file di input non sia a dimensione 0, in tal caso salta al successivo
			if(dwFileSize==0L || dwFileSize==(DWORD)-1)
			{
				dwTotFileNameLen -= (DWORD)stHeader.stGzwHdr.nFileNameLen;
				if(m_Gzw.wFlag.move)
					::DeleteFile(szInputFile);
				continue;
			}
		
			// ricava il nome file di output (.gzw) ed imposta la modalita' di apertura
			if(bCreate)
			{
				bCreate = FALSE;
				
				// copia il nome del file di output (.gzw) nel buffer
				memcpy(szOutputFile,m_Gzw.szOutputFile,m_Gzw.nOutputFileLen);

				// imposta la modalita' su create
				_snprintf(szOpenFlag,sizeof(szOpenFlag)-1,"wb%d",m_Gzw.wFlag.ratio);
			}
			else
			{
				// imposta la modalita' su append
				_snprintf(szOpenFlag,sizeof(szOpenFlag)-1,"ab%d",m_Gzw.wFlag.ratio);
			}

			// chiama la callback
			if(m_Gzw.fpCallBack!=NULL)
				m_Gzw.fpCallBack(GZW_CALLBACK_COMPRESS_BEGIN,(LPARAM)&stHeader,NULL);

			// comprime il file di input nel .gzw
			switch((nRet = CompressFile(szInputFile,szOutputFile,szOpenFlag,&stHeader)))
			{
				// ok
				case GZW_SUCCESS:
					break;

				// errore durante l'apertura del file di input
				case GZWE_OPEN_FAILURE:
					dwTotFileNameLen -= (DWORD)stHeader.stGzwHdr.nFileNameLen;
					continue;

				// errore durante la creazione/apertura del file di output (.gzw)
				case GZWE_CREATE_FAILURE:
				case GZWE_WRITE_FAILURE:
				case GZWE_UPDATE_FAILURE:
				case GZWE_CLOSE_FAILURE:
				case GZWE_COMPRESS_FAILURE:
					::DeleteFile(szOutputFile);
					return(nRet);
			}

			// elimina il file di input per -m
			if(m_Gzw.wFlag.move)
				::DeleteFile(szInputFile);
			
			// riapre il .gzw e registra nell'header la dimensione compressa del file di input
			if((hHandle = _lopen(szOutputFile,OF_READWRITE))!=HFILE_ERROR)
			{
				// ricava la dimensione del .gzw
				dwTotSize = (DWORD)_llseek(hHandle,0L,2);
				
				// salva il precedente totale dei dati compressi scritti nel .gzw
				dwLastData = dwTotData;
				
				// ricava il totale dei dati compressi scritti nel .gzw fino ad ora
				dwTotData = dwTotSize - (((((DWORD)GZ_HEADER_LEN + (DWORD)sizeof(GZWHEADER)) * ((DWORD)nTotFiles + 1L))) + dwTotFileNameLen);
				
				// ricava il totale dei dati compressi relativo al file di input
				dwCurrentData = dwTotData - dwLastData;

				// calcola l'offset all'interno del .gzw per rileggere l'header del file di input corrente
				if(nTotFiles==0)
					lOffset = (long)GZ_HEADER_LEN;
				else
					lOffset = (((long)nTotFiles + 1L) * (long)GZ_HEADER_LEN) + ((long)sizeof(GZWHEADER) * (long)nTotFiles) + ((long)dwTotFileNameLen - (long)stHeader.stGzwHdr.nFileNameLen) + ((long)dwTotData - (long)dwCurrentData);
							
				// posiziona e (ri)legge l'header relativo al file di input
				_llseek(hHandle,lOffset,0);
				_lread(hHandle,(LPVOID)&stHeader,sizeof(GZWHEADER));

				// imposta i campi dell'header con la dimensione compressa, la data e l'ora
				stHeader.stGzwHdr.dwFileCompressedSize = dwCurrentData;
				stHeader.stGzwHdr.wFileDate = wFileDate;
				stHeader.stGzwHdr.wFileTime = wFileTime;
				
				// aggiorna l'header relativo al file di input corrente
				_llseek(hHandle,((long)sizeof(GZWHEADER) * -1L),1);
				_lwrite(hHandle,(LPCSTR)&stHeader,sizeof(GZWHEADER));

				// chiude il file .gzw
				_lclose(hHandle);
			}
			else
			{
				::DeleteFile(szOutputFile);
				return(GZWE_UPDATE_FAILURE);
			}

			// chiama la callback
			if(m_Gzw.fpCallBack!=NULL)
				m_Gzw.fpCallBack(GZW_CALLBACK_COMPRESS_END,(LPARAM)&stHeader,NULL);

			// incrementa il numero di file scritti nel .gzw
			nTotFiles++;
		}

		// ricava il file successivo
		lpszInFile = m_Gzw.szInputFile[0]=='@' ? (LPSTR)ParseScript(m_Gzw.szInputFile+1,&nRet) : NULL;
	}

	return(nRet);
}

/*
	CompressFile()
	
	Comprime il file di input nel .gzw.
	A seconda della modalita' di apertura, crea o aggiunge al file .gzw il contenuto del file di input.
	Restituisce GZW_SUCCESS per compressione riuscita, GZWE_... per errore
*/
UINT CGzw::CompressFile(LPCSTR lpcszInFile,LPCSTR lpcszOutFile,LPCSTR lpcszOpenFlag,GZWHDR* lpHdr)
{
	FILE* fp;
	gzFile gp;

	// apre il file di input
	if((fp = fopen(lpcszInFile,"rb"))==(FILE*)NULL)
		return(GZWE_OPEN_FAILURE);

	// crea (o apre, per piu' files di input) il .gzw
	if((gp = gzopen(lpcszOutFile,lpcszOpenFlag,lpHdr))==(gzFile)NULL)
	{
		fclose(fp);
		return(GZWE_CREATE_FAILURE);
	}

	// compressione
	if(gzcompress(fp,gp)!=0)
	{
		fclose(fp);
		gzclose(gp);
		return(GZWE_COMPRESS_FAILURE);
	}
    
	// chiude il file di input
	fclose(fp);

	// chiude il file .gzw
	if(gzclose(gp)!=Z_OK)
		return(GZWE_CLOSE_FAILURE);

	return(GZW_SUCCESS);
}

/*
	Uncompress()
	
	Decomprime lo skeleton .gzw.
	Per ognuno dei files .gzw espansi dallo skeleton chiama UncompressFile().
	La decompressione avviene nella directory corrente a meno che non venga specificato, come directory di
	output (nel campo della struttura), un identificativo di drive o directory.
	Con l'opzione -p viene ricreato il pathname (memorizzato in compressione) in modo assoluto, ossia
	ignorando l'eventuale directory di output. Cio' significa che se in compressione e' stato specificato
	a:\..., la funzione estrarra' i file contenuti nel .gzw su a:\..., generando errore se il file non puo'
	essere creato.
	Con l'opzione -P viene estratto il pathname (memorizzato in compressione), in modo relativo, ossia a
	partire dalla directory corrente o da quella di output.
	Con l'opzione  -r cerca lo skeleton di input nelle sub-directory tramite il meccanismo del file script
	descritto per Compress().
	L'opzione  -w permette di specificare, in decompressione, lo skeleton da estrarre, distinguendo tra
	maiuscole e minuscole, per cui *.c e' diverso da *.C.
*/
UINT CGzw::Uncompress(void)
{
	LPSTR lpszInFile;
	UINT nRet = GZWE_UNCOMPRESS_FAILURE;
	char szPathName[_MAX_PATH+1];
	char szSkeleton[_MAX_PATH+1];

	// espande lo skeleton e decomprime i files di input
	m_FindFile.SplitPathName(m_Gzw.szInputFile,szPathName,sizeof(szPathName),szSkeleton,sizeof(szSkeleton));

	while((lpszInFile = (LPSTR)m_FindFile.FindEx(szPathName,szSkeleton,m_Gzw.wFlag.recurse))!=NULL)
	{
		// pseudo-multitasking
		::Sleep(1);

		// ricava il nome del file .gzw di input da decomprimere
		memset(m_Gzw.szInputFile,'\0',sizeof(m_Gzw.szInputFile));
		strncpy(m_Gzw.szInputFile,lpszInFile,sizeof(m_Gzw.szInputFile));
		m_Gzw.nInputFileLen = strlen(lpszInFile);
		
		// decomprime il file
		nRet = UncompressFile();
	}

	return(nRet);
}

/*
	UncompressFile()
	
	Decomprime il file .gzw.
	Gestisce la decompressione tramite gli stream (FILE*).
	Attualmente non esiste un modo per estrarre un unico file in modo diretto. Per estrarre solo i file
	necessari, utilizzare le wildcards con l'opzione -w.
*/
UINT CGzw::UncompressFile(void)
{
	GZWHDR	stHeader;
	FILE*	pStreamOut;
	FILE*	pStreamIn;
	gzFile	pStreamGz;
	LPSTR	lpszTmpFile;
	char		szInputFile[_MAX_PATH+1];
	char		szOutputFile[_MAX_PATH+1];
	char		szPassword[GZW_PSW_MAX+1];
	char		szPsw[GZW_PSW_MAX+1];
	DWORD	dwFileSize = 0L;
	UINT		nTotFiles = 0;
	UINT		nRet = GZW_SUCCESS;
    
	// inizializza l'header
	memset(&stHeader,'\0',sizeof(GZWHDR));

	// carica il nome del file .gzw nel buffer
	memset(szInputFile,'\0',sizeof(szInputFile));
	memcpy(szInputFile,m_Gzw.szInputFile,m_Gzw.nInputFileLen);

	// carica la password nel buffer
	if(m_Gzw.nPswLen > 0)
	{
		memset(szPassword,'\0',sizeof(szPassword));
		memcpy(szPassword,m_Gzw.szPsw,m_Gzw.nPswLen);
	}

	// controlla se il file e' in formato .gzw
	if((nRet = CheckHeader(szInputFile))!=GZW_SUCCESS)
		return(nRet);

	// apre il file .gzw
	if((pStreamIn = fopen(szInputFile,"rb"))==(FILE*)NULL)
		return(GZWE_OPEN_FAILURE);

	// ricava la dimensione del file .gzw
	fseek(pStreamIn,0L,SEEK_END);
	dwFileSize = (DWORD)ftell(pStreamIn);
	if(fseek(pStreamIn,0L,SEEK_SET)!=0)
	{
		fclose(pStreamIn);
		return(GZWE_SEEK_FAILURE);
	}

	/*
	cicla per ognuno dei file compressi contenuti nel .gzw (soddisfacenti lo skeleton per -w):
	- lettura dell'header (fisso) del file compresso
	- lettura nell'header (variabile) del nome file/estensione e conversione dell'eventuale password
	- estrazione del file compresso nel corrispondente .tmp
	- decompressione ed eliminazione del .tmp
	*/
	while((lpszTmpFile = (LPSTR)ExtractFile(pStreamIn,&stHeader,dwFileSize,&nRet))!=NULL)
	{
		// pseudo-multitasking
		::Sleep(1);

		// inizializza il buffer per il nome del file da estrarre dal .gzw
		memset(szOutputFile,'\0',sizeof(szOutputFile));

		if(m_Gzw.wFlag.path) // con l'opzione -P ricrea il pathname a partire dalla directory specificata (relativo)
		{
			UINT wCount = 0;
			char szCurdir[_MAX_PATH+1];

			// nessuna directory di output, ricava quella corrente
			if(m_Gzw.nOutputFileLen==0)
				::GetCurrentDirectory(_MAX_PATH,m_Gzw.szOutputFile);

			// aggiunge lo \ finale alla directory di output
			::EnsureBackslash(m_Gzw.szOutputFile,sizeof(m_Gzw.szOutputFile));
			
			// copia la directory nel buffer
			strncpy(szCurdir,m_Gzw.szOutputFile,sizeof(szCurdir));
			
			// controlla se il nome file nell'header inizia con un nome o con l'identificativo del drive
			if(isalpha(stHeader.pFileName[0]))
				if(stHeader.pFileName[1]==':')
					wCount = 1;
					
			// salta gli \ iniziali
			while(!isalpha(stHeader.pFileName[wCount]))
				wCount++;

			// aggiunge alla directory di output il nome file presente nell'header
			strncat(szCurdir,(stHeader.pFileName+wCount),sizeof(szCurdir)-1);

			strcpyn(szOutputFile,szCurdir,sizeof(szOutputFile));
			char* p = strrchr(szOutputFile,'\\');
			if(p)
				*p = '\0';
			
			// ricrea la directory di output
			if(!m_FindFile.CreatePathName(szOutputFile,sizeof(szOutputFile)))
			{
				::DeleteFile(lpszTmpFile);
				continue;
			}
			
			// imposta il nome file di output
			strcpyn(szOutputFile,szCurdir,sizeof(szOutputFile));
		}
		else if(m_Gzw.wFlag.pathname) // con l'opzione -p ricrea il pathname a partire dalla radice (assoluto)
		{
			memset(szOutputFile,'\0',sizeof(szOutputFile));
			memcpy(szOutputFile,stHeader.pFileName,stHeader.stGzwHdr.nFileNameLen);
			char* p = strrchr(szOutputFile,'\\');
			if(p)
				*p = '\0';

			// ricrea la directory di output
			if(!m_FindFile.CreatePathName(szOutputFile,sizeof(szOutputFile)))
			{
				::DeleteFile(lpszTmpFile);
				continue;
			}
				
			// imposta il nome file di output
			memcpy(szOutputFile,stHeader.pFileName,stHeader.stGzwHdr.nFileNameLen);
		}
		else // senza le opzioni -p|-P elimina l'eventuale pathname presente nell'header, creando il file nella directory corrente/di output
		{
			// elimina l'eventuale pathname dal nome file presente nell'header, se non riesce passa al file successivo
			if((stHeader.stGzwHdr.nFileNameLen = StripPath(stHeader.pFileName,stHeader.stGzwHdr.nFileNameLen)) < 0)
			{
				::DeleteFile(lpszTmpFile);
				continue;
			}
					
			// se e' stata specificata una directory di output ricrea il nome per il file di output
			if(m_Gzw.nOutputFileLen!=0)
			{    
				// inserisce la directory di output prima del nome del file
				if((m_Gzw.nOutputFileLen+stHeader.stGzwHdr.nFileNameLen+1) <= _MAX_PATH)
				{
					memcpy(szOutputFile,m_Gzw.szOutputFile,m_Gzw.nOutputFileLen);
	
					if(m_Gzw.szOutputFile[m_Gzw.nOutputFileLen-1]!='\\')
					{
						szOutputFile[m_Gzw.nOutputFileLen] = '\\';
						memcpy(((szOutputFile+m_Gzw.nOutputFileLen)+1),stHeader.pFileName,stHeader.stGzwHdr.nFileNameLen);
					}
					else
						memcpy(szOutputFile+m_Gzw.nOutputFileLen,stHeader.pFileName,stHeader.stGzwHdr.nFileNameLen);
				}
				else // directory di output troppo lunga, ripristina solo il nome del file
				{
					memcpy(szOutputFile,stHeader.pFileName,stHeader.stGzwHdr.nFileNameLen);
				}
			}
			else // nessuna directory di output, ripristina solo il nome del file
			{
				memcpy(szOutputFile,stHeader.pFileName,stHeader.stGzwHdr.nFileNameLen);
			}
		}

		// chiama la callback
		if(m_Gzw.fpCallBack)
			m_Gzw.fpCallBack(GZW_CALLBACK_UNCOMPRESS_BEGIN,(LPARAM)&stHeader,(LPARAM)szOutputFile);

		// controlla se il file e' protetto da password
		if(stHeader.stGzwHdr.nPswLen > 0)
		{
			// copia la password (in chiaro) nel buffer
			memset(szPsw,'\0',sizeof(szPsw));
			memcpy(szPsw,szPassword,stHeader.stGzwHdr.nPswLen);
		
			// mette in xor la password nel buffer
			memnxor(szPsw,szPassword,stHeader.stGzwHdr.nPswLen);
			
			// confronta la password del buffer (in xor) con quella dell'header
			if(memcmp(szPsw,stHeader.stGzwHdr.szPsw,stHeader.stGzwHdr.nPswLen)!=0)
			{
				// password errata
				fclose(pStreamIn);
				::DeleteFile(lpszTmpFile);
				return(GZWE_WRONG_PASSWORD);
			}
		}
          
		// apre il file .tmp (contenente i dati compressi) creato da ExtractFile()
		if((pStreamGz = gzopen(lpszTmpFile,"rb",&stHeader))==(gzFile)NULL)
		{
			fclose(pStreamIn);
			::DeleteFile(lpszTmpFile);
			return(GZWE_OPEN_FAILURE);
		}

		/* crea il file di output relativo al .tmp */
		if((pStreamOut = fopen(szOutputFile,"wb"))==(FILE*)NULL)
		{
			fclose(pStreamIn);
			gzclose(pStreamGz);
			::DeleteFile(lpszTmpFile);
			return(GZWE_CREATE_FAILURE);
		}

		/* decompressione */
		if(gzuncompress(pStreamGz,pStreamOut)!=0)
		{
			/* decompressione fallita */
			fclose(pStreamIn);
			gzclose(pStreamGz);
			fclose(pStreamOut);
			::DeleteFile(lpszTmpFile);
			return(GZWE_UNCOMPRESS_FAILURE);
		}

		/* chiude il .tmp */
		if(gzclose(pStreamGz)!=Z_OK)
		{
			fclose(pStreamIn);
			fclose(pStreamOut);
			::DeleteFile(lpszTmpFile);
			return(GZWE_CLOSE_FAILURE);
		}

		/* chiude il file decompresso */
		fclose(pStreamOut);

		/* reimposta la data/ora originali */
		m_FindFile.SetFileTime(szOutputFile,(WORD)stHeader.stGzwHdr.wFileDate,(WORD)stHeader.stGzwHdr.wFileTime);

		/* elimina il file temporaneo */
		::DeleteFile(lpszTmpFile);
		
		/* incrementa il numero di files estratti */
		nTotFiles++;

		// chiama la funzione callback
		if(m_Gzw.fpCallBack)
			m_Gzw.fpCallBack(GZW_CALLBACK_UNCOMPRESS_END,(LPARAM)&stHeader,NULL);
	}

	/* chiude il file .gzw */
	fclose(pStreamIn);

	/* elimina il file .gzw per -x */
	if(m_Gzw.wFlag.extract)
		::DeleteFile(szInputFile);

	return(nRet);
}

/*
	List()
	
	Lista o visualizza lo skeleton .gzw.
	Con l'opzione -l il nome file per la lista deve essere passato nel campo della struttura, mentre con
	l'opzione -v genera in proprio il nome file.
	Non disponendo di un device di output, la lista viene implementata solo su file, mentre la visualizzazione
	utilizza un viewer o la clipboard. Il nome del viewer deve essere passato nel campo della struttura.
	Se viene passato il nome 'clipboard', il contenuto del .gzw viene listato nella clipboard.
	Per ognuno dei file .gzw espansi dallo skeleton chiama ListFile().
	Con l'opzione  -r cerca lo  skeleton di input nelle sub-directory tramite il meccanismo del file script
	descritto per Compress().
	L'opzione -w permette di specificare, in lista/visualizzazione, lo skeleton da estrarre. 
	Notare che l'opzione distingue tra maiuscole e minuscole, per cui *.c e' diverso da *.C.
*/
UINT CGzw::List(void)
{
	LPSTR	lpszInFile;
	FILE*	pStreamOut;
	UINT		nRet = GZW_SUCCESS;
	char		szPathName[_MAX_PATH+1];
	char		szSkeleton[_MAX_PATH+1];
		
	// chiama la callback
	if(m_Gzw.fpCallBack!=NULL)
		m_Gzw.fpCallBack(GZW_CALLBACK_LIST_BEGIN,(LPARAM)NULL,NULL);

	/* crea il file per la lista/visualizzazione, ListFile() lo riapre in append */
	if(m_Gzw.szOutputFile[0]!='\0')
	{
		if(stricmp(m_Gzw.szOutputFile,"callback")!=0)
		{
			if((pStreamOut = fopen(m_Gzw.szOutputFile,"wt"))==(FILE*)NULL)
				nRet = GZWE_CREATE_FAILURE;
			else
				fclose(pStreamOut);
		}
	}

	// espande lo skeleton
	if(nRet==GZW_SUCCESS)
	{
		m_FindFile.SplitPathName(m_Gzw.szInputFile,szPathName,sizeof(szPathName),szSkeleton,sizeof(szSkeleton));
		while((lpszInFile = (LPSTR)m_FindFile.FindEx(szPathName,szSkeleton,m_Gzw.wFlag.recurse))!=NULL)
		{
			::Sleep(1);

			/* copia il nome del file espanso nella struttura */
			memset(m_Gzw.szInputFile,'\0',sizeof(m_Gzw.szInputFile));
			strncpy(m_Gzw.szInputFile,lpszInFile,sizeof(m_Gzw.szInputFile));
			m_Gzw.nInputFileLen = strlen(lpszInFile);
			
			/* lista/visualizzazione */
			if((nRet = ListFile())!=GZW_SUCCESS)
				break;
		}
	}
		
	// chiama la callback
	if(m_Gzw.fpCallBack!=NULL)
		m_Gzw.fpCallBack(GZW_CALLBACK_LIST_END,(LPARAM)NULL,NULL);

	return(nRet);
}

/*
	ListFile()
	
	Lista o visualizza i file compressi contenuti nel .gzw.
	Attualmente non esiste un modo per listare/visualizzare un unico file in modo diretto.
	Per listare/visualizzare i file necessari, utilizzare le wildcards con l'opzione -w.
*/
UINT CGzw::ListFile(void)
{
	GZWHDR	stHeader;					/* header GZW */
	FILE*	pStreamOut = NULL;				/* stream per il file decompresso */
	FILE*	pStreamIn;				/* stream per il file .gzw */
	char		szInputFile[_MAX_PATH+1];	/* buffer per il file da decomprimere */
	char		szPassword[GZW_PSW_MAX+1];	/* buffer per la password */
	char		szPsw[GZW_PSW_MAX+1];	/* buffer per la password */
	DWORD	dwFileSize = 0L;			/* dimensione del file .gzw */
	DWORD	dwTotSize = 0L,			/* totali bytes originali/compressi */
			dwTotCompressed = 0L;
	UINT		nTotFiles = 0;				/* totale files */
	char		szLtosO[16],			/* buffer per ltos() */
			szLtosC[16];
	char		szDtos[32];			/* buffer per datetostr() */
	BOOL		bHeader = FALSE;			/* flag per intestazione */
	UINT		nRet = GZW_SUCCESS;		/* codice di ritorno */

	/* inizializza l'header */
	memset(&stHeader,'\0',sizeof(GZWHDR));

	/* carica il nome del file .gzw nel buffer */
	memset(szInputFile,'\0',sizeof(szInputFile));
	memcpy(szInputFile,m_Gzw.szInputFile,m_Gzw.nInputFileLen);

	/* carica la password nel buffer */
	if(m_Gzw.nPswLen > 0)
	{
		memset(szPassword,'\0',sizeof(szPassword));
		memcpy(szPassword,m_Gzw.szPsw,m_Gzw.nPswLen);
	}

	/* controlla se il file e' in formato .gzw */
	if((nRet = CheckHeader(szInputFile))!=GZW_SUCCESS)
		return(nRet);

	/* apre il file .gzw */
	if((pStreamIn = fopen(szInputFile,"rb"))==(FILE*)NULL)
		return(GZWE_OPEN_FAILURE);

	/* ricava la dimensione del file .gzw */
	fseek(pStreamIn,0L,SEEK_END);
	dwFileSize = (DWORD)ftell(pStreamIn);
	if(fseek(pStreamIn,0L,SEEK_SET)!=0)
	{
		fclose(pStreamIn);
		return(GZWE_SEEK_FAILURE);
	}

	/* apre il file per la lista/visualizzazione */
	if(m_Gzw.szOutputFile[0]=='\0')
		pStreamOut = stdout;
	else
	{
		if(stricmp(m_Gzw.szOutputFile,"callback")!=0)
			if((pStreamOut = fopen(m_Gzw.szOutputFile,"at"))==(FILE*)NULL)
			{
				fclose(pStreamIn);
				return(GZWE_OPEN_FAILURE);
			}
	}

	/*
	cicla per ognuno dei file compressi contenuti nel .gzw
	la chiamata comporta la lettura dell'header del file compresso con il ripristino, 
	nell'header, del nome file/estensione originale e della password posta in xor
	*/
	while(ExtractHeader(pStreamIn,&stHeader,dwFileSize,&nRet)!=NULL)
	{
		::Sleep(1);

		/* con l'opzione -w  controlla che il nome file soddisfi lo skeleton */
		if(m_Gzw.szWildcardsIncl[0]!='\0')
			if(!m_WildCardsInclude.Match(m_Gzw.szWildcardsIncl,stHeader.pFileName))
				continue;
		if(m_Gzw.szWildcardsExcl[0]!='\0')
			if(m_WildCardsExclude.Match(m_Gzw.szWildcardsExcl,stHeader.pFileName))
				continue;

		/* controlla se il file e' protetto da password */
		if(stHeader.stGzwHdr.nPswLen > 0)
		{
			/* copia la password (in chiaro) nel buffer */
			memset(szPsw,'\0',sizeof(szPsw));
			memcpy(szPsw,szPassword,stHeader.stGzwHdr.nPswLen);
		
			/* mette in xor la password nel buffer */	
			memnxor(szPsw,szPassword,stHeader.stGzwHdr.nPswLen);
			
			/* confronta la password del buffer (in xor) con quella dell'header */
			if(memcmp(szPsw,stHeader.stGzwHdr.szPsw,stHeader.stGzwHdr.nPswLen)!=0)
			{
				/* password errata */
				fclose(pStreamIn);
				if(m_Gzw.wFlag.list)
					if(pStreamOut && pStreamOut!=stdout)
						fclose(pStreamOut);
				return(GZWE_WRONG_PASSWORD);
			}
		}
		
		/* intestazione */
		if(!bHeader)
		{
			bHeader = 1;
			if(pStreamOut)
			{
				fprintf(pStreamOut,"%s:\n\n",szInputFile);
				fprintf(pStreamOut,"%13s %13s %-6s %-4s %11s      %s\n\n","uncompressed","compressed","ratio","date","time","filename");
			}
		}
		
		/* lista/visualizzazione */
		if(pStreamOut)
		fprintf(	pStreamOut,"%13s %13s  %%%-3d  %-21s %s\n",
				ltos(stHeader.stGzwHdr.dwFileSize,szLtosO,sizeof(szLtosO)),
				ltos(stHeader.stGzwHdr.dwFileCompressedSize,szLtosC,sizeof(szLtosC)),
				(int)((100L*stHeader.stGzwHdr.dwFileCompressedSize)/stHeader.stGzwHdr.dwFileSize),
				datetostr(stHeader.stGzwHdr.wFileDate,stHeader.stGzwHdr.wFileTime,szDtos,sizeof(szDtos)),
				stHeader.pFileName
				);
		
		// chiama la callback
		if(m_Gzw.fpCallBack!=NULL)
			m_Gzw.fpCallBack(GZW_CALLBACK_LIST,(LPARAM)&stHeader,NULL);
		
		/* incrementa i totali */
		dwTotSize += stHeader.stGzwHdr.dwFileSize;
		dwTotCompressed += stHeader.stGzwHdr.dwFileCompressedSize;
		nTotFiles++;
	}

	/* stampa i totali */
	if(pStreamOut && nTotFiles > 0)
		fprintf(pStreamOut,"\n%13s %13s  %%%-3ld %22s %d file(s)\n",ltos(dwTotSize,szLtosO,sizeof(szLtosO)),ltos(dwTotCompressed,szLtosC,sizeof(szLtosC)),(dwTotSize > 0) ? (100L*dwTotCompressed)/dwTotSize : 0," ",nTotFiles);

	/* chiude il file .gzw ed il file per la lista/visualizzazione */
	fclose(pStreamIn);

if(pStreamOut && pStreamOut!=stdout)
	fclose(pStreamOut);

	return(nRet);
}

/*
	CheckHeader()
	
	Controlla se il file e' in formato .gzw confrontando la signature.
*/
UINT CGzw::CheckHeader(LPCSTR lpszFileName)
{
	HFILE	hHandle;				/* handle per il file da controllare */
	GZWHEADER	stGzwHdr;				/* header */
	UINT		nRet = GZW_SUCCESS;	/* codice di ritorno */
	
	if(!*lpszFileName)
		return(GZWE_INVALID_FILENAME);

	if((hHandle=_lopen(lpszFileName,OF_READ))==HFILE_ERROR)
		return(GZWE_NOSUCHFILE);

	/* salta l'header GZ originale */
	_lread(hHandle,(LPVOID)&stGzwHdr,GZ_HEADER_LEN);
	
	/* legge l'header GZW */
	_lread(hHandle,(LPVOID)&stGzwHdr,sizeof(GZWHEADER));

	_lclose(hHandle);
		
	/* controlla il formato */
	nRet = memcmp(&(stGzwHdr.szSignature),GZW_SIGNATURE,GZW_SIGN_LEN)==0 ? GZW_SUCCESS : GZWE_INVALID_FORMAT;

	return(nRet);
}

/*
	ExtractFile()
	
	Utilizzata per la decompressione del .gzw, estrae dal .gzw i files in esso contenuti creando i .tmp
	relativi da passare a gzuncompress().
	L'estrazione avviene scorrendo gli header contenuti nel file .gzw.
	Durante l'estrazione si occupa di controllare che il nome file soddisfi l'eventuale skeleton specificato
	dall'opzione -w.
	Notare che l'opzione -w, utilizzata per estrarre soltanto lo skeleton specificato, distingue tra maiuscole
	e minuscole.
*/
LPCSTR CGzw::ExtractFile(FILE *pStream,GZWHDR* pstHeader,DWORD dwFileSize,UINT* nRet)
{
	UINT			wCount;					/* contatore generico */
	DWORD		dwCount;					/* contatore generico */
	static DWORD	dwFilePointer=0L;			/* offset nel file .gzw */
	static DWORD	dwTotData=0L;				/* dimensione totale dei dati compressi */
	static UINT	wTotHeaders=1;				/* totale headers GZW letti */
	static char	szFileName[_MAX_PATH+1];	/* nome del file compresso */
	static char	szTmpFile[_MAX_PATH+1];	/* nome del file .tmp */
	static char	szBuffer[4096];			/*  */
	BOOL			bWld=TRUE;				/* flag per wildcards */
	static LPSTR	lpPtr=NULL;			/* puntatore per il nome file .tmp */
	FILE			*lpTmpFile;
	
	/* imposta il nome file per il .tmp */
	strcpyn(szTmpFile,TmpName(),sizeof(szTmpFile));

extract: /* loop per scansione su skeleton non soddisfatto per -w */

	/* controlla se e' arrivato alla fine del file .gzw */
	if(dwFilePointer >= dwFileSize)
	{
		::DeleteFile(szTmpFile);				/* elimina il temporaneo relativo al file estratto dal .gzw */
		dwFilePointer=dwTotData=0L;		/* (ri)inizializza per le chiamate successive */
		wTotHeaders=1;					/* (ri)inizializza per le chiamate successive */
		*nRet=GZW_SUCCESS;			/* imposta il codice di ritorno */
		return(NULL);
	}
    
	/* posiziona (nel .gzw) il puntatore sull'offset relativo al successivo file compresso */
	if(fseek(pStream,dwFilePointer,SEEK_SET)!=0)
	{
		::DeleteFile(szTmpFile);				/* elimina il temporaneo relativo al file estratto dal .gzw */
		dwFilePointer=dwTotData=0L;		/* (ri)inizializza per le chiamate successive */
		wTotHeaders=1;					/* (ri)inizializza per le chiamate successive */
		*nRet=GZWE_SEEK_FAILURE;	/* imposta il codice di ritorno */
		return(NULL);
	}
    
	/* crea il .tmp su cui estrarre il file compresso dal .gzw */
	if((lpTmpFile=fopen(szTmpFile,"wb"))==(FILE *)NULL)
	{
		dwFilePointer=dwTotData=0L;		/* (ri)inizializza per le chiamate successive */
		wTotHeaders=1;					/* (ri)inizializza per le chiamate successive */
		*nRet=GZWE_CREATE_FAILURE;	/* imposta il codice di ritorno */
		return(NULL);
	}
    
	/* legge dal .gzw l'header GZ originale e lo trascrive nel .tmp */
	for(wCount=0; wCount < GZ_HEADER_LEN; wCount++)
	{
		if(fputc(fgetc(pStream),lpTmpFile)==EOF)
		{
			fclose(lpTmpFile);				/* chiude il temporaneo */
			::DeleteFile(szTmpFile);				/* elimina il temporaneo relativo al file estratto dal .gzw */
			dwFilePointer=dwTotData=0L;		/* (ri)inizializza per le chiamate successive */
			wTotHeaders=1;					/* (ri)inizializza per le chiamate successive */
			*nRet=GZWE_WRITE_FAILURE;	/* imposta il codice di ritorno */
			return(NULL);
		}
	}

	/* legge dal .gzw l'header GZW e lo trascrive nel .tmp */	
	fread(&(pstHeader->stGzwHdr),1,sizeof(GZWHEADER),pStream);
	if(fwrite(&(pstHeader->stGzwHdr),1,sizeof(GZWHEADER),lpTmpFile)!=sizeof(GZWHEADER))
	{
		fclose(lpTmpFile);				/* chiude il temporaneo */
		::DeleteFile(szTmpFile);				/* elimina il temporaneo relativo al file estratto dal .gzw */
		dwFilePointer=dwTotData=0L;		/* (ri)inizializza per le chiamate successive */
		wTotHeaders=1;					/* (ri)inizializza per le chiamate successive */
		*nRet=GZWE_WRITE_FAILURE;	/* imposta il codice di ritorno */
		return(NULL);
	}
	
	/* controlla che il nome file abbia una dimensione consentita (ossia confacente al buffer) */
	if(pstHeader->stGzwHdr.nFileNameLen > _MAX_PATH)
	{
		fclose(lpTmpFile);				/* chiude il temporaneo */
		::DeleteFile(szTmpFile);					/* elimina il temporaneo relativo al file estratto dal .gzw */
		dwFilePointer=dwTotData=0L;			/* (ri)inizializza per le chiamate successive */
		wTotHeaders=1;						/* (ri)inizializza per le chiamate successive */
		*nRet=GZWE_INVALID_FILENAME;	/* imposta il codice di ritorno */
		return(NULL);
	}

	/* legge dal .gzw il nome del file compresso, lo trascrive nel .tmp e lo associa al puntatore dell'header */
	memset(szFileName,(int)NULL,sizeof(szFileName));
	fread(szFileName,1,pstHeader->stGzwHdr.nFileNameLen,pStream);
	if((UINT)fwrite(szFileName,1,(size_t)pstHeader->stGzwHdr.nFileNameLen,lpTmpFile)!=pstHeader->stGzwHdr.nFileNameLen)
	{
		fclose(lpTmpFile);				/* chiude il temporaneo */
		::DeleteFile(szTmpFile);				/* elimina il temporaneo relativo al file estratto dal .gzw */
		dwFilePointer=dwTotData=0L;		/* (ri)inizializza per le chiamate successive */
		wTotHeaders=1;					/* (ri)inizializza per le chiamate successive */
		*nRet=GZWE_WRITE_FAILURE;	/* imposta il codice di ritorno */
		return(NULL);
	}
	szFileName[pstHeader->stGzwHdr.nFileNameLen]=(char)NULL;
	pstHeader->pFileName=szFileName;
    
    /* confronta il nome file contenuto nel .gzw con l'eventuale skeleton */
	if(m_Gzw.szWildcardsIncl[0]!='\0')
	{
		if(!m_WildCardsInclude.Match(m_Gzw.szWildcardsIncl,szFileName))
			bWld=FALSE;
	}
	if(m_Gzw.szWildcardsExcl[0]!='\0')
	{
		if(m_WildCardsExclude.Match(m_Gzw.szWildcardsExcl,szFileName))
			bWld=FALSE;
	}
	     
	/* legge i dati compressi dal .gzw e li trascrive nel .tmp */
	if(bWld)
	{
		for(dwCount=0L; dwCount < pstHeader->stGzwHdr.dwFileCompressedSize; dwCount++)
		{
			if(fputc(fgetc(pStream),lpTmpFile)==EOF)
			{
				fclose(lpTmpFile);				/* chiude il temporaneo */
				::DeleteFile(szTmpFile);				/* elimina il temporaneo relativo al file estratto dal .gzw */
				dwFilePointer=dwTotData=0L;		/* (ri)inizializza per le chiamate successive */
				wTotHeaders=1;					/* (ri)inizializza per le chiamate successive */
				*nRet=GZWE_WRITE_FAILURE;	/* imposta il codice di ritorno */
				return(NULL);
			}
		}
	}
    
	/* aggiorna il totale relativo ai dati compressi letti */
	dwTotData+=pstHeader->stGzwHdr.dwFileCompressedSize+pstHeader->stGzwHdr.nFileNameLen;
    
	/* aggiorna l'offset all'interno del file .gzw (headers * tot.headers letti + tot.dati letti + tot.lunghezza nomi file letti) */
	dwFilePointer=(((DWORD)GZ_HEADER_LEN+(DWORD)sizeof(GZWHEADER))*(DWORD)wTotHeaders)+dwTotData;
	
	/* aggiorna il numero di headers letti per gli skip successivi */
	wTotHeaders++;
	
	/* chiude il file .tmp */
	if(fclose(lpTmpFile)!=0)
	{
		::DeleteFile(szTmpFile);				/* elimina il temporaneo relativo al file estratto dal .gzw */
		dwFilePointer=dwTotData=0L;		/* (ri)inizializza per le chiamate successive */
		wTotHeaders=1;					/* (ri)inizializza per le chiamate successive */
		*nRet=GZWE_CLOSE_FAILURE;	/* imposta il codice di ritorno */
		return(NULL);
	}
	
	/* posta a FALSE solo se, con l'opzione -w, il nome file non soddisfa lo skeleton */
	if(!bWld)
	{
		::DeleteFile(szTmpFile);				/* elimina il temporaneo relativo al file estratto dal .gzw */
		bWld=TRUE;
		goto extract;
	}

	/* ritorna il nome del file .tmp per l'apertura dello stream da passare a gzuncompress() */	
	return(szTmpFile);
}

/*
	ExtractHeader()
	
	Utilizzata per la lista/visualizzazione del .gzw, legge gli headers dei files compressi.
	
	input:
	FILE *	stream del file .gzw
	GZWHDR*	puntatore alla struttura per l'header,
	DWORD	offset corrente nel file .gzw
	UINT*	puntatore alla word per il codice d'errore
	
	output:
	LPSTR	puntatore al nome file compresso posto all'interno del .gzw
*/
LPCSTR CGzw::ExtractHeader(FILE *pStream,GZWHDR* pstHeader,DWORD dwFileSize,UINT* nRet)
{
	UINT			wCount;					/* contatore generico */
	static DWORD	dwFilePointer=0L;			/* offset nel file .gzw */
	static DWORD	dwTotData=0L;				/* dimensione totale dei dati compressi */
	static UINT	wTotHeaders=1;				/* totale headers GZW letti */
	static char	szFileName[_MAX_PATH+1];	/* buffer per il nome del file compresso */

	/* controlla se e' arrivato alla fine del file .gzw */
	if(dwFilePointer >= dwFileSize)
	{
		dwFilePointer=dwTotData=0L;	/* (ri)inizializza per le chiamate successive */
		wTotHeaders=1;				/* (ri)inizializza per le chiamate successive */
		*nRet=GZW_SUCCESS;		/* imposta il codice di ritorno */
		return(NULL);
	}
    
	/* posiziona (nel .gzw) il puntatore sull'offset relativo al successivo file compresso */
	if(fseek(pStream,dwFilePointer,SEEK_SET)!=0)
	{
		dwFilePointer=dwTotData=0L;		/* (ri)inizializza per le chiamate successive */
		wTotHeaders=1;					/* (ri)inizializza per le chiamate successive */
		*nRet=GZWE_SEEK_FAILURE;	/* imposta il codice di ritorno */
		return(NULL);
	}
    
	/* salta l'header GZ originale */
	for(wCount=0; wCount < GZ_HEADER_LEN; wCount++)
		fgetc(pStream);
	
	/* legge (e controlla) l'header GZW */	
	fread(pstHeader,1,sizeof(GZWHEADER),pStream);
	if(pstHeader->stGzwHdr.dwFileSize <= 0 || pstHeader->stGzwHdr.dwFileCompressedSize <= 0 || pstHeader->stGzwHdr.nFileNameLen <= 0 || pstHeader->stGzwHdr.nFileNameLen > _MAX_PATH)
	{
		dwFilePointer=dwTotData=0L;		/* (ri)inizializza per le chiamate successive */
		wTotHeaders=1;					/* (ri)inizializza per le chiamate successive */
		*nRet=GZWE_INVALID_FORMAT;	/* imposta il codice di ritorno */
		return(NULL);
	}

	/* legge dal .gzw il nome del file compresso e lo associa al puntatore dell'header */
	memset(szFileName,(int)NULL,sizeof(szFileName));
	fread(szFileName,1,pstHeader->stGzwHdr.nFileNameLen,pStream);
	szFileName[pstHeader->stGzwHdr.nFileNameLen]=(char)NULL;
	pstHeader->pFileName=szFileName;
		
	/* aggiorna il totale relativo ai dati compressi letti (include nel totale il nome del file compresso) */
	dwTotData+=pstHeader->stGzwHdr.dwFileCompressedSize+pstHeader->stGzwHdr.nFileNameLen;
    
	/* aggiorna l'offset all'interno del file .gzw (headers * tot.headers letti + tot.dati letti + tot.lunghezza nomi file letti) */
	dwFilePointer=(((DWORD)GZ_HEADER_LEN+(DWORD)sizeof(GZWHEADER))*(DWORD)wTotHeaders)+dwTotData;
	
	/* aggiorna il numero di headers letti per gli skip successivi */
	wTotHeaders++;
	
	/* ritorna il nome del file compresso */	
	return(pstHeader->pFileName);
}

LPCSTR CGzw::ParseScript(LPCSTR lpszFileSpec,UINT* nRet)
{    
	FILE* pStream;
	static DWORD dwFilePointer = 0L;
	static char szFileSpec[_MAX_PATH+1];
	LPSTR lpszFileName = NULL;
	char* p;

	/* controlla la validita' del nome file */
	if(!*lpszFileSpec)
	{
		*nRet = GZWE_INVALID_FILENAME;
		return(NULL);
	}
	
	/* imposta il ciclo per saltare al file successivo se quello appena letto dallo script non esiste */
	while(TRUE)
	{
		/* apre il file script */
		if((pStream = fopen(lpszFileSpec,"rt"))!=(FILE*)NULL)
		{
			/* (ri)posiziona */
			fseek(pStream,dwFilePointer,SEEK_SET);
			    
			/* legge una linea */
		    	if(fgets(szFileSpec,sizeof(szFileSpec)-1,pStream)!=NULL)
		    	{
				/* aggiorna l'offset per la successiva lettura */
				dwFilePointer += strlen(szFileSpec)+1;
			    	
				/* elimina il \n di fine riga */
				if((p = strrchr(szFileSpec,'\n'))!=NULL)
					*p = '\0';
				//crlf(szFileSpec);
				
				lpszFileName = szFileSpec;
			}
			else /* eof sul file script */
			{
				dwFilePointer = 0L; /* (ri)inizializza per le chiamate successive */
			}
		
			fclose(pStream);
		}
		else
		{
			*nRet = GZWE_OPEN_FAILURE;
		}
		
		break;
	}

	return(lpszFileName);
}

/*
	StripPath()
	
	Elimina il pathname dal nome del file, restituendone la nuova lunghezza.
	
	input:
	LPSTR	puntatore al nome file
	UINT	lunghezza del nome file

	output:
	UINT	nuova lunghezza del nome file, 0=errore
*/
UINT CGzw::StripPath(LPSTR lpszFileName,UINT wFileLen)
{
	LPSTR	lpPtr;					/* puntatore generico */
	char		szBuffer[_MAX_PATH+1];	/* buffer per il nome file */
	
	/* controlla la validita' del nome file */
	if(!*lpszFileName || wFileLen <= 0 || wFileLen > _MAX_PATH)
		return(0);

	/* se il nome file non contiene un pathname ritorna */
	if(strchr(lpszFileName,'\\')==(char *)NULL)
		return(wFileLen);
	
	/* copia il nome del file */
	strcpyn(szBuffer,lpszFileName,sizeof(szBuffer));
	
	/* inverte il buffer per trovare l'inizio del pathname */
	strrev(szBuffer);
	
	/* cerca l'inizio del pathname */
	if((lpPtr=strchr(szBuffer,'\\'))==NULL)
		lpPtr=strchr(szBuffer,':');
	
	/* se trova un pathname lo elimina */
	if(lpPtr!=NULL)
		*lpPtr=(char)NULL;
		
	/* (re)inverte il buffer */
	strrev(szBuffer);
    
	/* copia il contenuto del buffer sul nome file */
	strcpy(lpszFileName,szBuffer);

	/* ritorna la lunghezza del nome file */
	return(strlen(lpszFileName));
}

/*
	AddAbsolutePath()
	
	Inserisce il pathname assoluto nel nome del file, restituendone la nuova lunghezza.
	Se il nome file contiene gia' un pathname ritorna, altrimenti inserisce quello relativo
	alla directory corrente.
	Il buffer contenente il nome del file deve essere di _MAX_PATH+1 caratteri.
	
	input:
	LPSTR	puntatore al nome file
	UINT		lunghezza del nome file
	
	output:
	UINT		nuova lunghezza del nome file, 0=errore
*/
UINT CGzw::AddAbsolutePath(LPSTR lpszFileName,UINT wFileLen)
{
	UINT	wLen;					/* nuova lunghezza del nome file */
	char	szBuffer[_MAX_PATH+1];	/* buffer per il nome file */
	
	/* controlla la validita' del nome file */
	if(!*lpszFileName || wFileLen <= 0)
		return(0);
	
	/* se il nome file contiene gia' un drive/pathname ritorna */
	if(strchr(lpszFileName,'\\')!=(char *)NULL || strchr(lpszFileName,':')!=(char *)NULL)
		return(wFileLen);

	/* ricava la directory corrente e aggiunge il nome file (lascia lo spazio per il nome file + NULL) */
	::GetCurrentDirectory((_MAX_PATH-wFileLen)-1,szBuffer);
	lstrcat(szBuffer,"\\");
	
	/* controlla la dimensione del buffer prima della copia (pathname+nomefile) */
	if((wLen=(UINT)strlen(szBuffer))+wFileLen > _MAX_PATH)
		return(0);

	/* aggiunge al pathname il nome del file */
	lstrcat(szBuffer,lpszFileName);

	/* copia il contenuto del buffer nel nome file */
	lstrcpy(lpszFileName,szBuffer);
		
	/* ritorna la lunghezza del nome file */
	return(strlen(lpszFileName));
}

/*
	AddRelativePath()
	
	Inserisce il pathname relativo nel nome del file, restituendone la nuova lunghezza.
	Il buffer contenente il nome del file deve essere di _MAX_PATH+1 caratteri.
	
	input:
	LPSTR	puntatore al nome file
	UINT	lunghezza del nome file
	
	output:
	UINT	nuova lunghezza del nome file, 0=errore
*/
UINT CGzw::AddRelativePath(LPSTR lpszFileName,UINT wFileLen)
{
	LPSTR	lpPtr;					/* puntatore generico */
	char		szBuffer[_MAX_PATH+1];	/* buffer per il nome file */
	
	/* controlla la validita' del nome file */
	if(!*lpszFileName || wFileLen <= 0)
		return(0);

	/* ricava la directory corrente */
	::GetCurrentDirectory(sizeof(szBuffer)-2,szBuffer);
	lstrcat(szBuffer,"\\");

	/* cerca la directory corrente nel nome del file e la elimina */
	if((lpPtr=strstr(lpszFileName,szBuffer))!=NULL)
	{
		LPSTR lpBuf=szBuffer;

		lpPtr += strlen(szBuffer);

		while(*lpPtr)
			*lpBuf++=*lpPtr++;

		*lpBuf=(char)NULL;

		lstrcpy(lpszFileName,szBuffer);
	}

	/* ritorna la lunghezza del nome file */
	return(strlen(lpszFileName));
}

/*
	Diff()
	
	Controlla che i due nomi non facciano riferimento allo stesso file.
	I nomi non possono eccedere i _MAX_PATH caratteri.
	Utilizzata per non includere nel .gzw di output il .gzw stesso quando viene passato *.*
	come input.
	
	input:	
	LPSTR	puntatore al primo nome file
	LPSTR	puntatore al secondo nome file

	output:
	UINT		0 uguali
			1 diversi
			2 nome file non valido
			3 lunghezza nome file eccessiva
*/
UINT CGzw::Diff(LPSTR lpszInFile,LPSTR lpszOutFile)
{
	UINT	iInLen,iOutLen;			/* lunghezza dei nomi file */
	UINT	iStrcmp;					/* flag per strcmp() */
	char	szInBuffer[_MAX_PATH+1],
		szOutBuffer[_MAX_PATH+1];
	
	/* controlla la validita' dei nomi file */
	if(!*lpszInFile || !*lpszOutFile)
		return(2);
     
	/* calcola (e controlla) la lunghezza dei nomi file */
	if((iInLen=(UINT)strlen(lpszInFile)) >= _MAX_PATH || (iOutLen=(UINT)strlen(lpszOutFile)) >= _MAX_PATH)
		return(3);
	
	/* copia i nomi file nei buffer locali */	
	strcpyn(szInBuffer,lpszInFile,sizeof(szInBuffer));
	strcpyn(szOutBuffer,lpszOutFile,sizeof(szOutBuffer));

	/* elimina l'eventuale pathname e confronta i nomi */
	if(StripPath(szInBuffer,iInLen)==StripPath(szOutBuffer,iOutLen))
		iStrcmp=(UINT)(lstrcmp(strupr(szInBuffer),strupr(szOutBuffer))==0 ? 0 : 1);
	else
		iStrcmp=1;
	
	return(iStrcmp);
}

/*
	TmpName()
	
	Ritorna un nome di file unico.
	
	input:
	void

	output:
	LPSTR	puntatore al nome del file
*/
LPSTR CGzw::TmpName(void)
{
	time_t		tTime;					/* struttura per orario */
	struct tm		*lpLcTime;                  
	char			szTime[16];				/* buffer per nome file temporaneo */
	static int	iCounter=0;				/* contatore */
	static char 	szTmpName[_MAX_PATH+1];	/* buffer per pathname + nome file temporaneo */
	LPSTR		lpPtr;					/* puntatore */
	    
	/* ricava la directory per i temporanei */
	if((lpPtr=(char*)getenv("TMP"))==NULL)
	{
		if((lpPtr=(char*)getenv("TEMP"))==NULL)
			lpPtr="\\";
	}
	
	/* controlla l'esistenza della directory */
	if(access(lpPtr,06)!=0)
		lpPtr=".\\";

	/* costruisce il pathname per il temporaneo */
	strcpyn(szTmpName,lpPtr,sizeof(szTmpName));
	if(szTmpName[strlen(szTmpName)-1]!='\\')
		lstrcat(szTmpName,"\\");

	/* ricava l'ora per la costruzione del nome file */
	tTime=time((time_t *)NULL);
	lpLcTime=localtime(&tTime);
	
	if((lpLcTime->tm_sec+=(++iCounter)) > 9999)
		lpLcTime->tm_sec=iCounter=0;
	
	_snprintf(szTime,sizeof(szTime)-1,"~G%02d%04d.TMP",lpLcTime->tm_min,lpLcTime->tm_sec);
	
	/* aggiunge al pathname il nome file */
	lstrcat(szTmpName,szTime);
	
	return(szTmpName);
}
