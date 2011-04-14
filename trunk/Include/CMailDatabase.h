/*
	CMailDatabase.h
	Implementazione delle tabelle per i servizi SMTP/POP3.
	La classe per il servizio (CMailService) fornisce le funzioni di base per l'accesso ai dati
	attraverso le tabelle qui (CMailDatabase) definite (derivando da CTable).
	Luca Piergentili, 14/07/99
	lpiergentili@yahoo.com
*/
#ifndef _CMAILDATABASE_H
#define _CMAILDATABASE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strcpyn.h"
#include "strsetn.h"
#include "strsetn.h"
#ifdef _WINDOWS
	#include "window.h"
	#ifdef WIN32_MFC
		#include "CMutexEx.h"
	#endif
#else
	#include "typedef.h"
#endif
#include "CDateTime.h"
#include "CBase.h"
#include "CTable.h"

/*
	users.db
*/

// tabella
#define USERS_TABLE			"users.db"
#define USERS_EMAIL_LEN		64
#define USERS_CODE_LEN		5
#define USERS_OFFICE_LEN		128
#define USERS_ADDRESS_LEN	128
#define USERS_ZIP_LEN		5
#define USERS_CITY_LEN		64
#define USERS_PROVINCE_LEN	2
#define USERS_STATE_LEN		2
#define USERS_PHONE_1_LEN	9
#define USERS_PHONE_2_LEN	9
#define USERS_FAX_LEN		9
#define USERS_USR_LEN		64
#define USERS_PSW_LEN		64
#define USERS_CRC_LEN		5
#define USERS_DATE_UP_LEN	10
#define USERS_RECORD_LENGTH	(USERS_EMAIL_LEN+USERS_CODE_LEN+USERS_OFFICE_LEN+USERS_ADDRESS_LEN+USERS_ZIP_LEN+USERS_CITY_LEN+USERS_PROVINCE_LEN+USERS_STATE_LEN+USERS_PHONE_1_LEN+USERS_PHONE_2_LEN+USERS_FAX_LEN+USERS_USR_LEN+USERS_PSW_LEN+USERS_CRC_LEN+USERS_DATE_UP_LEN)

// indici
#define USERS_INDEX_EMAIL	"users.email.idx"
#define USERS_IDX_EMAIL		0

/*
	CUsersTable
*/
class CUsersTable : public CTable
{
private:
	// definizione del record
	struct RECORD {
		char			email	[USERS_EMAIL_LEN + 1];
		char			code		[USERS_CODE_LEN + 1];
		char			office	[USERS_OFFICE_LEN + 1];
		char			address	[USERS_ADDRESS_LEN + 1];
		int			zip;
		char			city		[USERS_CITY_LEN + 1];
		char			province	[USERS_PROVINCE_LEN + 1];
		char			state	[USERS_STATE_LEN + 1];
		unsigned long	phone_1;
		unsigned long	phone_2;
		unsigned long	fax;
		char			usr		[USERS_USR_LEN + 1];
		char			psw		[USERS_PSW_LEN + 1];
		int			crc;
		CDateTime		date_up;
	};

	char			table_name[_MAX_PATH + _MAX_FNAME + 1];
	char			idx_email[_MAX_PATH + _MAX_FNAME + 1];
	CBASE_TABLE*	table_struct;
	CBASE_INDEX*	idx_struct;
	RECORD		record;
	char			record_string[USERS_RECORD_LENGTH + 1];

public:
	CUsersTable(BOOLEAN bOpenTable,const char* pPathname);
	virtual ~CUsersTable();

	// derivate virtuali
	const char*				GetClassName(void)						{return("CUsersTable");}
	const char*				GetTableName(void)						{return(USERS_TABLE);}
	const char*				GetStaticTableName(void)					{return(USERS_TABLE);}
	const char*				GetTablePathName(void)					{return(table_name);}
	const CBASE_TABLE*			GetTableStruct(void)					{return(table_struct);}
	const CBASE_INDEX*			GetIndexStruct(void)					{return(idx_struct);}
	const int					GetRecordLength(void)					{return(USERS_RECORD_LENGTH);}
	const char*				GetRecordAsString(void);
	void						ResetMemvars(void);
	void						GatherMemvars(void);
	void						ScatterMemvars(BOOL = TRUE);

