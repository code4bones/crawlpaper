/*
	CBase64.cpp
	Classe per la codifica/decodifica in base64 .
	Luca Piergentili, 14/09/96
	lpiergentili@yahoo.com

	I due metodi per la codifica/decodifica del buffer sono stati ripresi dal
	codice di "Pavuk" di Stefan Ondrejicka (http://www.idata.sk/~ondrej/pavuk/).
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#ifdef _WINDOWS
  #include "window.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CBase64.h"

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
	
static const char base64table[] = {
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
};

/*
	Encode()
*/
char* CBase64::Encode(const char* data)
{
	int len = strlen(data);
	unsigned char* outstr = (unsigned char*)new char[((len/3 + 1) * 4 + 1)];
	unsigned char* instr = (unsigned char*)new char[(len+3)];
	int idx = 0,outidx = 0;

	memset(instr,'\0',len+3);
	memcpy(instr,data,len);
	memset(outstr,'\0',(len/3 + 1) * 4 + 1);

	while(idx < len)
	{
		if((idx % 3)==0)
		{
			outstr[outidx] = base64table[(int)(instr[idx] >> 2)];
			outidx ++;
		}
		else if((idx % 3)==1)
		{
			outstr[outidx] = base64table[(int)(((instr[idx - 1] << 4) & 0x30) | ((instr[idx] >> 4) & 0x0f))];
			outidx ++;
			
			outstr[outidx] = base64table[(int)(((instr[idx] << 2) & 0x3c) | ((instr[idx + 1] >> 6) & 0x03))];
			outidx ++;
		}
		else
		{
			outstr[outidx] = base64table[(int)(instr[idx] & 0x3f)];
			outidx ++;
		}

		idx ++;
	}
	if((idx % 3)==1)
	{
		outstr[outidx] = base64table[(int)(((instr[idx - 1] << 4) & 0x30) | ((instr[idx] >> 4) & 0x0f))];
		outstr[outidx+1] = '=';
		outstr[outidx+2] = '=';
	}
	else if((idx % 3)==2)
	{
		outstr[outidx] = '=';
	}
	
	delete [] instr;

	return((char*)outstr);
}

/*
	Decode()
*/
int CBase64::Decode(const char* inbuf,char** outbuf)
{
	unsigned char* p = (unsigned char*)inbuf;
	char* rv = new char[(((strlen(inbuf) + 1) * 3) / 4 + 1)];
	int len = 0;
	int stop = 0, err = 0;

	while(*p)
	{
		int n = 0;
		int c[4] = {0,0,0,0};
		int nt[5] = {0,1,1,2,3};
		unsigned char triple[3] = {'\0','\0','\0'};

		for(n = 0; n < 4; n++)
		{
			if(!*(p+n) || *(p+n)=='=')
			{
				stop = 1;
				break;
			}
			else
				c[n] = ChrIndex(base64table,*(p+n));

			if(c[n] < 0)
			{
				err = 1;
				stop = 1;
				break;
			}
		}

		triple[0] = (c[0] << 2) | (c[1] >> 4);
		triple[1] = (c[1] & 0x3f)  << 4 | ((c[2] & 0x3c) >> 2);
		triple[2] = (c[2] & 0x3) << 6 | (c[3] & 0x3f);

		memcpy(rv + len,triple,nt[n]);

		if(stop)
		{
			len += nt[n];
			break;
		}
		else
			len += 3;

		p += 4;
	}

	if(err)
	{
		delete [] rv;
		len = -1;
	}
	else
	{
		rv[len] = '\0';
		*outbuf = rv;
	}

	return(len);
}

