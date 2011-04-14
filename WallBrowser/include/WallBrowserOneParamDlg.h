/*
	WallBrowserOneParamDlg.h
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
#ifndef _WALLBROWSERONEPARAM_H
#define _WALLBROWSERONEPARAM_H 1

#include "window.h"
#include "resource.h"

/*
	CWallBrowserOneParamDlg
*/
class CWallBrowserOneParamDlg : public CDialog
{
public:
	CWallBrowserOneParamDlg(
						CString strProperty,
						CString strOperation,
						double nDefaultValue = 0.0,
						double nMin = 0.0,
						double nMax = 100.0
						);	
	~CWallBrowserOneParamDlg() {}

	inline int	GetIntValue	(void) const {return((int)m_nValue);}
	inline double	GetDoubleValue	(void) const {return(m_nValue);}

private:
	void			DoDataExchange	(CDataExchange* pDX);	
	BOOL			OnInitDialog	(void);
	void			OnOK			(void);
	void			OnVScroll		(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	void			OnHScroll		(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	void			OnChangeValue	(void);

	CString		m_strProperty;
	CString		m_strOperation;
	double		m_nValue;
	double		m_nDefaultValue;
	double		m_nMin;
	double		m_nMax;
	CSliderCtrl	m_ctrlSlider;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLBROWSERONEPARAM_H
