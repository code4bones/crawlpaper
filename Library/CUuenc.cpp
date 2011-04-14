/*
	CUuenc.cpp
	Classe per la codifica/decodifica in formato UUENC (CRT).
	Luca Piergentili, 21/11/96
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#pragma warning (disable:4135) // conversione tipi
#pragma warning (disable:4244) // conversione tipi
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CUuenc.h"

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
	uuENC()
*/
static unsigned char uuENC(unsigned char outbyte)
{
	static const char UUEncodeTable[] = {
	"`!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
	};

	return(UUEncodeTable[(outbyte & 0x3f)]);
}

/*
	uuDEC()
*/
static unsigned char uuDEC(unsigned char inbyte)
{
	return((inbyte - ' ') & 0x3f);
}

/*
	Encode()

	Codifica in UUENC il file di input nel file di output.
	Restituisce 0 per conversione effettuata, -1 se non puo' aprire il file di input,
	-2 se non puo' aprire il file di output.
*/
int CUuenc::Encode(const char *lpcszInputFileName,const char *lpcszOutputFileName)
{
	int nNumRead;
	int nCurPos;
	unsigned char cCurByte;
	unsigned char szCurLine[64];
	unsigned char szEncodedLine[512];
	unsigned char* pPtrEncodedLine = szEncodedLine;
	FILE* fpDecodedFile;
	FILE* fpEncodedFile;
	
	if((fpDecodedFile = fopen(lpcszInputFileName,"rb"))==(FILE*)NULL)
		return(-1);

	if((fpEncodedFile = fopen(lpcszOutputFileName,"wb"))==(FILE*)NULL)
	{
		fclose(fpDecodedFile);
		return(-2);
	}
	
	while(!feof(fpDecodedFile))
	{
		memset(szCurLine,'\0',sizeof(szCurLine));
		memset(szEncodedLine,'\0',sizeof(szEncodedLine));
		pPtrEncodedLine = szEncodedLine;
		
		nNumRead = 0;
		
		while(nNumRead < 45 && !feof(fpDecodedFile))
		{
			cCurByte = fgetc(fpDecodedFile);
			nNumRead++;
			szCurLine[nNumRead] = cCurByte;
		}
		
		*pPtrEncodedLine++ = uuENC((char)nNumRead);
		
		nCurPos = 1;
		
		while(nCurPos <= (nNumRead-2))
		{
			cCurByte = (szCurLine[nCurPos] >> 2);
			*pPtrEncodedLine++ = uuENC(cCurByte);
			
			cCurByte = (szCurLine[nCurPos] << 4) | (szCurLine[nCurPos+1] >> 4);
			*pPtrEncodedLine++ = uuENC(cCurByte);
			
			cCurByte = (szCurLine[nCurPos+1] << 2) | (szCurLine[nCurPos+2] >> 6);
			*pPtrEncodedLine++ = uuENC(cCurByte);
			
			cCurByte = (szCurLine[nCurPos+2] & 0x3f);
			*pPtrEncodedLine++ = uuENC(cCurByte);
			
			nCurPos += 3;
		}
		
		if(nCurPos < nNumRead)
		{
			cCurByte = (szCurLine[nCurPos] >> 2);
			*pPtrEncodedLine++ = uuENC(cCurByte);
			
			if(nCurPos==(nNumRead-1))
			{
				cCurByte = ((szCurLine[nCurPos] << 4) & 0x30);
				*pPtrEncodedLine++ = uuENC(cCurByte);
			}
			else
			{
				cCurByte = (((szCurLine[nCurPos] << 4) & 0x30) | ((szCurLine[nCurPos+1] >> 4) & 0x0f));
				*pPtrEncodedLine++ = uuENC(cCurByte);
				cCurByte = ((szCurLine[nCurPos+1] << 2) & 0x3c);
			}
			
			*pPtrEncodedLine++ = uuENC(cCurByte);
		}
		
		fprintf(fpEncodedFile,"%s\r\n",szEncodedLine);
	}
	
	fclose(fpEncodedFile);
	fclose(fpDecodedFile);

	return(0);
}

/*
	Decode()

	Decodifica il file di input nel file di output.
	Restituisce 0 per conversione effettuata, -1 se non puo' aprire il file di input,
	-2 se non puo' aprire il file di output.
*/
int CUuenc::Decode(const char *lpcszInputFileName,const char *lpcszOutputFileName)
{
	int NumChars;
	int nCurPos;
	char szCurLine[64];
	char cCurChar;
	FILE* fpEncodedFile;
	FILE* fpDecodedFile;

	if((fpEncodedFile = fopen(lpcszInputFileName,"rt"))==(FILE*)NULL)
		return(-1);

	if((fpDecodedFile = fopen(lpcszOutputFileName,"wb"))==(FILE*)NULL)
	{
		fclose(fpEncodedFile);
		return(-2);
	}

	while(!feof(fpEncodedFile))
	{
		if(fgets(szCurLine,sizeof(szCurLine)-1,fpEncodedFile))
		{
			NumChars = (int)uuDEC(szCurLine[0]);

			if(NumChars < 45)
				NumChars--;

			if(NumChars > 0)
			{
				nCurPos = 1;
				
				while(NumChars > 0)
				{
					if(NumChars >= 3)
					{
						cCurChar = ((uuDEC(szCurLine[nCurPos]) << 2) | (uuDEC(szCurLine[nCurPos+1]) >> 4));
						fputc(cCurChar,fpDecodedFile);

						cCurChar = ((uuDEC(szCurLine[nCurPos+1]) << 4) | (uuDEC(szCurLine[nCurPos+2]) >> 2));
						fputc(cCurChar,fpDecodedFile);

						cCurChar = ((uuDEC(szCurLine[nCurPos+2]) << 6) | (uuDEC(szCurLine[nCurPos+3])));
						fputc(cCurChar,fpDecodedFile);
					}
					else
					{
						if(NumChars >= 1)
						{
							cCurChar = ((uuDEC(szCurLine[nCurPos]) << 2) | (uuDEC(szCurLine[nCurPos+1]) >> 4));
							fputc(cCurChar,fpDecodedFile);
						}
						if(NumChars >= 2)
						{
							cCurChar = ((uuDEC(szCurLine[nCurPos+1]) << 4) | (uuDEC(szCurLine[nCurPos+2]) >> 2));
							fputc(cCurChar,fpDecodedFile);
						}
					}

					NumChars -= 3;
					nCurPos += 4;
				}
			}
		}
	}
	
	fclose(fpEncodedFile);
	fclose(fpDecodedFile);

	return(0);
}

#pragma warning (default:4135) // conversione tipi
#pragma warning (default:4244) // conversione tipi
