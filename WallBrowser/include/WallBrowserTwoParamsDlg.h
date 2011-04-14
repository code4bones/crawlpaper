/*
	WallBrowserTwoParamsDlg.h
	Dialogo per le opzioni.
	Luca Piergentili, 30/09/01
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
#ifndef _WALLBROWSERTWOPARAMS_H
#define _WALLBROWSERTWOPARAMS_H 1

#include "window.h"
#include "resource.h"

/*
	CWallBrowserTwoParamsDlg
*/
class CWallBrowserTwoParamsDlg : public CDialog
{
public:
	CWallBrowserTwoParamsDlg(
						CString strProperty,
						CString strOperation,
						CString strSubOperation,
						int nFirstDefaultValue = 0,
						int nFirstMin = 0,
						int nFirstMax = 100,
						int nSecondDefaultValue = 0,
						int nSecondMin = 0,
						int nSecondMax = 100
						);
	~CWallBrowserTwoParamsDlg() {}

	inline int GetFirstValue		(void) const {return m_nFirstValue;}
	inline int GetSecondValue	(void) const {return m_nSecondValue;}

protected:
	void		DoDataExchange		(CDataExchange* pDX);	
	BOOL		OnInitDialog		(void);
	void		OnOK				(void);
	void		OnKillFocusCombo	(void);
	void		OnSelChangeCombo	(void);
	void		OnVScroll			(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);

	BOOL		m_bHaveCombo;
	CString	m_strProperty;
	CString	m_strOperation;
	CString	m_strSubOperation;
	int		m_nFirstValue;
	int		m_nSecondValue;

	int		m_nFirstDefaultValue;
	int		m_nFirstMin; 
	int		m_nFirstMax;
	int		m_nSecondDefaultValue;
	int		m_nSecondMin; 
	int		m_nSecondMax;

	CComboBox	m_CComboSubValue;
	char		m_szComboSubValue[256];

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLBROWSERTWOPARAMS_H
