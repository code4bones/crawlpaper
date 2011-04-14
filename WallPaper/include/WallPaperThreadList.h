/*
	WallPaperThreadList.h
	Classe per la lista dei threads attivi.
	Luca Piergentili, 14/06/01
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
#ifndef _WALLPAPERTHREADLIST_H
#define _WALLPAPERTHREADLIST_H 1

#include "window.h"
#include "CNodeList.h"
#include "CThread.h"

#define MAX_THREAD_NAME _MAX_PATH+1

/*
	THREAD
	struttura per l'elemento della lista dei threads attivi
*/
struct THREAD {
	CThread*		pThread;
	CWinThread*	pWinThread;
	char			szThreadName[MAX_THREAD_NAME+1];
	DWORD		dwThreadId;
	LPARAM		lParam;
	
	THREAD()
	{
		pThread = NULL;
		pWinThread = NULL;
		memset(szThreadName,'\0',MAX_THREAD_NAME);
		dwThreadId = (DWORD)-1L;
		lParam = (LPARAM)NULL;
	}
	
	~THREAD()
	{
		if(pThread)
			delete pThread,pThread = NULL;
		if(pWinThread)
			delete pWinThread,pWinThread = NULL;
		memset(szThreadName,'\0',MAX_THREAD_NAME);
	}

	void Reset(void)
	{
		if(pThread)
			delete pThread,pThread = NULL;
		if(pWinThread)
			delete pWinThread,pWinThread = NULL;
		memset(szThreadName,'\0',MAX_THREAD_NAME);
	}
};

/*
	CWallPaperThreadList
	classe per la lista dei threads attivi
*/
class CWallPaperThreadList : public CNodeList
{
public:
	CWallPaperThreadList() : CNodeList() {}
	virtual ~CWallPaperThreadList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new THREAD);
	}
	void* Initialize(void* pVoid)
	{
		THREAD* pData = (THREAD*)pVoid;
		if(!pData)
			pData = (THREAD*)Create();
		if(pData)
			pData->Reset();
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((THREAD*)iter->data)
			delete ((THREAD*)iter->data),iter->data = (THREAD*)NULL;
		return(TRUE);
	}
	int Size(void) {return(-1);}
#ifdef _DEBUG
	const char* Signature(void) {return("CWallPaperThreadList");}
#endif
};

#endif // _WALLPAPERTHREADLIST_H
