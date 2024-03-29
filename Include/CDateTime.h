/*
	CDateTime.h
	Classe base per data/ora (CRT/SDK/MFC).
	Luca Piergentili, 24/11/99
	lpiergentili@yahoo.com

	UTC(=GMT): Sun, 06 Nov 1994 08:49:37 GMT (0000)

	con timezone = +1 (ossia 'HST' = 'GMT' + '+0100'):
		Sun, 06 Nov 1994 08:49:37 GMT
		Sun, 06 Nov 1994 09:49:37 HST
		Sun, 06 Nov 1994 09:49:37 +0100

	rfc822:
	=======
		Time zone may be indicated in several ways.  "UT" is Univer-
	sal  Time  (formerly called "Greenwich Mean Time"); "GMT" is per-
	mitted as a reference to Universal Time.  The  military  standard
	uses  a  single  character for each zone.  "Z" is Universal Time.
	"A" indicates one hour earlier, and "M" indicates 12  hours  ear-
	lier;  "N"  is  one  hour  later, and "Y" is 12 hours later.  The
	letter "J" is not used.  The other remaining two forms are  taken
	from ANSI standard X3.51-1975.  One allows explicit indication of
	the amount of offset from UT; the other uses  common  3-character
	strings for indicating time zones in North America.
	
	rfc1036:
	========
	The "Date" line (formerly "Posted") is the date that the message was
	originally posted to the network.  Its format must be acceptable
	both in RFC-822 and to the getdate(3) routine that is provided with
	the Usenet software.  This date remains unchanged as the message is
	propagated throughout the network.  One format that is acceptable to
	both is:

	Wdy, DD Mon YY HH:MM:SS TIMEZONE

	rfc1945:
	========
	3.3  Date/Time Formats

	HTTP/1.0 applications have historically allowed three different
	formats for the representation of date/time stamps:

	Sun, 06 Nov 1994 08:49:37 GMT    ; RFC 822, updated by RFC 1123
	Sunday, 06-Nov-94 08:49:37 GMT   ; RFC 850, obsoleted by RFC 1036
	Sun Nov  6 08:49:37 1994         ; ANSI C's asctime() format

	The first format is preferred as an Internet standard and represents
	a fixed-length subset of that defined by RFC 1123 [6] (an update to
	RFC 822 [7]). The second format is in common use, but is based on the
	obsolete RFC 850 [10] date format and lacks a four-digit year.
	HTTP/1.0 clients and servers that parse the date value should accept
	all three formats, though they must never generate the third
	(asctime) format.

	Note: Recipients of date values are encouraged to be robust in
	accepting date values that may have been generated by non-HTTP
	applications, as is sometimes the case when retrieving or posting
	messages via proxies/gateways to SMTP or NNTP.

	All HTTP/1.0 date/time stamps must be represented in Universal Time
	(UT), also known as Greenwich Mean Time (GMT), without exception.
	This is indicated in the first two formats by the inclusion of "GMT"
	as the three-letter abbreviation for time zone, and should be assumed
	when reading the asctime format.

	HTTP-date      = rfc1123-date | rfc850-date | asctime-date

	rfc1123-date   = wkday "," SP date1 SP time SP "GMT"
	rfc850-date    = weekday "," SP date2 SP time SP "GMT"
	asctime-date   = wkday SP date3 SP time SP 4DIGIT

	date1          = 2DIGIT SP month SP 4DIGIT
				; day month year (e.g., 02 Jun 1982)
	date2          = 2DIGIT "-" month "-" 2DIGIT
				; day-month-year (e.g., 02-Jun-82)
	date3          = month SP ( 2DIGIT | ( SP 1DIGIT ))
				; month day (e.g., Jun  2)

	time           = 2DIGIT ":" 2DIGIT ":" 2DIGIT
				; 00:00:00 - 23:59:59

	wkday          = "Mon" | "Tue" | "Wed"
				| "Thu" | "Fri" | "Sat" | "Sun"

	weekday        = "Monday" | "Tuesday" | "Wednesday"
				| "Thursday" | "Friday" | "Saturday" | "Sunday"

	month          = "Jan" | "Feb" | "Mar" | "Apr"
				| "May" | "Jun" | "Jul" | "Aug"
				| "Sep" | "Oct" | "Nov" | "Dec"

	Note: HTTP requirements for the date/time stamp format apply
	only to their usage within the protocol stream. Clients and
	servers are not required to use these formats for user
*/
#ifndef _CDATETIME_H
#define _CDATETIME_H 1

#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "Date.h"

