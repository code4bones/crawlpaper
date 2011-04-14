/*
	WallPaperIconIndex.h
	Luca Piergentili, 06/08/98
	lpiergentili@yahoo.com

	WallPaper (alias crawlpaper) - the hardcore of Windows desktop
	http://www.crawlpaper.com/
	copyright � 1998-2004 Luca Piergentili, all rights reserved
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
#ifndef _WALLPAPERICONINDEX_H
#define _WALLPAPERICONINDEX_H 1

#include "window.h"
#include "CListCtrlEx.h"

/*
	ICONINDEX
*/
struct ICONINDEX {
	int	index;
	char	ext[_MAX_EXT+1];
};

/*
	CIconIndexList
*/
class CIconIndexList : public CNodeList
{
public:
	CIconIndexList() : CNodeList() {}
	virtual ~CIconIndexList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new ICONINDEX);
	}
	void* Initialize(void* pVoid)
	{
		ICONINDEX* pData = (ICONINDEX*)pVoid;
		if(!pData)
			pData = (ICONINDEX*)Create();
		if(pData)
			memset(pData,'\0',sizeof(ICONINDEX));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((ICONINDEX*)iter->data)
			delete ((ICONINDEX*)iter->data),iter->data = (ICONINDEX*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(ICONINDEX));}
#ifdef _DEBUG
	const char* Signature(void) {return("CIconIndexList");}
#endif
};

int GetIconIndex(LPCSTR lpcszFileName,CIconIndexList* plistIconIndex,CListCtrlEx* pListCtrlEx);

#endif // _WALLPAPERICONINDEX_H
