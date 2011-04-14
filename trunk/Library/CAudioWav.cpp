/*
	CAudioWav.cpp
	Classe (derivata) per l'interfaccia audio verso i files di tipo .wav
	Come decoder viene utilizzata l'interfaccia MCI, acceduta tramite le classi presenti in mciaudio.
	Luca Piergentili, 13/07/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "mmaudio.h"
#include "mciaudio.h"
#include "CAudioWav.h"

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
	CAudioWav()
*/
CAudioWav::CAudioWav(HWND hWnd/*=NULL*/)
{
	m_hWnd = hWnd;
	memset(&m_mcifi,'\0',sizeof(MCIFILEINFO));
	m_mcifi.mmaPlayerStatus = mmAudioPmClosed;
	m_pMCIAudioWave = new CMCIAudioWave(&m_mcifi);
}

/*
	~CAudioWav()
*/
CAudioWav::~CAudioWav()
{
	if(m_pMCIAudioWave)
	{
		if(m_mcifi.mmaPlayerStatus==mmAudioPmPlaying)
			m_pMCIAudioWave->Stop();
		if(m_mcifi.mmaPlayerStatus!=mmAudioPmClosed)
			m_pMCIAudioWave->Close();
		delete m_pMCIAudioWave,m_pMCIAudioWave = NULL;
	}
}

/*
	GetLength()
*/
QWORD CAudioWav::GetLength(long& lMinutes,long& lSeconds)
{
	lMinutes = m_pMCIAudioWave ? m_mcifi.lMinutes : 0L;
	lSeconds = m_pMCIAudioWave ? m_mcifi.lSeconds : 0L;
	
	return(m_pMCIAudioWave ? (long)m_mcifi.qwFileSize : 0L);
}
