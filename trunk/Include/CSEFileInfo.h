/*
	CSEFileInfo.h
	James Spibey, 04/08/1998
	spib@bigfoot.com
	Specification of the CSEFileInfo class.
	This class holds data regarding each file in an archive
	You are free to use, distribute or modify this code as long as this header is not removed or modified.

	Rimossi i riferimenti a MFC (solo SDK) e cambi vari.
	Luca Piergentili, 24/08/00
	lpiergentili@yahoo.com
*/
#ifndef _CSEFILEINFO_H
#define _CSEFILEINFO_H 1

#include <stdlib.h>
#include "strings.h"
#include "window.h"

/*
	CSEFileInfo
*/
class CSEFileInfo
{
public:
	CSEFileInfo() {Reset();}
	virtual ~CSEFileInfo() {}

	BOOL			SetData		(LPCSTR lpcszFileName);
	inline void	SetFileName	(LPCSTR pFile)		{strcpyn(m_szFileName,pFile,sizeof(m_szFileName));}
	inline void	SetFileSize	(DWORD dwSize)		{m_dwSize = dwSize;}
	inline void	SetFileOffset	(DWORD dwOffset)	{m_dwOffset = dwOffset;}
	
	inline LPCSTR	GetPathName	(void) const		{return(m_szPathName);}
	inline LPCSTR	GetFileName	(void) const		{return(m_szFileName);}
	inline DWORD	GetFileSize	(void) const		{return(m_dwSize);}
	inline DWORD	GetFileOffset	(void) const		{return(m_dwOffset);}
	
	void			Reset		(void);

protected:
	char			m_szPathName[_MAX_PATH+1];
	char			m_szFileName[_MAX_PATH+1];
	DWORD		m_dwSize;
	DWORD		m_dwOffset;
};

#endif // _CSEFILEINFO_H
