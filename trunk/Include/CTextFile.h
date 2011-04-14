/*
	CTextFile.h
	Classe derivata per interfaccia file di testo (SDK).
	Luca Piergentili, 06/07/98
	lpiergentili@yahoo.com
*/
#ifndef _CTEXTFILE_H
#define _CTEXTFILE_H 1

#include "window.h"
#include "CBinFile.h"

class CTextFile : public CBinFile
{
public:
	CTextFile() : CBinFile() {}
	virtual ~CTextFile() {}

	inline void	GoTop			(void) {CBinFile::Seek(0L,FILE_BEGIN);}
	inline void	GoBottom			(void) {CBinFile::Seek(0L,FILE_END);}
	DWORD		ReadLine			(LPSTR lpBuffer,DWORD dwToRead);
	DWORD		WriteLine			(LPCSTR lpcBuffer,DWORD dwToWrite = (DWORD)-1L);
	DWORD		WriteFormattedLine	(LPSTR pFmt,...);
};

#endif // _CTEXTFILE