	// campi
	inline const char*			GetField_Email(void)					{return(record.email);}
	inline const char*			GetField_Code(void)						{return(record.code);}
	inline const char*			GetField_Office(void)					{return(record.office);}
	inline const char*			GetField_Address(void)					{return(record.address);}
	inline const int			GetField_Zip(void)						{return(record.zip);}
	inline const char*			GetField_City(void)						{return(record.city);}
	inline const char*			GetField_Province(void)					{return(record.province);}
	inline const char*			GetField_State(void)					{return(record.state);}
	inline const unsigned long	GetField_Phone_1(void)					{return(record.phone_1);}
	inline const unsigned long	GetField_Phone_2(void)					{return(record.phone_2);}
	inline const unsigned long	GetField_Fax(void)						{return(record.fax);}
	inline const char*			GetField_Usr(void)						{return(record.usr);}
	inline const char*			GetField_Psw(void)						{return(record.psw);}
	inline const int			GetField_Crc(void)						{return(record.crc);}
	inline const CDateTime&		GetField_Date_up(void)					{return(record.date_up);}

	// campi
	inline void				PutField_Email(const char* value)			{strcpyn(record.email,value,USERS_EMAIL_LEN+1);}
	inline void				PutField_Code(const char* value)			{strcpyn(record.code,value,USERS_CODE_LEN+1);}
	inline void				PutField_Office(const char* value)			{strcpyn(record.office,value,USERS_OFFICE_LEN+1);}
	inline void				PutField_Address(const char* value)		{strcpyn(record.address,value,USERS_ADDRESS_LEN+1);}
	inline void				PutField_Zip(const int value)				{record.zip = value;}
	inline void				PutField_City(const char* value)			{strcpyn(record.city,value,USERS_CITY_LEN+1);}
	inline void				PutField_Province(const char* value)		{strcpyn(record.province,value,USERS_PROVINCE_LEN+1);}
	inline void				PutField_State(const char* value)			{strcpyn(record.state,value,USERS_STATE_LEN+1);}
	inline void				PutField_Phone_1(const unsigned long value)	{record.phone_1 = value;}
	inline void				PutField_Phone_2(const unsigned long value)	{record.phone_2 = value;}
	inline void				PutField_Fax(const unsigned long value)		{record.fax = value;}
	inline void				PutField_Usr(const char* value)			{strcpyn(record.usr,value,USERS_USR_LEN+1);}
	inline void				PutField_Psw(const char* value)			{strcpyn(record.psw,value,USERS_PSW_LEN+1);}
	inline void				PutField_Crc(const int value)				{record.crc = value;}
	inline void				PutField_Date_up(const CDateTime& value)	{record.date_up = value;}
};

/*
	exchange.db
*/

// tabella
#define EXCHANGE_TABLE			"exchange.db"
#define EXCHANGE_EMAIL_LEN		USERS_EMAIL_LEN
#define EXCHANGE_EXCHANGE_LEN		USERS_EMAIL_LEN
#define EXCHANGE_RECORD_LENGTH	(EXCHANGE_EMAIL_LEN+EXCHANGE_EXCHANGE_LEN)

// indici
#define EXCHANGE_INDEX_EMAIL		"exchange.email.idx"
#define EXCHANGE_INDEX_EXCHANGE	"exchange.exchange.idx"
#define EXCHANGE_IDX_EMAIL		0
#define EXCHANGE_IDX_EXCHANGE		1

/*
	CExchangeTable
*/
class CExchangeTable : public CTable
{
private:
	// definizione del record
	struct RECORD {
		char	email	[EXCHANGE_EMAIL_LEN + 1];
		char	exchange	[EXCHANGE_EXCHANGE_LEN + 1];
	};

