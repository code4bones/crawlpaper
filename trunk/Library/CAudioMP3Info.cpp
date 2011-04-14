/*
	CAudioMP3Info.cpp
	Classe per info/tags sui files .mp3.
	Luca Piergentili, 05/08/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <string.h>
#include "window.h"
#include "mmaudio.h"
#include "mciaudio.h"
#include "CId3Lib.h"
#include "CMP3Info.h"
#include "CAudioMP3Info.h"

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
	CAudioMp3Info()
*/
CAudioMp3Info::CAudioMp3Info(LPCSTR lpcszMp3FileName/* = NULL */)
{
	if(lpcszMp3FileName)
		Link(lpcszMp3FileName);
}

/*
	Link()
*/
int CAudioMp3Info::Link(LPCSTR lpcszMp3FileName)
{
	int nRet = 0;

	// diciamoci la verita', la id3lib suca un poco la minchia e sono piu' quelli che sfarfalla
	// che quelli che azzecca, per cui, mentre il wrapper relativo carica i tag v1 come valori
	// di ripiego, il codice qui sotto si occupa di ricavare i valori relativi all'mp3 in proprio
	Unlink();

	// id3lib (v2 + v1 come ripiego)
	m_Id3Lib.Link(lpcszMp3FileName);

	// analisi diretta dell'header mp3
	if(m_MP3Info.Load(lpcszMp3FileName))
	{
		strcpyn(m_szFileName,lpcszMp3FileName,sizeof(m_szFileName));
		m_qwFileSize	 = m_MP3Info.GetFileSize();
		m_lLength		 = m_MP3Info.GetLength();
		m_nBitRate	 = m_MP3Info.GetBitRate();
		m_lSampleRate	 = m_MP3Info.GetFrequency();
		m_nMPEGLayer	 = m_MP3Info.GetLayerNumber();
		m_dlMPEGVersion = m_MP3Info.GetVersion();
		m_pChannelMode	 = (char*)m_MP3Info.GetChannelMode();
		nRet = 1;
	}

	return(nRet);
}

/*
	Unlink()
*/
void CAudioMp3Info::Unlink(void)
{
	m_qwFileSize = 0L;
	memset(m_szFileName,'\0',sizeof(m_szFileName));

	m_lLength = 0L;
	m_nBitRate = 0;
	m_lSampleRate = 0L;
	m_dlMPEGVersion = 0.0f;
	m_nMPEGLayer = 0;
	m_pChannelMode = NULL;
	m_bCopyrighted = FALSE;
	m_bOriginal = FALSE;

	m_Id3Lib.Unlink();
}

/*
	GetLength()
*/
QWORD CAudioMp3Info::GetLength(long& lMinutes,long& lSeconds)
{
	lMinutes = lSeconds = 0L;
	
	if(m_lLength > 0L)
	{
		lMinutes = m_lLength / 60L;
		lSeconds = m_lLength % 60L;
	}

	return(m_qwFileSize);
}

/*
	GetLayer()
*/
LPCSTR CAudioMp3Info::GetLayer(void)
{
	char* pLayer = "?";
	switch(m_nMPEGLayer)
	{
		case 1:
			pLayer = "I";
			break;
		case 2:
			pLayer = "II";
			break;
		case 3:
			pLayer = "III";
			break;
	}
	return(pLayer);
}

/*
	GetVersion()
*/
LPCSTR CAudioMp3Info::GetVersion(void)
{
	char* pVersion = "?";
	if(m_dlMPEGVersion==2.5f)
		pVersion = "2.5";
	else if(m_dlMPEGVersion==2.0f)
		pVersion = "2";
	else if(m_dlMPEGVersion==1.0f)
		pVersion = "1";
	return(pVersion);
}
