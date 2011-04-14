/*
	CImageParams.cpp
	Classe base per i parametri per la manipolazione dell'immagine.
	Luca Piergentili, 10/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <string.h>
#include "window.h"
#include "CImageParams.h"

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

void CImageParams::Reset(void)
{
	memset(&m_nBlur,'\0',sizeof(m_nBlur));
	memset(&m_nBrightness,'\0',sizeof(m_nBrightness));
	memset(&m_nContrast,'\0',sizeof(m_nContrast));
	memset(&m_nDilate,'\0',sizeof(m_nDilate));
	memset(&m_nEmboss,'\0',sizeof(m_nEmboss));
	memset(&m_nEmbossDirection,'\0',sizeof(m_nEmbossDirection));
	memset(&m_nErosion,'\0',sizeof(m_nErosion));
	memset(&m_nGamma,'\0',sizeof(m_nGamma));
	memset(&m_nHalftoneAngle,'\0',sizeof(m_nHalftoneAngle));
	memset(&m_nHalftoneType,'\0',sizeof(m_nHalftoneType));
	memset(&m_nHeight,'\0',sizeof(m_nHeight));
	memset(&m_nHistoContrast,'\0',sizeof(m_nHistoContrast));
	memset(&m_nHue,'\0',sizeof(m_nHue));
	memset(&m_nIntensityDetectMax,'\0',sizeof(m_nIntensityDetectMax));
	memset(&m_nIntensityDetectMin,'\0',sizeof(m_nIntensityDetectMin));
	memset(&m_nInterpolation,'\0',sizeof(m_nInterpolation));
	memset(&m_nMedian,'\0',sizeof(m_nMedian));
	memset(&m_nMosaic,'\0',sizeof(m_nMosaic));
	memset(&m_nNoiseChannel,'\0',sizeof(m_nNoiseChannel));
	memset(&m_nNoiseRange,'\0',sizeof(m_nNoiseRange));
	memset(&m_nNoiseType,'\0',sizeof(m_nNoiseType));
	memset(&m_nPosterize,'\0',sizeof(m_nPosterize));
	memset(&m_nRadius,'\0',sizeof(m_nRadius));
	memset(&m_nSaturation,'\0',sizeof(m_nSaturation));
	memset(&m_nSharpen,'\0',sizeof(m_nSharpen));
	memset(&m_nSizeFilter,'\0',sizeof(m_nSizeFilter));
	memset(&m_nSizeQualityControl,'\0',sizeof(m_nSizeQualityControl));
	memset(&m_nWidth,'\0',sizeof(m_nWidth));
}
