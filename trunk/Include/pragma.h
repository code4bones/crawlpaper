/*
	pragma.h
	Elimina i warning scassacazzi del VC++ 4.2 (da includere nei .c/.cpp, nei .h se includono codice inline).
	Luca Piergentili, 13/08/96
	lpiergentili@yahoo.com
*/
#ifndef _PRAGMA_H
#define _PRAGMA_H 1

#if defined(_MSC_VER)
	#ifdef _DEBUG
		#ifdef PRAGMA_MESSAGE_VERBOSE
			#pragma message("\t\t\t"__FILE__" included: some warning disabled")
		#endif
//		#pragma warning(disable:4100) /* unreferenced formal parameter */
//		#pragma warning(disable:4102) /* unreferenced label */
//		#pragma warning(disable:4115) /* named type definition in parentheses */
//		#pragma warning(disable:4127) /* conditional expression is constant */
//		#pragma warning(disable:4131) /* uses old-style declarator */
//		#pragma warning(disable:4146) /* (headers VC) unary minus operator applied to unsigned type, result still unsigned */
//		#pragma warning(disable:4206) /* nonstandard extension used : translation unit is empty */
//		#pragma warning(disable:4214) /* nonstandard extension used : bit field types other than int */
//		#pragma warning(disable:4244) /* conversion from 'int' to 'unsigned short', possible loss of data */
//		#pragma warning(disable:4305) /* abstract declarator : truncation from '...' to '...' */
//		#pragma warning(disable:4505) /* unreferenced local function has been removed */
//		#pragma warning(disable:4514) /* unreferenced inline function has been removed */
//		#pragma warning(disable:4663) /* (headers VC) C++ language change: to explicitly specialize class template '...' use the following syntax:... */
	#endif

	#pragma warning(disable:4201) /* nonstandard extension used : nameless struct/union */
	#pragma warning(disable:4482) /* nonstandard extension used: enum ... used in qualified name */
	#pragma warning(disable:4996) /* MSVC2008: this function or variable may be unsafe (deprecated) */
#endif

#endif /* _PRAGMA_H */
