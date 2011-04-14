/*
	WallPaperIconIndex.cpp
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
#include "window.h"
#include "CListCtrlEx.h"
#include "CRegistry.h"
#include "CUrl.h"
#include "WallPaperIconIndex.h"

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
	GetIconIndex()

	Restituisce l'indice dell'icona relativa al tipo file.
*/
int GetIconIndex(LPCSTR lpcszFileName,CIconIndexList* plistIconIndex,CListCtrlEx* pListCtrlEx)
{
	int nIndex = 0;
	char szExt[_MAX_PATH+1];

	strcpyn(szExt,lpcszFileName,sizeof(szExt));
	strrev(szExt);

	char* p = strchr(szExt,'.');

	if(p)
	{
		*(p+1) = '\0';
		strrev(szExt);

		ITERATOR iter;
		ICONINDEX* iconindex;

		// cerca tra quanto gia' presente nella lista		
		if((iter = plistIconIndex->First())!=(ITERATOR)NULL)
		{
			do
			{
				iconindex = (ICONINDEX*)iter->data;
				if(iconindex)
				{
					if(stricmp(iconindex->ext,szExt)==0)
						return(iconindex->index);
				}

				iter = plistIconIndex->Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}

		// non trovato, inserisce un nuovo elemento
		CUrl url;
		CRegistry registry;
		iconindex = (ICONINDEX*)plistIconIndex->Add();
		if(iconindex)
		{
			if(url.IsUrl(lpcszFileName))
				strcpyn(szExt,".htm",_MAX_EXT+1);
			strcpyn(iconindex->ext,szExt,_MAX_EXT+1);
			nIndex = iconindex->index = pListCtrlEx->AddIcon(registry.GetSafeIconForRegisteredFileType(szExt));
		}
	}

	return(nIndex);
}
