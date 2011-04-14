/*
	WallBrowserMessages.h
	Messaggi utilizzati dall'applicazione.
	Luca Piergentili, 30/00/00
	lpiergentili@yahoo.com

	WallBrowser - the smart picture browser
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
#ifndef _WALLBROWSERMESSAGES_H
#define _WALLBROWSERMESSAGES_H 1

#include "strings.h"
#include "window.h"

#define ONUPDATE_FLAG_DIRSELECTED		0x00000001
#define ONUPDATE_FLAG_DIRCHANGED		0x00000002
#define ONUPDATE_FLAG_DIRUPDATED		0x00000004
#define ONUPDATE_FLAG_FILESELECTED		0x00000008
#define ONUPDATE_FLAG_FILECHANGED		0x00000010
#define ONUPDATE_FLAG_FILEDELETED		0x00000020
#define ONUPDATE_FLAG_EMPTYSTACK		0x00000040
#define ONUPDATE_FLAG_DEFAULTLIBRARY	0x00000080
#define ONUPDATE_FLAG_DRAWSTRETCHDIBITS	0x00000100
#define ONUPDATE_FLAG_DRAWSTRETCHBLT	0x00000200
#define ONUPDATE_FLAG_DRAWVFWDRAWDIB	0x00000400
#define ONUPDATE_FLAG_VIEWTYPE		0x00000800
#define ONUPDATE_FLAG_ZOOMIN			0x00001000
#define ONUPDATE_FLAG_ZOOMOUT			0x00002000
#define ONUPDATE_FLAG_ZOOMRESET		0x00004000

#define ONUPDATE_FLAG_TREEEXPANDED		0x00008000
#define ONUPDATE_FLAG_TREEEXPANDEDEX	0x00010000

#define WM_FILE_SAVE_AS				WM_USER+1
#define WM_FILE_SAVE				WM_USER+2
#define WM_FILE_MOVE				WM_USER+3
#define WM_FILE_DELETE				WM_USER+4

#define WM_EDIT_COPY				WM_USER+10
#define WM_EDIT_PASTE				WM_USER+11
#define WM_EDIT_UNDO				WM_USER+12
#define WM_EDIT_REDO				WM_USER+13

#define WM_VIEW_PREVIOUS_PICTURE		WM_USER+14
#define WM_VIEW_NEXT_PICTURE			WM_USER+15

#define WM_TOOLS_SETWALLPAPER			WM_USER+16
#define WM_TOOLS_RESETWALLPAPER		WM_USER+17
#define WM_TOOLS_LIBRARY				WM_USER+18

#define WM_IMAGE_MIRROR_H			WM_USER+20
#define WM_IMAGE_MIRROR_V			WM_USER+21
#define WM_IMAGE_ROTATE_90_LEFT		WM_USER+22
#define WM_IMAGE_ROTATE_90_RIGHT		WM_USER+23
#define WM_IMAGE_ROTATE_180			WM_USER+24
#define WM_IMAGE_DESKEW				WM_USER+25
#define WM_IMAGE_SIZE				WM_USER+26
#define WM_IMAGE_POSTERIZE			WM_USER+27
#define WM_IMAGE_MOSAIC				WM_USER+28
#define WM_IMAGE_BLUR				WM_USER+29
#define WM_IMAGE_MEDIAN				WM_USER+30
#define WM_IMAGE_SHARPEN				WM_USER+31
#define WM_IMAGE_DESPECKLE			WM_USER+32
#define WM_IMAGE_NOISE				WM_USER+33
#define WM_IMAGE_EMBOSS				WM_USER+34
#define WM_IMAGE_EDGEENHANCE			WM_USER+35
#define WM_IMAGE_FINDEDGE			WM_USER+36
#define WM_IMAGE_EROSION				WM_USER+37
#define WM_IMAGE_DILATE				WM_USER+38

#define WM_COLOR_HALFTONE			WM_USER+40
#define WM_COLOR_GRAYSCALE			WM_USER+41
#define WM_COLOR_INVERT				WM_USER+42
#define WM_COLOR_BRIGHTNESS			WM_USER+43
#define WM_COLOR_CONTRAST			WM_USER+44
#define WM_COLOR_HISTOGRAM_CONTRAST	WM_USER+45
#define WM_COLOR_HUE				WM_USER+46
#define WM_COLOR_SATURATION_H			WM_USER+47
#define WM_COLOR_SATURATION_V			WM_USER+48
#define WM_COLOR_EQUALIZE			WM_USER+49
#define WM_COLOR_INTENSITY			WM_USER+50
#define WM_COLOR_INTENSITY_DETECT		WM_USER+51
#define WM_COLOR_GAMMA_CORRECTION		WM_USER+52

#define WM_GET_STATUSBAR				WM_USER+60
#define WM_TOGGLE_FILESPLITTER		WM_USER+61
#define WM_TOGGLE_DIRSPLITTER			WM_USER+62
#define WM_GET_FILESPLITTER_SIZE		WM_USER+63
#define WM_GET_DIRSPLITTER_SIZE		WM_USER+64

class COnUpdateInfo : public CObject
{
public:
	COnUpdateInfo(LPCSTR lpcszInfo) {strcpyn(m_szInfo,lpcszInfo,sizeof(m_szInfo));}
	void SetInfo(LPCSTR lpcszInfo) {strcpyn(m_szInfo,lpcszInfo,sizeof(m_szInfo));}
	LPCSTR GetInfo(void) {return(m_szInfo);}

private:
	char m_szInfo[256];
};

#endif // _WALLBROWSERMESSAGES_H