#define MAX_DATE_STRING	64	// dim. max per una stringa con la data
#define MAX_TIME_STRING	32	// dim. max per una stringa con l'ora
#define MAX_DATETIME_BUF	16	// dim. max per un elemento della data/ora

/*
	DAY
	tipo per i giorni della settimana
*/
enum DAY {
	MON = 0,
	TUE,
	WED,
	THU,
	FRI,
	SAT,
	SUN
};

/*
	MONTH
	tipi per i mesi dell'anno
*/
enum MONTH {
	JAN = 0,
	FEB,
	MAR,
	APR,
	MAY,
	JUN,
	JUL,
	AUG,
	SEP,
	OCT,
	NOV,
	DEC
};

/*
	DATETIMEOBJECT
	tipo per il tipo dell'oggetto
*/
enum DATETIMEOBJECT {
	YEAR,
	MONTH,
	DAY,
	HOUR,
	MINUTE,
	SECOND
};

/*
	DATEFORMAT
	tipo per i formati della data
*/
enum DATEFORMAT {
	AMERICAN = 0,		// "mm/dd/yyyy"
	ANSI,			// "yyyy.mm.dd"
	ANSI_SHORT,		// "yyyymmdd"
	BRITISH,			// "dd/mm/yyyy"
	FRENCH,			// "dd/mm/yyyy"
	GERMAN,			// "dd.mm.yyyy"
	ITALIAN,			// "dd-mm-yyyy"
	JAPAN,			// "yyyy/mm/dd"
	USA,				// "mm-dd-yyyy"
	MDY,				// "mm/dd/yyyy"
	DMY,				// "dd/mm/yyyy"
	YMD,				// "yyyy/mm/dd"
	GMT_SHORT,		// "Day, dd Mon yyyy hh:mm:ss" (assumendo GMT, ossia convertendo l'UTC in GMT)
	GMT,				// "Day, dd Mon yyyy hh:mm:ss <-|+>nnnn" (con l'UTC, ossia il <-|+>nnnn, locale)
	GMT_TZ,			// "Day, dd Mon yyyy hh:mm:ss <-|+>nnnn TZ" (con l'UTC, ossia il <-|+>nnnn, locale, dove TZ e' l'identificativo di tre caratteri per l'UTC)
	UTC_TIME,			// nessun formato, solo per distinguere tra UTC e local time
	LOCAL_TIME,		// nessun formato, solo per distinguere tra UTC e local time
	UNKNOW_DATEFORMAT	// ???
};

/*
	TIMEFORMAT
	tipo per i formati dell'ora
*/
enum TIMEFORMAT {
	HHMMSS = 0,		// "hh:mm:ss"
	HHMMSS_AMPM,		// "hh:mm:ss <AM|PM>"
	HHMMSS_SHORT,		// "hhmmss"
	HHMMSS_GMT_SHORT,	// "hh:mm:ss" (assumendo GMT, ossia convertendo l'UTC in GMT)
	HHMMSS_GMT,		// "hh:mm:ss <-|+>nnnn" (con l'UTC, ossia il <-|+>nnnn, locale)
	HHMMSS_GMT_TZ,		// "hh:mm:ss <-|+>nnnn TZ" (con l'UTC, ossia il <-|+>nnnn, locale, dove TZ e' l'identificativo di tre caratteri per l'UTC)
	UNKNOW_TIMEFORMAT	// ???
};

/*
	CDateTime
*/
class CDateTime
{
public:
	// operatori
	CDateTime& operator=(const CDateTime& d);
	CDateTime& operator=(const CDateTime* d);

	// costruttori
	CDateTime(
			DATEFORMAT = UNKNOW_DATEFORMAT,
			TIMEFORMAT = UNKNOW_TIMEFORMAT,
			int dayofweek = -1,
			int day  = -1, int month = -1, int year = -1,
			int hour = -1, int min   = -1, int sec  = -1
			);

	CDateTime(const char* date,DATEFORMAT datefmt);

	// distruttore
	virtual ~CDateTime() {}
	
	// azzera l'oggetto
	void				Reset			(void);

	// impostano e ricavano i valori della struttura interna
	// le Set-Date/Time() possono impostare con i valori di sistema se chiamate senza parametri
	void				SetDate			(int dayofweek = -1,int day = -1,int month = -1,int year = -1);
	inline void		SetDayOfWeek		(int dayofweek)				{m_Date.dayofweek = dayofweek;}
	inline void		SetDay			(int day)						{m_Date.day = day;}
	inline void		SetMonth			(int month)					{m_Date.month = month;}
	inline void		SetYear			(int year)					{m_Date.year = year;}
	
