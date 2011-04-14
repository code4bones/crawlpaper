/*
	CAudioCDA.cpp
	Classe (derivata) per l'interfaccia audio verso i files di tipo .mp3
	Come decoder viene utilizzata l'interfaccia MCI, acceduta tramite le classi presenti in mciaudio.
	Luca Piergentili, 10/06/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "mmaudio.h"
#include "mciaudio.h"
#include "CAudioCDA.h"

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
	CAudioCDA()
*/
CAudioCDA::CAudioCDA(HWND hWnd/*=NULL*/)
{
	m_hWnd = hWnd;
	m_pMCIAudioCda = new CMCIAudioCDA();
}

/*
	~CAudioCDA()
*/
CAudioCDA::~CAudioCDA()
{
	if(m_pMCIAudioCda)
	{
		if(m_pMCIAudioCda->GetStatus()==mmAudioPmPlaying)
			m_pMCIAudioCda->Stop();
		if(m_pMCIAudioCda->GetStatus()!=mmAudioPmClosed)
			m_pMCIAudioCda->Close();
		delete m_pMCIAudioCda,m_pMCIAudioCda = NULL;
	}
}

/*
	GetLength()
*/
QWORD CAudioCDA::GetLength(long& lMinutes,long& lSeconds)
{
	CDATRACKINFO cdaTrackInfo = {0};
	
	if(m_pMCIAudioCda)
		m_pMCIAudioCda->GetTrackInfo(&cdaTrackInfo);
	
	lMinutes = cdaTrackInfo.lMinutes;
	lSeconds = cdaTrackInfo.lSeconds;
	
	return(cdaTrackInfo.qwTrackSize);
}
