/*
	WallPaperDoNotAskMore.cpp
	Luca Piergentili, 13/03/04
	lpiergentili@yahoo.com

	WallPaper (alias crawlpaper) - the hardcore of Windows desktop
	http://www.crawlpaper.com/
	copyright © 1998-2004 Luca Piergentili, all rights reserved
	crawlpaper is a registered name, all rights reserved

	This is a free software, released under the terms of the BSD license. Do not
	attempt to use it in any form which violates the license or you will be persecuted
	and charged for this.

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer. 

	- Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or other
	materials provided with the distribution. 

	- Neither the name of "crawlpaper" nor the names of its contributors may be used
	to endorse or promote products derived from this software without specific prior
	written permission. 

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
	OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "win32api.h"
#include "MessageBoxExt.h"
#include "WallPaperConfig.h"
#include "WallPaperDoNotAskMore.h"

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
	DoNotAskMoreMessageBox()
*/
int DoNotAskMoreMessageBox(HWND hWnd,UINT nResourceID,int nTimeout,CWallPaperConfig* pConfig,LPCSTR lpcszDoNotAskMoreKey,LPCSTR lpcszDoNotAskMoreValue,int nForcedValue/* = -1*/,UINT nStyle/* = MB_YESNO*/)
{
	return(DoNotAskMoreMessageBox(hWnd,MAKEINTRESOURCE(nResourceID),nTimeout,pConfig,lpcszDoNotAskMoreKey,lpcszDoNotAskMoreValue,nForcedValue,nStyle));
}

/*
	DoNotAskMoreMessageBox()
*/
int DoNotAskMoreMessageBox(HWND hWnd,LPCSTR lpcszText,int nTimeout,CWallPaperConfig* pConfig,LPCSTR lpcszDoNotAskMoreKey,LPCSTR lpcszDoNotAskMoreValue,int nForcedValue/* = -1*/,UINT nStyle/* = MB_YESNO*/)
{
	int nRet = IDNO;
	
	// il valore relativo alla chiave (utilizzato sotto per impostare il bottone di default) deve essere 1 (=yes) o 0 (=no)
	// se non viene forzato viene caricato dalla configurazione
	int nDefaultValue = (nForcedValue!=-1) ? nForcedValue : pConfig->GetNumber(WALLPAPER_DONOTASKMORE_KEY,lpcszDoNotAskMoreValue);

	if(pConfig->GetNumber(WALLPAPER_DONOTASKMORE_KEY,lpcszDoNotAskMoreKey))
		nRet = pConfig->GetNumber(WALLPAPER_DONOTASKMORE_KEY,lpcszDoNotAskMoreValue) ? IDYES : IDNO;
	else
	{
		if(nStyle==MB_YESNO)
			nStyle = MB_DONOTASKAGAIN|MB_YESNO|(nDefaultValue==1 ? MB_DEFBUTTON1 : MB_DEFBUTTON2)|MB_ICONQUESTION|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST;
		else if(nStyle==MB_ICONINFORMATION)
			nStyle = MB_DONOTSHOWAGAIN|MB_OK|MB_ICONINFORMATION|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST;
		XMSGBOXPARAMS xmb;
		if(nTimeout > 0)
			xmb.nTimeoutSeconds = nTimeout;
		
		// bug: la MessageBoxExt(), se chiamata da una finestra minimizzata, non mette in primo piano il dialogo, inchiodando il chiamante
		if(hWnd)
			::SetForegroundWindowEx(hWnd);
		nRet = ::MessageBoxExt(hWnd,lpcszText,WALLPAPER_PROGRAM_NAME,nStyle,&xmb);

		if(nRet & MB_TIMEOUT)
			nRet = nRet & ~MB_TIMEOUT;

		if((nRet & MB_DONOTASKAGAIN) || (nRet & MB_DONOTSHOWAGAIN))
		{
			nRet = nRet & ~MB_DONOTASKAGAIN;
			pConfig->UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,lpcszDoNotAskMoreKey,1);
			pConfig->SaveKey(WALLPAPER_DONOTASKMORE_KEY,lpcszDoNotAskMoreKey);
		}
		
		pConfig->UpdateNumber(WALLPAPER_DONOTASKMORE_KEY,lpcszDoNotAskMoreValue,nRet==IDYES ? 1 : 0);
		pConfig->SaveKey(WALLPAPER_DONOTASKMORE_KEY,lpcszDoNotAskMoreValue);
	}

	return(nRet);
}