/*
	Encode()

	Codifica in BASE64 il file di input nel file di output.
	0 per conversione effettuata
	-1 se non puo' aprire il file di input
	-2 se non puo' aprire il file di output
*/
int CBase64::Encode(const char *pPlainFile,const char *pEncodedFile)
{
	char szBuffer[128];
	FILE* pInputFile;
	FILE* pOutputFile;
	unsigned char cChr = 0;
	unsigned long nShifted = 0L;
	unsigned long lValue = 0L;
	int nIndex = 0;
	int nShift = 0;
	int nSaveShift = 0;
	int bDone = 0;
     
	if((pInputFile = fopen(pPlainFile,"rb"))==(FILE*)NULL)
		return(-1);
	
	if((pOutputFile = fopen(pEncodedFile,"w"))==(FILE*)NULL)
	{
		fclose(pInputFile);
		return(-2);
	}

	do
	{
		bDone = 0;
		nShift = 0;
		nShifted = 0L;
		nIndex = 0;

		while(!feof(pInputFile) || nShift!=0)
		{
			if(!feof(pInputFile) && !bDone)
			{
				cChr = (char)fgetc(pInputFile);

				if(feof(pInputFile))
				{
					bDone = 1;
					nSaveShift = nShift;
					cChr = 0;
				}
			}
			else
			{
				bDone = 1;
				nSaveShift = nShift;
				cChr = 0;
			}

			if(!bDone || nShift!=0)
			{
				lValue = (unsigned long)cChr;
				nShifted <<= 8;
				nShift += 8;
				nShifted |= lValue;
			}

			while(nShift >= 6)
			{
				nShift -= 6;
				lValue = (nShifted >> nShift) & 0x3fL;
				cChr = (unsigned char)base64table[(int)lValue];

				szBuffer[nIndex++] = cChr;
				
				if(nIndex >= 60)
				{
					szBuffer[nIndex] = '\0';
					fprintf(pOutputFile,"%s\n",szBuffer);
					nIndex = 0;
				}

				if(bDone)
					nShift = 0;
			}
		}

		if(nSaveShift==2)
		{
			szBuffer[nIndex++] = '=';
			
			if(nIndex >= 60)
			{
				szBuffer[nIndex] = '\0';
				fprintf(pOutputFile,"%s\n",szBuffer);
				nIndex = 0;
			}

			szBuffer[nIndex++] = '=';
			
			if(nIndex >= 60)
			{
				szBuffer[nIndex] = '\0';
				fprintf(pOutputFile,"%s\n",szBuffer);
				nIndex = 0;
			}
		}
		else if(nSaveShift==4)
		{
			szBuffer[nIndex++] = '=';
			
			if(nIndex >= 60)
			{
				szBuffer[nIndex] = '\0';
				fprintf(pOutputFile,"%s\n",szBuffer);
				nIndex = 0;
			}
		}

		if(nIndex!=0)
		{
			szBuffer[nIndex] = '\0';
			fprintf(pOutputFile,"%s\n",szBuffer);
		}
	}
	while(!feof(pInputFile));

	fclose(pInputFile);
	fclose(pOutputFile);

	return(0);
}

/*
	Decode()

	Decodifica il file di input nel file di output.
	0 per conversione effettuata
	-1 se non puo' aprire il file di input
	-2 se non puo' aprire il file di output
*/
int CBase64::Decode(const char *pEncodedFile,const char *pPlainFile)
{
	char szBuffer[128];
	FILE* pInputFile;
	FILE* pOutputFile;
	unsigned char cChr = 0;
	unsigned long nShifted = 0L;
	unsigned long lValue = 0L;
	int nIndex = 0;
	int nShift = 0;
	int bDone = 0;
     
	if((pInputFile = fopen(pEncodedFile,"r"))==(FILE*)NULL)
		return(-1);

	if((pOutputFile = fopen(pPlainFile,"wb"))==(FILE*)NULL)
	{
		fclose(pInputFile);
		return(-2);
	}
     
	do
	{
		bDone = 0;
		nShift = 0;
		nShifted = 0L;
		
		while(!feof(pInputFile) && !bDone)
		{
			if(fgets(szBuffer,sizeof(szBuffer),pInputFile))
			{
				for(nIndex = 0; szBuffer[nIndex] && szBuffer[nIndex]!='\n'; nIndex++)
				{
					lValue = ConvertToAscii(szBuffer[nIndex]);
	
					if(lValue < 64)
					{
						nShifted <<= 6;
						nShift += 6;
						nShifted |= lValue;
						
						if(nShift >= 8)
						{
							nShift -= 8;
							lValue = nShifted >> nShift;
							cChr = (unsigned char)(lValue & 0xffL);
							fputc(cChr,pOutputFile);
						}
					}
					else
					{
						bDone = 1;
						break;
					}
				}
			}
		}
	}
	while(!feof(pInputFile));

	fclose(pInputFile);
	fclose(pOutputFile);

	return(0);
}

/*
	ConvertToAscii()
*/
int CBase64::ConvertToAscii(unsigned char c)
{
	if((c >= 'A') && (c <= 'Z'))
		return((int)(c - 'A'));
	else if((c >= 'a') && (c <= 'z'))
		return(26 + (int)(c - 'a'));
	else if((c >= '0') && (c <= '9'))
		return(52 + (int)(c - '0'));
	else if(c=='+')
		return(62);
	else if(c=='/')
		return(63);
	else if(c=='=')
		return(-2);
	else
		return(-1);
}

/*
	ChrIndex()
*/
int CBase64::ChrIndex(const char* pString,int c)
{
	char* p = (char*)strchr(pString,c);
	if(!p)
		return(-1);
	else
		return(p - pString);
}
