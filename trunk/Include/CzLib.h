/*
	CzLib.h
	Classe base per interfaccia con la libreria zLib di Jean-loup Gailly e Mark Adler (http://www.gzip.org/zlib/) (CRT/SDK/MFC).
	Luca Piergentili, 20/12/98
	lpiergentili@yahoo.com
*/
#ifndef _CZLIB_H
#define _CZLIB_H 1

#include <stdio.h>
#include <stdlib.h>
#ifdef _WINDOWS
  #include "window.h"
#endif
#include "typedef.h"
#include "macro.h"
#include "zlibapi.h"	// definizione interfaccia (ZLIBAPI)
#include "zlib.h"		// interfaccia zLib

// crea la referenza:
// _ZLIB_DLL per usare la dll
// _ZLIB_LIB per usare la libreria
// <nessuna definizione> per il link diretto con il codice
#if defined(_ZLIB_DLL)
	//
	//	_ZLIB_DLL
	//
#ifdef PRAGMA_MESSAGE_VERBOSE
	#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): zLib support through dll")
#endif
	#ifdef _DEBUG
		#pragma comment(lib,"zLibdll.d.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
		#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with zLibdll.d.dll")
#endif
	#else
		#pragma comment(lib,"zLibdll.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
		#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with zLibdll.dll")
#endif
	#endif
#elif defined(_ZLIB_LIB)
	//
	//	_ZLIB_LIB
	//
#ifdef PRAGMA_MESSAGE_VERBOSE
	#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): zLib support through lib")
#endif
	#ifdef _DEBUG
		#pragma comment(lib,"zLiblib.d.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
		#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with zLibdll.d.lib")
#endif
	#else
		#pragma comment(lib,"zLiblib.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
		#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with zLibdll.d.lib")
#endif
	#endif
#else
	//
	//	link con il codice
	//
	#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): warning: zLib support undefined (are you linking directly with the zLib code?)")
#endif

#define ZIP_EXTENSION	".zip"
#define ZIP_TYPEDESC	"compressed data"

/*
	CzLib
*/
class CzLib
{
public:
	CzLib() {}
	virtual ~CzLib() {}
	
	// se usata direttamente, esporta solo le due funzioni basiche
	inline BOOL Compress(const char* filein,const char* fileout) {return(gzcompressfile(filein,fileout)==0);}
	inline BOOL UnCompress(const char* filein,const char* fileout) {return(gzuncompressfile(filein,fileout)==0);}

protected:
	// se usata come classe derivata, esporta l'interfaccia della zLib
	inline int		gzcompressfile		(const char* a,const char* b)			{return(::gzcompressfile(a,b));}
	inline int		gzuncompressfile	(const char* a,const char* b)			{return(::gzuncompressfile(a,b));}
	inline int		gzcompress		(FILE* a,gzFile b)					{return(::gzcompress(a,b));}
	inline int		gzuncompress		(gzFile a,FILE* b)					{return(::gzuncompress(a,b));}
	inline gzFile		gzopen			(const char* a,const char* b,GZWHDR* c)	{return(::gzopen(a,b,c));}
	inline gzFile		gzdopen			(int a,const char* b)				{return(::gzdopen(a,b));}
	inline int		gzread			(gzFile a,voidp b,unsigned int c)		{return(::gzread(a,b,c));}
	inline int		gzwrite			(gzFile a,const voidp b,unsigned int c)	{return(::gzwrite(a,b,c));}
	inline int		gzflush			(gzFile a,int b)					{return(::gzflush(a,b));}
	inline int		gzclose			(gzFile a)						{return(::gzclose(a));}
	inline const char*	gzerror			(gzFile a,int* b)					{return(::gzerror(a,b));}
	inline const char*	version			(void)							{return(::zlibVersion());}
};

#endif // _CZLIB_H
