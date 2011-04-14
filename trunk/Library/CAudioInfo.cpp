/*
	CAudioInfo.cpp
	Classe (semi-factory) per le info sull'audio.
	Ricava le info sui files audio supportati tramite gli oggetti derivati dalla classe CAudioInfoObject.
	Luca Piergentili, 14/07/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include <mmsystem.h>
#include "mmaudio.h"
#include "CAudioInfoObject.h"
#include "CAudioWavInfo.h"
#include "CAudioCDAInfo.h"
#include "CAudioMP3Info.h"
#include "CAudioInfo.h"

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
	CAudioInfo()
*/
CAudioInfo::CAudioInfo(LPCSTR lpcszAudioFileName/* = NULL*/)
{
	m_pAudioInfoObject = NULL;
	m_pAudioInfoWav = NULL;
	m_pAudioInfoCDA = NULL;
	m_pAudioInfoMPeg = NULL;
	
	if(lpcszAudioFileName)
		Link(lpcszAudioFileName);
}

/*
	~CAudioInfo()
*/
CAudioInfo::~CAudioInfo()
{
	if(m_pAudioInfoWav)
		delete m_pAudioInfoWav,m_pAudioInfoWav = NULL;
	if(m_pAudioInfoCDA)
		delete m_pAudioInfoCDA,m_pAudioInfoCDA = NULL;
	if(m_pAudioInfoMPeg)
		delete m_pAudioInfoMPeg,m_pAudioInfoMPeg = NULL;
	m_pAudioInfoObject = NULL;
}

/*
	Link()
*/
int CAudioInfo::Link(LPCSTR lpcszAudioFileName)
{
	if(striright(lpcszAudioFileName,WAV_EXTENSION)==0)
		m_pAudioInfoObject = m_pAudioInfoWav ? m_pAudioInfoWav : (m_pAudioInfoWav = new CAudioWavInfo());
	else if(striright(lpcszAudioFileName,CDA_EXTENSION)==0)
		m_pAudioInfoObject = m_pAudioInfoCDA ? m_pAudioInfoCDA : (m_pAudioInfoCDA = new CAudioCDAInfo());
	else if(striright(lpcszAudioFileName,MP3_EXTENSION)==0)
		m_pAudioInfoObject = m_pAudioInfoMPeg ? m_pAudioInfoMPeg : (m_pAudioInfoMPeg = new CAudioMp3Info());
	else
		m_pAudioInfoObject = NULL;

	return(m_pAudioInfoObject ? m_pAudioInfoObject->Link(lpcszAudioFileName) : 0);
}
