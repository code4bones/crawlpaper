/*
	wtime.h
	Luca Piergentili, 12/03/99
	l.piergentili@ifies.es

	Gestione data/ora.
*/
#ifndef _WTIME_H
#define _WTIME_H 1

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

/* headers */
#define STRICT 1
#include <windows.h>

#define	OS_TIME_SIZE			10
#define	OS_DATE_SIZE			10
#define	GMT_DATE_SIZE			32

void		Get12HourTime			(LPSTR);
void		GetOsTime				(LPSTR);
void		GetOsDate				(LPSTR);
void		GetGmtDate			(LPSTR);
int		CompareGmtDate			(LPCSTR,LPCSTR);
int		GetDstZone			(void);
long		GetTimeZoneDiff		(void);
LPCSTR	GetTimeZoneName		(void);
LPCSTR	GetDstZoneName			(void);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _WTIME_H */