	char			table_name[_MAX_PATH + _MAX_FNAME + 1];
	char			idx_email[_MAX_PATH + _MAX_FNAME + 1];
	char			idx_exchange[_MAX_PATH + _MAX_FNAME + 1];
	CBASE_TABLE*	table_struct;
	CBASE_INDEX*	idx_struct;
	RECORD		record;
	char			record_string[EXCHANGE_RECORD_LENGTH + 1];

public:
	CExchangeTable(BOOLEAN bOpenTable,const char* pPathname);
	virtual ~CExchangeTable();

	// derivate virtuali
	const char*		GetClassName(void)					{return("CExchangeTable");}
	const char*		GetTableName(void)					{return(EXCHANGE_TABLE);}
	const char*		GetStaticTableName(void)				{return(EXCHANGE_TABLE);}
	const char*		GetTablePathName(void)				{return(table_name);}
	const CBASE_TABLE*	GetTableStruct(void)				{return(table_struct);}
	const CBASE_INDEX*	GetIndexStruct(void)				{return(idx_struct);}
	const int			GetRecordLength(void)				{return(EXCHANGE_RECORD_LENGTH);}
	const char*		GetRecordAsString(void);
	inline void		ResetMemvars(void)					{memset(&record,'\0',sizeof(RECORD));}
	void				GatherMemvars(void);
	void				ScatterMemvars(BOOL = TRUE);

	// campi
	inline const char*	GetField_Email(void)				{return(record.email);}
	inline const char*	GetField_Exchange(void)				{return(record.exchange);}

	// campi
	inline void		PutField_Email(const char* value)		{strcpyn(record.email,value,EXCHANGE_EMAIL_LEN+1);}
	inline void		PutField_Exchange(const char* value)	{strcpyn(record.exchange,value,EXCHANGE_EXCHANGE_LEN+1);}
};

/*
	mailusers.db
*/

// tabella
#define MAILUSERS_TABLE			"mailusers.db"
#define MAILUSERS_EMAIL_LEN		64
#define MAILUSERS_USR_LEN		64
#define MAILUSERS_PSW_LEN		64
#define MAILUSERS_DATE_UP_LEN		10
#define MAILUSERS_TIME_UP_LEN		8
#define MAILUSERS_MAILBOX_LEN		_MAX_PATH+_MAX_FNAME+1
#define MAILUSERS_LAST_MESSAGE	10
#define MAILUSERS_RECORD_LENGTH	(MAILUSERS_EMAIL_LEN+MAILUSERS_USR_LEN+MAILUSERS_PSW_LEN+MAILUSERS_DATE_UP_LEN+MAILUSERS_TIME_UP_LEN+MAILUSERS_MAILBOX_LEN+MAILUSERS_LAST_MESSAGE)

// indici
#define MAILUSERS_INDEX_EMAIL		"mailusers.email.idx"
#define MAILUSERS_INDEX_USR		"mailusers.usr.idx"
#define MAILUSERS_INDEX_PSW		"mailusers.psw.idx"
#define MAILUSERS_IDX_EMAIL		0
#define MAILUSERS_IDX_USR		1
#define MAILUSERS_IDX_PSW		2

/*
	CMailUsersTable
*/
class CMailUsersTable : public CTable
{
private:
	// definizione del record
	struct RECORD {
		char			email	[MAILUSERS_EMAIL_LEN + 1];
		char			usr		[MAILUSERS_USR_LEN + 1];
		char			psw		[MAILUSERS_PSW_LEN + 1];
		CDateTime		date_up;
		char			time_up	[MAILUSERS_TIME_UP_LEN + 1];
		char			mailbox	[MAILUSERS_MAILBOX_LEN + 1];
		unsigned long	last_message_number;
	};

