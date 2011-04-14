/*
	WallPaperMessages.h
	Messaggi utilizzati dall'applicazione.
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
#ifndef _WALLPAPERMESSAGES_H
#define _WALLPAPERMESSAGES_H 1

#define ID_TIMER_DESKTOP					1
#define ID_TIMER_NERVOUS_TITLE			2
#define ID_TIMER_AUDIO_PLAYER				3
#define ID_TIMER_SPLASH_SCREEN			4
#define ID_TIMER_POPUP_APPEARING			5
#define ID_TIMER_POPUP_DISAPPEARING		6
#define ID_TIMER_POPUP_HIDDEN				7
#define ID_TIMER_POPUP_WAITING			8
#define ID_TIMER_POPUP_APPEARING_AUDIO		9
#define ID_TIMER_POPUP_DISAPPEARING_AUDIO	10
#define ID_TIMER_POPUP_SCROLLING_AUDIO		11
#define ID_TIMER_POPUP_WAITING_AUDIO		12
#define ID_TIMER_POPUP_APPEARING_PICT		13
#define ID_TIMER_POPUP_DISAPPEARING_PICT	14
#define ID_TIMER_POPUP_SCROLLING_PICT		15
#define ID_TIMER_POPUP_WAITING_PICT		16

									// 1 - 20
#define WM_ONLBUTTONDOWN					WM_USER+1
#define WM_ONLBUTTONUP					WM_USER+2
#define WM_ONRBUTTONDOWN					WM_USER+3
#define WM_ONRBUTTONUP					WM_USER+4
#define WM_ONLBUTTONDBLCLK				WM_USER+5
#define WM_ONRBUTTONDBLCLK				WM_USER+6
#define WM_ONPOPUPMENU					WM_USER+7
#define WM_ONCOLUMNCLICK					WM_USER+8
#define WM_NOTIFYTASKBARICON				WM_USER+9
#define WM_NOTIFYTASKBARICON_AUDIO			WM_USER+10
#define WM_NOTIFYTASKBARPOPUP				WM_USER+11
#define WM_NOTIFYTASKBARPOPUP_AUDIO		WM_USER+12

									// 20 - 40
#define WM_DROPOLE						WM_USER+20
#define WM_SETWALLPAPER					WM_USER+21
#define WM_PLAYLIST_ADDFILE				WM_USER+22
#define WM_PLAYLIST_ADDDIR				WM_USER+23
									
									//40 - 70
#define WM_AUDIOPLAYER					WM_USER+40
#define WM_AUDIOPLAYER_MODE				WM_USER+41
#define WM_AUDIOPLAYER_PLAY				WM_USER+42
#define WM_AUDIOPLAYER_STOP				WM_USER+43
#define WM_AUDIOPLAYER_PAUSE				WM_USER+44
#define WM_AUDIOPLAYER_NEXT				WM_USER+45
#define WM_AUDIOPLAYER_PREV				WM_USER+46
//#define WM_AUDIOPLAYER_DELETE			WM_USER+47
//#define WM_AUDIOPLAYER_MOVETO			WM_USER+48
#define WM_AUDIOPLAYER_REMOVE				WM_USER+49
#define WM_AUDIOPLAYER_REMOVEALL			WM_USER+50
#define WM_AUDIOPLAYER_EXIT				WM_USER+51
#define WM_AUDIOPLAYER_GETSTATUS			WM_USER+52
#define WM_AUDIOPLAYER_SETSTATUS			WM_USER+53
#define WM_AUDIOPLAYER_POPUPLIST			WM_USER+54
#define WM_AUDIOPLAYER_SETVOLUME			WM_USER+55
#define WM_AUDIOPLAYER_EXCEPTION			WM_USER+56
#define WM_AUDIOPLAYER_FAVOURITE			WM_USER+57
#define WM_PLAY_WAVE					WM_USER+58
#define WM_MP3TAGEDITOR_EXIT				WM_USER+60

									//70 - 90
#define WM_UPDATE_GUI					WM_USER+70
#define WM_THREAD_PARAMS					WM_USER+71
#define WM_DOWNLOAD_PROGRESS				WM_USER+72
#define WM_THUMBNAILS_DONE				WM_USER+73
#define WM_CRAWLER_DONE					WM_USER+74
#define WM_AUDIOPLAYER_DONE				WM_USER+75
#define WM_AUDIOPLAYER_EVENT				WM_USER+76
#define WM_AUDIOPLAYER_REMOVEITEM			WM_USER+77
#define WM_PREVIEW_CLOSED				WM_USER+78
#define WM_ARCHIVE_EXTRACT_FILE			WM_USER+79
#define WM_ARCHIVE_GET_PARENT				WM_USER+80

									//90 - ...
#define WM_LOADPLAYLIST					WM_USER+90
#define WM_FAVOURITE_ADD					WM_USER+91
#define WM_CREATEPREVIEWWINDOW			WM_USER+92
#define WM_ENABLEGUI					WM_USER+93
#define WM_DISABLEGUI					WM_USER+94
#define WM_PREVIEWMINMAXIMIZE				WM_USER+95
#define WM_PREVIEWFORCEFOCUS				WM_USER+96
#define WM_PREVIEWENABLED				WM_USER+97
#define WM_PREVIEWDISABLED				WM_USER+98
#define WM_SETCURRENTPLAYLISTITEM			WM_USER+99
#define WM_GETCURRENTPLAYLISTITEM			WM_USER+100
#define WM_SETPLAYLIST					WM_USER+101
#define WM_GETPLAYLIST					WM_USER+102
#define WM_GETCONFIGURATION				WM_USER+103
#define WM_SETCONFIGURATION				WM_USER+104
#define WM_LOADITEM						WM_USER+105

									//110 - 120
#define WM_TOOLTIP_CALLBACK				WM_USER+110
#define WM_NOTIFY_CTRL					WM_USER+111
#define WM_SETPARENTWINDOW				WM_USER+112
#define WM_GETPARENTWINDOW				WM_USER+113

#endif // _WALLPAPERMESSAGES_H
