/*
	CCrypto.h
	Interfaccia per crypto++ 2.3 (la 3.0 compila solo con MSVC 6.0)
	Luca Piergentili, 10/05/00
	lpiergentili@yahoo.com
*/
#ifndef _CCRYPTO_H
#define _CCRYPTO_H 1

#ifdef _DEBUG
	#if defined(_CRYPTLIB_STATIC_LIBRARY)
		#pragma comment(lib,"cryptlibst.d.lib")
		#pragma message("\t\t\t"__FILE__" using cryptlibst.d.lib")
	#else
		#pragma comment(lib,"cryptlib.d.lib")
		#pragma message("\t\t\t"__FILE__" using cryptlib.d.lib")
	#endif
#else
	#if defined(_CRYPTLIB_STATIC_LIBRARY)
		#pragma comment(lib,"cryptlibst.lib")
		#pragma message("\t\t\t"__FILE__" using cryptlibst.lib")
	#else
		#pragma comment(lib,"cryptlib.lib")
		#pragma message("\t\t\t"__FILE__" using cryptlib.lib")
	#endif
#endif

// interfaccia crypto++ 2.3
#include "l:/crypto++.2.3/include/config.h"
#include "l:/crypto++.2.3/include/cryptlib.h"
#include "l:/crypto++.2.3/include/default.h"
#include "l:/crypto++.2.3/include/hex.h"
#include "l:/crypto++.2.3/include/files.h"
#pragma message("\t\t\t"__FILE__" warning: headers referenced with absolute path")

// 3.0
//USING_NAMESPACE(CryptoPP)

class CCrypto
{
public:
	CCrypto() {}
	virtual ~CCrypto() {}
	
	// testo, password, output, dim. output
	char*	DESEncryptString	(const char*,const char*,char*,int);
	char*	DESDecryptString	(const char*,const char*,char*,int);
};

#endif // _CCRYPTO_H