	char			table_name[_MAX_PATH + _MAX_FNAME + 1];
	char			idx_email[_MAX_PATH + _MAX_FNAME + 1];
	char			idx_usr[_MAX_PATH + _MAX_FNAME + 1];
	char			idx_psw[_MAX_PATH + _MAX_FNAME + 1];
	CBASE_TABLE*	table_struct;
	CBASE_INDEX*	idx_struct;
	RECORD		record;
	char			record_string[MAILUSERS_RECORD_LENGTH + 1];

public:
	CMailUsersTable(BOOLEAN bOpenTable,const char* pPathname);
	virtual ~CMailUsersTable();

	// derivate virtuali
	const char*				GetClassName(void)						{return("CMailUsersTable");}
	const char*				GetTableName(void)						{return(MAILUSERS_TABLE);}
	const char*				GetStaticTableName(void)					{return(MAILUSERS_TABLE);}
	const char*				GetTablePathName(void)					{return(table_name);}
	const CBASE_TABLE*			GetTableStruct(void)					{return(table_struct);}
	const CBASE_INDEX*			GetIndexStruct(void)					{return(idx_struct);}
	const int					GetRecordLength(void)					{return(MAILUSERS_RECORD_LENGTH);}
	const char*				GetRecordAsString(void);
	void						ResetMemvars(void);
	void						GatherMemvars(void);
	void						ScatterMemvars(BOOL = TRUE);

	// campi
	inline const char*			GetField_Email(void)			{return(record.email);}
	inline const char*			GetField_Usr(void)				{return(record.usr);}
	inline const char*			GetField_Psw(void)				{return(record.psw);}
	inline const CDateTime&		GetField_Date_up(void)			{return(record.date_up);}
	inline const char*			GetField_Time_up(void)			{return(record.time_up);}
	inline const char*			GetField_Mailbox(void)			{return(record.mailbox);}
	inline const unsigned long	GetField_LastMessageNumber(void)	{return(record.last_message_number);}

	// campi
	inline void				PutField_Email(const char* value)				{strcpyn(record.email,value,MAILUSERS_EMAIL_LEN+1);}
	inline void				PutField_Usr(const char* value)					{strcpyn(record.usr,value,MAILUSERS_USR_LEN+1);}
	inline void				PutField_Psw(const char* value)					{strcpyn(record.psw,value,MAILUSERS_PSW_LEN+1);}
	inline void				PutField_Date_up(const CDateTime& value)			{record.date_up = value;}
	inline void				PutField_Time_up(const char* value)				{strcpyn(record.time_up,value,MAILUSERS_TIME_UP_LEN+1);}
	inline void				PutField_Mailbox(const char* value)				{strcpyn(record.mailbox,value,MAILUSERS_MAILBOX_LEN+1);}
	inline void				PutField_LastMessageNumber(unsigned long value)		{record.last_message_number = value;}
};

/*
	mailbox.db
*/

// tabella
#define MAILBOX_TABLE			"mailbox.db"
#define MAILBOX_FROM_LEN			MAILUSERS_EMAIL_LEN
#define MAILBOX_TO_LEN			MAILUSERS_EMAIL_LEN
#define MAILBOX_OBJECT_LEN		128
#define MAILBOX_RECV_LEN			MAX_DATE_STRING
#define MAILBOX_SEND_LEN			MAX_DATE_STRING
#define MAILBOX_FILENAME_LEN		_MAX_PATH
#define MAILBOX_FILESIZE_LEN		10
#define MAILBOX_ATTACHNAME_LEN	_MAX_PATH
#define MAILBOX_ATTACHSIZE_LEN	10
#define MAILBOX_STATE_LEN		1
#define MAILBOX_RECORD_LENGTH		(MAILBOX_FROM_LEN+MAILBOX_TO_LEN+MAILBOX_OBJECT_LEN+MAILBOX_RECV_LEN+MAILBOX_SEND_LEN+MAILBOX_FILENAME_LEN+MAILBOX_FILESIZE_LEN+MAILBOX_ATTACHNAME_LEN+MAILBOX_ATTACHSIZE_LEN+MAILBOX_STATE_LEN)