	void				GetDate			(int& dayofweek,int& day,int& month,int& year);
	inline int		GetDayOfWeek		(void) const					{return(m_Date.dayofweek);}
	inline int		GetDay			(void) const					{return(m_Date.day);}
	inline int		GetMonth			(void) const					{return(m_Date.month);}
	inline int		GetYear			(void) const					{return(m_Date.year);}

	void				SetTime			(int hour = -1,int min = -1,int sec = -1);
	inline void		SetHour			(int hour)					{m_Time.hour = hour;}
	inline void		SetMin			(int min)						{m_Time.min = min;}
	inline void		SetSec			(int sec)						{m_Time.sec = sec;}
	
	void				GetTime			(int& hour,int& min,int& sec);
	inline int		GetHour			(void) const					{return(m_Time.hour);}
	inline int		GetMin			(void) const					{return(m_Time.min);}
	inline int		GetSec			(void) const					{return(m_Time.sec);}

	// formato
	inline void		SetCentury		(bool bOnOff)					{m_bSetCentury = bOnOff;}
	inline void		SetDateFormat		(DATEFORMAT format)				{m_Date.format = format;}
	inline DATEFORMAT	GetDateFormat		(void) const					{return(m_Date.format);}
	const char*		GetFormattedDate	(BOOL getsysdate = TRUE);
	inline void		SetTimeFormat		(TIMEFORMAT format)				{m_Time.format = format;}
	inline TIMEFORMAT	GetTimeFormat		(void) const					{return(m_Time.format);}
	const char*		GetFormattedTime	(BOOL getsystime = TRUE);

	const char*		Get12HourTime		(BOOL getsystime = TRUE);
	const char*		GetElapsedTime		(double seconds);
	
	static int		GetJulianDateDiff	(CDateTime& firstDate,CDateTime& secondDate);
	static void		GetJulianDateTimeDiff(CDateTime& firstDate,CDateTime& secondDate,int& nDays,long& nSeconds);

	// conversioni	
	const char*		ConvertDate		(DATEFORMAT fmtsrc,DATEFORMAT fmtdst,const char* pdate,const char* ptime);
	const char*		ConvertTime		(TIMEFORMAT fmtsrc,TIMEFORMAT fmtdst,const char* pdate,const char* ptime);

	// operazioni
	int				DaysInMonth		(int month,int year);
	void				ModifyDateTime		(DATETIMEOBJECT type,int qta);

	// time zone
	int				GetDSTZone		(void);
	long				GetTimeZoneDiff	(void);
	const char*		GetTimeZoneName	(void);
	const char*		GetDSTZoneName		(void);

	// impostano i campi della struct interna con data/ora di sistema
	const char*		GetSystemDate		(void);
	void				GetSystemDate		(int& day,int& month,int& year);
	const char*		GetSystemTime		(void);
	void				GetSystemTime		(int& hour,int& min,int& sec);

	// verifica se si tratta di un anno bisestile
	BOOL				IsLeapYear		(int year);
	
	void				LoadFromString		(const char* pdate,DATEFORMAT datefmt = GMT_SHORT,TIMEFORMAT timefmt = HHMMSS_GMT_SHORT);

private:
	// copia un oggetto sull'altro
	void				Copy				(CDateTime* date1,const CDateTime* date2);

	// ricavano data/ora del sistema
	const char*		GetOsDate			(void);
	const char*		GetOsTime			(void);
	
	struct DATE {
		DATEFORMAT format;						// formato
		char		datestr[MAX_DATE_STRING + 1];		// buffer interno usato per le conversioni (volatile)
		int		dayofweek;					// giorno della settimana (numerico)
		int		day;							// giorno
		int		month;						// mese
		int		year;						// anno
		char		dayofweekstr[MAX_DATETIME_BUF + 1];// giorno della settimana (stringa)
		char		daystr[MAX_DATETIME_BUF + 1];		// giorno
		char		monthstr[MAX_DATETIME_BUF + 1];	// mese
		char		yearstr[MAX_DATETIME_BUF + 1];	// anno
	};

	struct TIME {
		TIMEFORMAT format;						// formato
		char		timestr[MAX_TIME_STRING + 1];		// buffer interno usato per le conversioni (volatile)
		int		hour;						// ore, minuti, secondi
		int		min;
		int		sec;
		char		hourstr[MAX_DATETIME_BUF + 1];
		char		minstr[MAX_DATETIME_BUF + 1];
		char		secstr[MAX_DATETIME_BUF + 1];
	};

	DATE m_Date;
	TIME m_Time;
	bool m_bSetCentury;
};

#endif // _CDATETIME_H
