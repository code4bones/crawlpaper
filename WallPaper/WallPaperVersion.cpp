/*
	WallPaperVersion.cpp
	Luca Piergentili, 06/08/98
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
#define _WALLPAPERVERSION_CPP 1
#include "WallPaperVersion.h"
#undef _WALLPAPERVERSION_CPP

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

const char* g_lpcszCredits = {
	"\r\n"WALLPAPER_PROGRAM_TITLE_ALIAS"\r\n"WALLPAPER_WEB_SITE"\r\n"WALLPAPER_AUTHOR_STATIC_COPYRIGHT"\r\n"
	"crawlpaper is a registered name, all rights reserved\r\n\r\n"
	"version: "WALLPAPER_VERSION"\r\nrelease date: "WALLPAPER_RELEASE_DATE"\r\ninternal build: "WALLPAPER_BUILD_DATE"\r\n\r\n"
	"This is a free software, released under the terms of the BSD license (see the license.txt file). "
	"Do not attempt to use it in any form which violates the license or you will be persecuted and charged for this.\r\n"
	"\r\n"WALLPAPER_PROGRAM_NAME" uses the following:\r\n\r\n"
	"- libjpeg (v.6b), copyright (c) 1991-1998 Thomas G. Lane/Independent JPEG Group's software (http://www.ijg.org/)\r\n\r\n"
	"- libtiff (v.3.4), copyright (c) 1988-1997 Sam Leffler/1991-1997 Silicon Graphics Inc. (http://www.libtiff.org/)\r\n\r\n"
	"- NexgenIPL (v.2.9.6), copyright (c) 1999-2003 Binary Technologies (http://www.binary-technologies.com/)\r\n\r\n"
	"- Berkeley DB Database (v.2.7.7), (http://en.wikipedia.org/wiki/Berkeley_DB)\r\n\r\n"
	"- unrar (v.4), copyright (c) Eugene Roshal (http://www.rarlabs.com/)\r\n\r\n"
	"- unzip550 (v.5.5), copyright (c) 1990-2003 Info-ZIP (ftp://ftp.info-zip.org/pub/infozip/)\r\n\r\n"
	"- zLib (v.1.1.3) copyright (c) 1995-2003 Jean-loup Gailly and Mark Adler (http://www.gzip.org/zlib/)\r\n\r\n"
	"- streamripper (v.1.x) by Jon Clegg (http://streamripper.sourceforge.net/)\r\n\r\n"
	"- libmad MPEG audio decoder library (v.0.15.0), copyright (c) 2000, 2003 Robert Leslie/Underbit Technologies Inc. (http://www.underbit.com/products/mad/)\r\n\r\n"
	"- code snippets from MSDN samples, codeproject.com and codeguru.com (look at the source code for detailed credits)\r\n\r\n"
};