// indici
#define MAILBOX_INDEX_FILENAME	"mailbox.filename.idx"
#define MAILUSERS_IDX_FILENAME	0

/*
	CMailboxTable
*/
class CMailboxTable : public CTable
{
private:
	// definizione del record
	struct RECORD {
		char			from		[MAILBOX_FROM_LEN + 1];
		char			to		[MAILBOX_TO_LEN + 1];
		char			object	[MAILBOX_OBJECT_LEN + 1];
		char			recv		[MAILBOX_RECV_LEN + 1];
		char			send		[MAILBOX_SEND_LEN + 1];
		char			filename	[MAILBOX_FILENAME_LEN + 1];
		unsigned long	filesize;
		char			attachname[MAILBOX_ATTACHNAME_LEN + 1];
		unsigned long	attachsize;
		int			state;
	};

	char			table_name[_MAX_PATH + _MAX_FNAME + 1];
	char			idx_filename[_MAX_PATH + _MAX_FNAME + 1];
	CBASE_TABLE*	table_struct;
	CBASE_INDEX*	idx_struct;
	RECORD		record;
	char			record_string[MAILBOX_RECORD_LENGTH + 1];

public:
	CMailboxTable(BOOLEAN bOpenTable,const char* pPathname);
	virtual ~CMailboxTable();

	// derivate virtuali
	const char*			GetClassName(void)							{return("CMailboxTable");}
	const char*			GetTableName(void)							{return(MAILBOX_TABLE);}
	const char*			GetStaticTableName(void)						{return(MAILBOX_TABLE);}
	const char*			GetTablePathName(void)						{return(table_name);}
	const CBASE_TABLE*		GetTableStruct(void)						{return(table_struct);}
	const CBASE_INDEX*		GetIndexStruct(void)						{return(idx_struct);}
	const int				GetRecordLength(void)						{return(MAILBOX_RECORD_LENGTH);}
	const char*			GetRecordAsString(void);
	inline void			ResetMemvars(void)							{memset(&record,'\0',sizeof(RECORD));}
	void					GatherMemvars(void);
	void					ScatterMemvars(BOOL = TRUE);

	// campi
	inline const char*		GetField_From(void)							{return(record.from);}
	inline const char*		GetField_To(void)							{return(record.to);}
	inline const char*		GetField_Object(void)						{return(record.object);}
	inline const char*		GetField_Recv(void)							{return(record.recv);}
	inline const char*		GetField_Send(void)							{return(record.send);}
	inline const char*		GetField_Filename(void)						{return(record.filename);}
	inline unsigned long	GetField_Filesize(void)						{return(record.filesize);}
	inline const char*		GetField_Attachname(void)					{return(record.attachname);}
	inline unsigned long	GetField_Attachsize(void)					{return(record.attachsize);}
	inline int			GetField_State(void)						{return(record.state);}

	// campi
	inline void			PutField_From(const char* value)				{strcpyn(record.from,value,MAILBOX_FROM_LEN+1);}
	inline void			PutField_To(const char* value)				{strcpyn(record.to,value,MAILBOX_TO_LEN+1);}
	inline void			PutField_Object(const char* value)				{strcpyn(record.object,value,MAILBOX_OBJECT_LEN+1);}
	inline void			PutField_Recv(const char* value)				{strcpyn(record.recv,value,MAILBOX_RECV_LEN+1);}
	inline void			PutField_Send(const char* value)				{strcpyn(record.send,value,MAILBOX_SEND_LEN+1);}
	inline void			PutField_Filename(const char* value)			{strcpyn(record.filename,value,MAILBOX_FILENAME_LEN+1);}
	inline void			PutField_Filesize(const unsigned long value)		{record.filesize = value;}
	inline void			PutField_Attachname(const char* value)			{strcpyn(record.attachname,value,MAILBOX_ATTACHNAME_LEN+1);}
	inline void			PutField_Attachsize(const unsigned long value)	{record.attachsize = value;}
	inline void			PutField_State(const int value)				{record.state = value;}
};

#endif // _CMAILDATABASE_H
