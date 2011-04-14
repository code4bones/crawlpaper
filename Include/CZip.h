/*
	CZip.h
	Luca Piergentili, 01/12/03
	lpiergentili@yahoo.com
*/
#ifndef _CZIP_H
#define _CZIP_H 1

#include "/unzip550/win32/rsxntwin.h"
#include "/unzip550/windll/structs.h"
#include "/unzip550/unzip.h"
#include "/unzip550/unzvers.h"

#define ZIP_EXTENSION	".zip"
#define ZIP_TYPEDESC	"compressed data"

#ifdef _DEBUG
  #pragma comment(lib,"unzip32.d.lib")
#else
  #pragma comment(lib,"unzip32.lib")
#endif

// prototipi per le funzioni della dll, gli header di unzip550 non esportano le funzioni
#ifdef __cplusplus
extern "C" {
#endif

int WINAPI Wiz_SingleEntryUnzip(int ifnc,char **ifnv,int xfnc,char **xfnv,LPDCL C,LPUSERFUNCTIONS lpUserFunc);

#ifdef __cplusplus
}
#endif

#endif /* _CZIP_H */
