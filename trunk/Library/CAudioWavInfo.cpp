/*
	CAudioWavInfo.cpp
	Classe per info/tags sui files .wav.
	Luca Piergentili, 15/07/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <string.h>
#include "window.h"
#include "mmaudio.h"
#include "mciaudio.h"
#include "waveio.h"
#include "CAudioWavInfo.h"

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
	CAudioWavInfo()
*/
CAudioWavInfo::CAudioWavInfo(LPCSTR lpcszWavFileName/*=NULL*/)
{
	memset(&m_mcifi,'\0',sizeof(MCIFILEINFO));
	m_pMCIAudioWave = new CMCIAudioWave(&m_mcifi);
	if(m_pMCIAudioWave)
		if(lpcszWavFileName)
			Link(lpcszWavFileName);
}

/*
	~CAudioWavInfo()
*/
CAudioWavInfo::~CAudioWavInfo()
{
	if(m_pMCIAudioWave)
		delete m_pMCIAudioWave,m_pMCIAudioWave = NULL;
}

/*
	Unlink()
*/
void CAudioWavInfo::Unlink(void)
{
	if(m_pMCIAudioWave)
	{
		if(m_mcifi.pwfx)
		{
			GlobalFreePtr(m_mcifi.pwfx);
			m_mcifi.pwfx = NULL;
		}
		memset(&m_mcifi,'\0',sizeof(MCIFILEINFO));
	}
}

/*
	GetLength()
*/
QWORD CAudioWavInfo::GetLength(long& lMinutes,long& lSeconds)
{
	QWORD qwSize = 0L;
	lMinutes = lSeconds = 0L;
	
	if(m_pMCIAudioWave)
	{
		lMinutes = m_mcifi.lMinutes;
		lSeconds = m_mcifi.lSeconds;
		qwSize   = m_mcifi.qwFileSize;
	}
	
	return(qwSize);
}
