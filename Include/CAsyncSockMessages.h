/*
	CAsyncSockMessages.h
	Luca Piergentili, 04/09/98
	lpiergentili@yahoo.com
*/
#ifndef _CASYNCSOCKMESSAGES_H
#define _CASYNCSOCKMESSAGES_H 1

// messaggi per WSAAsync...()
#define WM_ASYNCSELECT				(0x7FFF - 1)
#define WM_ASYNCREAD				(0x7FFF - 2)
#define WM_ASYNCWRITE				(0x7FFF - 3)
#define WM_ASYNCOOB					(0x7FFF - 4)
#define WM_ASYNCACCEPT				(0x7FFF - 5)
#define WM_ASYNCCONNECT				(0x7FFF - 6)
#define WM_ASYNCCLOSE				(0x7FFF - 7)

// idem per modalita' sincrona
#define WM_SYNCSELECT				(0x7FFF - 8)
#define WM_SYNCREAD					(0x7FFF - 9)
#define WM_SYNCWRITE				(0x7FFF - 10)
#define WM_SYNCOOB					(0x7FFF - 11)
#define WM_SYNCACCEPT				(0x7FFF - 12)
#define WM_SYNCCONNECT				(0x7FFF - 13)
#define WM_SYNCCLOSE				(0x7FFF - 14)

// messaggi per WSAAsyncGetXByY()
#define WM_ASYNCGETHOSTBYNAME			(0x7FFF - 15)
#define WM_ASYNCGETHOSTBYADDR			(0x7FFF - 16)
#define WM_ASYNCGETSERVBYNAME			(0x7FFF - 17)
#define WM_ASYNCGETSERVBYPORT			(0x7FFF - 18)

#endif // _CASYNCSOCKMESSAGES_H
