/*
	WallPaperUni.cpp
	Eseguibile per la disinstallazione di WallPaper (SDK).
	Usato durante la disinstallazione per eliminare la directory base del programma.
	Luca Piergentili, 29/05/01
	lpiergentili@yahoo.com
	http://www.geocities.com/crawlpaper/

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
#include <stdio.h>
#include <stdlib.h>
#include "strings.h"
#define STRICT 1
#include <windows.h>
#include <windowsx.h>
#include "win32api.h"
#include "WallPaperVersion.h"
#include "WallPaperUniVersion.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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
	WinMain()
*/
int WINAPI WinMain(HINSTANCE /*hInstance*/,HINSTANCE /*hPrevInstance*/,LPSTR lpszCmdLine,int /*nCmdShow*/)
{
	// si assicura che venga passata la directory d'installazione come parametro
	if(!lpszCmdLine || !*lpszCmdLine)
		return(0);

	char szCmd[_MAX_PATH+1];
	strcpyn(szCmd,lpszCmdLine,sizeof(szCmd));
	::RemoveBackslash(szCmd);

	char szBuffer[512];
	_snprintf(szBuffer,
			sizeof(szBuffer)-1,
			"%s has been unregistered from your system. To complete the uninstall process the %s directory must be removed.\n\nAre you sure you want to delete %s and all its content ?",
			WALLPAPER_PROGRAM_NAME,
			WALLPAPER_PROGRAM_NAME,
			szCmd);

	// elimina la directory base del programma
	if(::MessageBox((HWND)NULL,szBuffer,WALLPAPERUNI_PROGRAM_TITLE,MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST|MB_ICONQUESTION|MB_YESNO)==IDYES)
	{
		::DeleteFileToRecycleBin(NULL,szCmd,FALSE);
		_snprintf(szBuffer,sizeof(szBuffer)-1,"Uninstall complete, thank you for using %s.",WALLPAPER_PROGRAM_NAME);
	}
	else
	{
		_snprintf(szBuffer,sizeof(szBuffer)-1,"%s has been uninstalled from your system, but some elements (%s) must be removed manually.\nThank you for using %s.",WALLPAPER_PROGRAM_NAME,szCmd,WALLPAPER_PROGRAM_NAME);
	}
	
	::MessageBox((HWND)NULL,szBuffer,WALLPAPERUNI_PROGRAM_TITLE,MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST|MB_OK|MB_ICONINFORMATION);

	return(0);
}
