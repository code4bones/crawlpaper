/*
	WallPaperCmdLine.h
	Gestione della linea di comando.
	Luca Piergentili, 02/08/00
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
#ifndef _WALLPAPERCMDLINE_H
#define _WALLPAPERCMDLINE_H 1

#include "window.h"
#include <string.h>
#include "strings.h"
#include "CGetopt.h"
#include "CImage.h"
#include "CAudioPlayer.h"

#define WALLPAPER_CMDLINE_OPTIONS 6

/*
	CCmdLineOptions
*/
class CCmdLineOptions : public CGetopt
{
public:
	CCmdLineOptions(LPCSTR pCommandLine)
	{
		pOptions = new GETOPT[WALLPAPER_CMDLINE_OPTIONS];
		if(pOptions)
		{
			Reset();
			CGetopt::GetCommandLineOptions(pCommandLine,pOptions,WALLPAPER_CMDLINE_OPTIONS);
		}
	}
	
	~CCmdLineOptions()
	{
		if(pOptions)
			delete [] pOptions,pOptions = NULL;
	}

	//	/l<filename.dpl>
	inline BOOL	HaveDplFile(void)			{return(pOptions[0].bFound);}
	inline void	ResetDplFile(void)			{pOptions[0].bFound = FALSE; memset(pOptions[0].uValue.szValue,'\0',MAX_VALUE+1);}
	inline LPCSTR	GetDplFile(void)			{return(pOptions[0].bFound ? pOptions[0].uValue.szValue : NULL);}
	inline void	SetDplFile(LPCSTR file)		{pOptions[0].bFound = TRUE; strcpyn(pOptions[0].uValue.szValue,file,MAX_VALUE+1);}

	//	/w<picture file>
	inline BOOL	HavePictureFile(void)		{return(pOptions[1].bFound);}
	inline void	ResetPictureFile(void)		{pOptions[1].bFound = FALSE; memset(pOptions[1].uValue.szValue,'\0',MAX_VALUE+1);}
	inline LPCSTR	GetPictureFile(void)		{return(pOptions[1].bFound ? pOptions[1].uValue.szValue : NULL);}
	inline void	SetPictureFile(LPCSTR file)	{pOptions[1].bFound = TRUE; strcpyn(pOptions[1].uValue.szValue,file,MAX_VALUE+1);}

	//	/a<audio file> (play)
	inline BOOL	HaveAudioFile(void)			{return(pOptions[2].bFound);}
	inline void	ResetAudioFile(void)		{pOptions[2].bFound = FALSE; memset(pOptions[2].uValue.szValue,'\0',MAX_VALUE+1);}
	inline LPCSTR	GetAudioFile(void)			{return(pOptions[2].bFound ? pOptions[2].uValue.szValue : NULL);}
	inline void	SetAudioFile(LPCSTR file)	{pOptions[2].bFound = TRUE; strcpyn(pOptions[2].uValue.szValue,file,MAX_VALUE+1);}

	//	/A<audio file> (queue)
	inline BOOL	HaveQueuedAudioFile(void)	{return(pOptions[3].bFound);}
	inline void	ResetQueuedAudioFile(void)	{pOptions[3].bFound = FALSE; memset(pOptions[3].uValue.szValue,'\0',MAX_VALUE+1);}
	inline LPCSTR	GetQueuedAudioFile(void)		{return(pOptions[3].bFound ? pOptions[3].uValue.szValue : NULL);}
	inline void	SetQueuedAudioFile(LPCSTR file){pOptions[3].bFound = TRUE; strcpyn(pOptions[3].uValue.szValue,file,MAX_VALUE+1);}

	//	/i<install directory + .exe>
	inline BOOL	HaveInstall(void)			{return(pOptions[4].bFound);}
	inline void	ResetInstallDir(void)		{pOptions[4].bFound = FALSE; memset(pOptions[4].uValue.szValue,'\0',MAX_VALUE+1);}
	inline LPCSTR	GetInstallDir(void)			{return(pOptions[4].bFound ? pOptions[4].uValue.szValue : NULL);}
	inline void	SetInstallDir(LPCSTR dir)	{pOptions[4].bFound = TRUE; strcpyn(pOptions[4].uValue.szValue,dir,MAX_VALUE+1);}

