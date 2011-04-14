//this file is part of eMule
//Copyright (C)2002 Merkur ( merkur-@users.sourceforge.net / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

// Modificato il codice originale (vedi sopra).
// Luca Piergentili, 02/05/04

#ifndef _CICONSTATIC_H
#define _CICONSTATIC_H 1

#include "window.h"

/*
	CTempIconLoader
*/
class CTempIconLoader
{
public:
	CTempIconLoader(UINT nIconID, int cx = 16, int cy = 16, UINT uFlags = LR_DEFAULTCOLOR);
	~CTempIconLoader();

	operator HICON() const {return(this==NULL ? NULL : m_hIcon);}

protected:
	HICON	m_hIcon;
};

/*
	CIconStatic
*/
class CIconStatic : public CStatic
{
public:
	CIconStatic();
	virtual ~CIconStatic();

	void		SetIconSize		(UINT nIconSize = 16/* 16=small, 32=large */) {m_nIconSize = nIconSize;}
	BOOL		SetIcon			(UINT nIconID);
	BOOL		SetText			(LPCSTR lpcszText);
	BOOL		SetText			(CString& strText);

protected:
	void		OnSysColorChange	(void);

private:
	BOOL		Init				(UINT nIconID);

	CStatic	m_wndPicture;
	UINT		m_nIconSize;
	UINT		m_nIconID;
	CBitmap	m_bmpIcon;
	CString	m_strText;

	DECLARE_MESSAGE_MAP()
};

#endif // _CICONSTATIC_H
