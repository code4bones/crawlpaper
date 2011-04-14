/*
	CFileNameOpenDialog.cpp
	Classe base per il dialogo File|Apri per i formati diversi dalle immagini (MFC).
	Luca Piergentili, 13/06/03
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "strings.h"
#include "window.h"
#include "CAudioPlayer.h"
#include "CAudioInfo.h"
#include "CImageDialog.h"
#include "CFileNameOpenDialog.h"

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
	OnUnknowFileType()
*/
void CFileNameOpenDialog::OnUnknowFileType(LPCSTR lpcszFileName,CDibCtrl& pDibCtrl,LPCSTR lpcszLibraryName,LPSTR lpszInfo,int nInfoSize)
{
	BOOL bIsRecognizedFileType = FALSE;
	
	if(CAudioPlayer::IsSupportedFormat(lpcszFileName))
	{
		CAudioInfo audioInfo(lpcszFileName);
		long lMinutes = 0L;
		long lSeconds = 0L;
		audioInfo.GetLength(lMinutes,lSeconds);

		if(striright(lpcszFileName,MP3_EXTENSION)==0)
		{
			_snprintf(lpszInfo,
					nInfoSize-1
					,
					"Title: %s\r\n"
					"Artist: %s\r\n"
					"Album: %s\r\n"
					"Genre: %s\r\n"
					"Year: %d\r\n"
					"Track: %d\r\n"
					"Bitrate: %d kbps\r\n"
					"Frequency: %ld khz\r\n"
					"Duration: %02d:%02d\r\n"
					"Size: %s\r\n"
					"MPEG Layer: %s\r\n"
					"MPEG Version: %s\r\n"
					"Channel Mode: %s\r\n"
					,
					audioInfo.GetTitle(),
					audioInfo.GetArtist(),
					audioInfo.GetAlbum(),
					audioInfo.GetGenre(),
					audioInfo.GetYear(),
					audioInfo.GetTrack(),
					audioInfo.GetBitRate(),
					audioInfo.GetFrequency(),
					lMinutes,lSeconds,
					strsize((double)audioInfo.GetFileSize()),
					audioInfo.GetLayer(),
					audioInfo.GetVersion(),
					audioInfo.GetChannelMode()
					);
			
			bIsRecognizedFileType = TRUE;
		}
		else if(striright(lpcszFileName,WAV_EXTENSION)==0)
		{
			_snprintf(lpszInfo,
					nInfoSize-1
					,
					"Title: %s\r\n"
					"Bitrate: %d bit\r\n"
					"Frequency: %ld khz\r\n"
					"Duration: %02d:%02d\r\n"
					"Size: %s\r\n"
					"Version: %s\r\n"
					"Channel Mode: %s\r\n"
					,
					audioInfo.GetTitle(),
					audioInfo.GetBitRate(),
					audioInfo.GetFrequency(),
					lMinutes,lSeconds,
					strsize((double)audioInfo.GetFileSize()),
					audioInfo.GetVersion(),
					audioInfo.GetChannelMode()
					);
			
			bIsRecognizedFileType = TRUE;
		}
		else if(striright(lpcszFileName,CDA_EXTENSION)==0)
		{
			_snprintf(lpszInfo,
					nInfoSize-1
					,
					"CD Audio Track (%s)\r\n"
					"Duration: %02d:%02d\r\n"
					,
					audioInfo.GetTitle(),
					lMinutes,lSeconds
					);
			
			bIsRecognizedFileType = TRUE;
		}
	}

	if(bIsRecognizedFileType)
	{
		pDibCtrl.SetTransparent(TRUE,RGB(255,0,255));
		pDibCtrl.Load(m_szBitmapFileName,lpcszLibraryName);
	}
	else
		pDibCtrl.Clear();
}