	//	/u
	inline BOOL	HaveUninstall(void)			{return(pOptions[5].bFound);}
	inline void	ResetUninstall(void)		{pOptions[5].bFound = FALSE;}

	BOOL HaveCommandLine(void)
	{
		for(int i = 0; i < WALLPAPER_CMDLINE_OPTIONS; i++)
			if(pOptions[i].bFound)
				return(TRUE);
		return(FALSE);
	}
	
	int GetCommandLineOptions(LPCSTR pCommandLine)
	{
		int nRet = CGetopt::GetCommandLineOptions(pCommandLine,pOptions,WALLPAPER_CMDLINE_OPTIONS);
		char szLongFileName[_MAX_FILEPATH+1] = {0};

		if(HaveDplFile())
		{
			if(::GetLongPathName(GetDplFile(),szLongFileName,sizeof(szLongFileName)-1)!=0L)
				SetDplFile(szLongFileName);

			if(striright(GetDplFile(),".dpl")!=0)
			{
				if(CImage::IsImageFile(GetDplFile()))
					SetPictureFile(GetDplFile());
				else if(CAudioPlayer::IsSupportedFormat(GetDplFile()))
					SetAudioFile(GetDplFile());
				ResetDplFile();
			}
		}

		if(HavePictureFile())
		{
			if(::GetLongPathName(GetPictureFile(),szLongFileName,sizeof(szLongFileName)-1)!=0L)
				SetPictureFile(szLongFileName);

			if(!CImage::IsImageFile(GetPictureFile()))
			{
				if(striright(GetPictureFile(),".dpl")==0)
					SetDplFile(GetPictureFile());
				else if(CAudioPlayer::IsSupportedFormat(GetPictureFile()))
					SetAudioFile(GetPictureFile());
				ResetPictureFile();
			}
		}

		if(HaveAudioFile())
		{
			if(::GetLongPathName(GetAudioFile(),szLongFileName,sizeof(szLongFileName)-1)!=0L)
				SetAudioFile(szLongFileName);

			if(!CAudioPlayer::IsSupportedFormat(GetAudioFile()))
			{
				if(striright(GetAudioFile(),".dpl")==0)
					SetDplFile(GetAudioFile());
				else if(CImage::IsImageFile(GetAudioFile()))
					SetPictureFile(GetAudioFile());
				ResetAudioFile();
			}
		}

		if(HaveQueuedAudioFile())
		{
			if(::GetLongPathName(GetQueuedAudioFile(),szLongFileName,sizeof(szLongFileName)-1)!=0L)
				SetQueuedAudioFile(szLongFileName);

			if(!CAudioPlayer::IsSupportedFormat(GetQueuedAudioFile()))
			{
				if(striright(GetQueuedAudioFile(),".dpl")==0)
					SetDplFile(GetQueuedAudioFile());
				else if(CImage::IsImageFile(GetQueuedAudioFile()))
					SetPictureFile(GetQueuedAudioFile());
				ResetQueuedAudioFile();
			}
		}

		return(nRet);
	}

	void Reset(void)
	{
		pOptions[0].cOpt   = 'l';
		pOptions[0].bFound = FALSE;
		pOptions[0].bArgs  = TRUE;
		pOptions[0].eType  = string;

		pOptions[1].cOpt   = 'w';
		pOptions[1].bFound = FALSE;
		pOptions[1].bArgs  = TRUE;
		pOptions[1].eType  = string;

		pOptions[2].cOpt   = 'a';
		pOptions[2].bFound = FALSE;
		pOptions[2].bArgs  = TRUE;
		pOptions[2].eType  = string;

		pOptions[3].cOpt   = 'A';
		pOptions[3].bFound = FALSE;
		pOptions[3].bArgs  = TRUE;
		pOptions[3].eType  = string;

		pOptions[4].cOpt   = 'i';
		pOptions[4].bFound = FALSE;
		pOptions[4].bArgs  = TRUE;
		pOptions[4].eType  = string;

		pOptions[5].cOpt   = 'u';
		pOptions[5].bFound = FALSE;
		pOptions[5].bArgs  = FALSE;
		pOptions[5].eType  = none;
	}

private:
	GETOPT* pOptions;
};

#endif // _WALLPAPERCMDLINE_H
