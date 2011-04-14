/*
	WallPaperVersion.h
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
#ifndef _WALLPAPERVERSION_H
#define _WALLPAPERVERSION_H 1

#ifndef _WALLPAPERVERSION_CPP
	extern const char* g_lpcszCredits;
#endif

// nomi applicazione
#define WALLPAPER_INTERNAL_NAME			"CrawlPaper"
#define WALLPAPER_PROGRAM_NAME			"WallPaper"

// copyright, about, etc.
#define WALLPAPER_AUTHOR_NAME				"Luca Piergentili"
#define WALLPAPER_AUTHOR_EMAIL			"lpiergentili@yahoo.com"
#define WALLPAPER_AUTHOR_WEB				"http://sourceforge.net/users/crawlpaper/"
#define WALLPAPER_AUTHOR_COPYRIGHT			"Copyright © 1998-2004 by "WALLPAPER_AUTHOR_NAME" (%s), all rights reserved"
#define WALLPAPER_AUTHOR_STATIC_COPYRIGHT	"Copyright © 1998-2004 by "WALLPAPER_AUTHOR_NAME" ("WALLPAPER_AUTHOR_EMAIL"), all rights reserved"
#define WALLPAPER_AUTHOR_HTML_COPYRIGHT		"Copyright © 1998-2004 by <a href=\"mailto:%s\">"WALLPAPER_AUTHOR_NAME"</a>, all rights reserved"
#define WALLPAPER_REGISTERED_NAME			WALLPAPER_INTERNAL_NAME" is a registered name, all rights reserved"

// versione
// ricordarsi di mantenere aggiornato il numero di versione con la risorsa relativa
// (VS_VERSION_INFO) e con quanto presente nel file per il sito web (update.txt)
#define WALLPAPER_VERSION_TYPE			""
#define WALLPAPER_VERSION_NUMBER			"4.2.4"
#define WALLPAPER_RELEASE_DATE			"10/04/2011"
#define WALLPAPER_BUILD_DATE				__DATE__" "__TIME__
#if defined(WALLPAPER_VERSION_TYPE)
	#define WALLPAPER_VERSION			WALLPAPER_VERSION_NUMBER""WALLPAPER_VERSION_TYPE
#else
	#define WALLPAPER_VERSION			WALLPAPER_VERSION_NUMBER
#endif

// nomi interni e pubblici
#define WALLPAPER_PROGRAM_NAME_ALIAS		WALLPAPER_PROGRAM_NAME" (alias crawlpaper)"
#define WALLPAPER_PROGRAM_DESCRIPTION		"the hardcore of Windows desktop"
#define WALLPAPER_PROGRAM_TAGLINE			"" /*"In crawl we trust..."*/
#define WALLPAPER_PROGRAM_TITLE			WALLPAPER_PROGRAM_NAME" v."WALLPAPER_VERSION" - "WALLPAPER_PROGRAM_DESCRIPTION
#define WALLPAPER_PROGRAM_TITLE_ALIAS		WALLPAPER_PROGRAM_NAME_ALIAS" v."WALLPAPER_VERSION" - "WALLPAPER_PROGRAM_DESCRIPTION

// file di configurazione presenti sul sito web
#define WALLPAPER_UPDATE_FILE				"update.txt"
#define WALLPAPER_REGISTEREDSITES_FILE		"sites.txt"

// sito web, email
#define WALLPAPER_WEB_SITE				"http://sourceforge.net/projects/crawlpaper/"
#define WALLPAPER_EMAIL_ADDRESS			WALLPAPER_AUTHOR_EMAIL
#define WALLPAPER_SOURCEFORGE_MIRROR		"http://downloads.sourceforge.net/project/crawlpaper/"
//#define WALLPAPER_FILESDOTCOM_MIRROR		"http://www.files.com/file/4d55b1a354e41/"

// user agent: CrawlPaper/n.n.n (Windows xxx)
#define WALLPAPER_USER_AGENT				WALLPAPER_INTERNAL_NAME"/"WALLPAPER_VERSION" (Windows %s)"

#endif // _WALLPAPERVERSION_H
