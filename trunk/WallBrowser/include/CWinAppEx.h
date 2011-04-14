/*
	CWinAppEx.h
	Classe per l'applicazione.
	Luca Piergentili, 30/07/01
	lpiergentili@yahoo.com

	WallBrowser - the smart picture browser
	http://www.crawlpaper.com/

	Written by Luca Piergentili, 2000-2002 - All rights reserved
	This software is placed into public domain and is provided as freeware (no commercial use/purpose allowed)
	you can use it only if you accept the following terms: 

	THIS SOFTWARE IS PROVIDED BY LUCA PIERGENTILI ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL LUCA PIERGENTILI BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
	OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
	OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _CWINAPPEX_H
#define _CWINAPPEX_H 1

#include "window.h"
#include "WallBrowserConfig.h"

class CWinAppEx : public CWinApp
{
public:
	CWallBrowserConfig m_Config;
};

#endif // _CWINAPPEX_H
