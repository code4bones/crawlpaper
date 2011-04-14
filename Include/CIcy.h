/*
	CIcy.h
	Classe base per il protocollo ICY (SDK/MFC).
	Luca Piergentili, 02/09/03
	lpiergentili@yahoo.com


	http://forums.radiotoolbox.com/viewtopic.php?t=74
	==========================================================
	Post subject: The SHOUTcast Streaming Standard [Technical]
	PostPosted: Wed Feb 20, 2002 3:00 pm 
	Offline
	Will work for food (Administrator)
	User avatar

	Joined: Mon Jan 14, 2002 1:48 am
	Posts: 2392
	Location: Next Door 	
	Intro
	Not much is available about how our streaming technology works, more specifically MP3 streaming. When you get right down to it, it is actually very simple to understand. You will especially understand it if you know anything about HTTP's Protocol, which is simply a communication method for a client to a server. This article is mainly geared for those hard core into this stuff or those who want to develop applications similar to those utilized at this site.

	Tools used
	The tools used for streaming are quite simple:

	    * Source (usually a dsp in a player)
	    * Server (dispatches the source's mp3 stream to the client)
	    * Client (is used to listen to the audio coming down from the server


	Source to Server
	In order for a server to allow connections from a client, it needs a source. When a source connection is made then the server will pass the data along to the clients when they connect.

	The dialog goes something like this (I will use SHOUTcast as the example)

	1. The source makes a connection to the service port (shoutcast's is the port +1)
	2. The source then sends the password like so password\r\n
	3. If the password is correct, the server will reply with OK2\r\nicy-caps:11\r\n\r\n, this basically informs the source that the server has authorized the dsp to be the source and it is ready for data. If the password is incorrect, the server sends invalid password\r\n.
	4. If the source receives the OK2, it then begins sending information about the stream to the server. Usually in this form:

	Code:
	icy-name:Unnamed Server\r\n
	icy-genre:Unknown Genre\r\n
	icy-pub:1\r\n
	icy-br:56\r\n
	icy-url:http://www.shoutcast.com\r\n
	icy-irc:%23shoutcast\r\n
	icy-icq:0\r\n
	icy-aim:N%2FA\r\n
	\r\n


	Then The source will begin sending the mp3 encoded stream
	* icy-name is the name of the stations
	* icy-genre is the genre that the station resides in
	* icy-pub is basically a switch to either allow the server to publish itself in the directory or not (1 meaning yes and 0 meaning no)
	* icy-br is the bitrate of the stream
	* icy-url is the homepage for the stream
	* icy-irc is yp shoutcast specific (used for contact information)
	* icy-icq is yp shoutcast specific (used for contact information)
	* icy-aim is yp shoutcast specific (used for contact information)

	You can also pass this optional data:
	Code:
	content-type: mime/type\r\n
	icy-reset: 1\r\n
	icy-prebuffer: ??\r\n


	* content-type is the data type to expect from this stream. (HTTP spec header)
	* icy-reset tells the server whether it should clear out the buffer. (necessary for NSV/NSA streams.
	* icy-prebuffer, we aren't quite certain what this is for, how to use it or even whether it works, but it exists.

	The optional params are not necessarily passed to the client, content-type is of course but as for the others it is not clear.

	This is just a simple walk through of how the source communicates with the server. No other information is passed on this port as far as I am aware.

	Title streaming from source to server
	This is a simple one, the server receives the title of the song and the URL of the page simply by having the source make the URL call

	Code:
	http://www.host.com:portnumber/admin.cgi?pass=Server%20Password&mode=updinfo&song=Song%20Goes%20here&url=http://someurl.com


	When this gets called by the source or a browser even, the title of the song changes in the clients which support shoutcast style title streaming. This communication always happens on the public port (by default 8000) never on the service port as it is used for strictly sending the stream to the server.

	You also must make sure that when you make your HTTP calls that it comes from a browser or program that specifies the User-Agent: header as Mozilla.

	Client to Server
	The Client to Server communication is handle in a similar fashion to the way that a browser communicates with a webpage server. This is known as the HTTP protocol. However SHOUTcast and icecast do not handle in exactly the same manner, the headers are different. I have yet to pin point exactly what is so different. I think it may have something to do with the notification error, as it is HTTP/1.0 200 OK on all web servers using HTTP, this may confuse some clients and causes the headers to not exist.
	1. The client connects to the server and sends information about itself, if it can handle title streaming it sends and extra field like so:

	Code:
	icy-metadata:1\r\n


	In addition to the normal headers sent. This tag signifies that the client has the ability to stream the title streaming tags from the stream, therefore the server will send the extra title information, if this were not possible, some clients would hiccup when the title information is sent
	2. The server then responds with

	Code:
	ICY 200 OK\r\n (signifying that the server was successful)

	icy-notice1:<BR>This stream requires <a href="http://www.winamp.com/">Winamp</a><BR> (redundant notice)

	icy-notice2:SHOUTcast Distributed Network Audio Server/posix v1.x.x<BR> (tells the client what server it is and version)SHOUTcast Specific

	icy-name:Unnamed Server\r\n (Name of the server)

	icy-genre:Unknown Genre\r\n (what genre the server falls under)

	icy-url:http://www.shoutcast.com\r\n (homepage for the server)

	Content-Type:audio/mpeg\r\n (Content type of the stream to follow)

	icy-pub:1\r\n (whether the server is public or not)

	icy-br:56\r\n (bitrate of the server)

	icy-metaint:8192\r\n (if icy-metadata:1 was signified this was shown I will discuss this further later)

	\r\n (end of header)


	3. At this point the server begins sending the audio data.

	SHOUTcast Meta Title Streaming
	Earlier we discussed how the server gets the title of the song from the source, but we didn't quite get into how the client gets the title of the song.

	When the client signifies that it is title streaming compatible, the shoutcast server adds an extra header tag set like so
	Code:
	icy-metaint:8192\r\n

	this tells the client exactly how many bytes of data to read out of the stream before it can expect the beginning of the Meta-Data (which is where the title is stored) It also always starts counting at the beginning of the stream (not the header)
	After this the client then reads 1 byte, this byte tells the client how large the Meta-Data Tag is divided by 16, so if the byte was 4 then the client would know that the meta-data tag was 64 bytes long. But, you ask, not all titles are going to equal 64 bytes or 48 bytes etc...? Well the simple answer is that SHOUTcast places blanks or "\0" in the unused space until it equals the length, after that is read, then it is back to the mpeg data to start the process all over again.

	Pretty simple huh?

	In Closing
	I am sure that this technology will change, and I will try my best to keep this article up to the specs as I know them. If you find anything incorrect in this article, or any oversights, then please email me

	Feel free to leave a comment if you feel that I have missed something. Do not reply with questions. Questions should go in the Audio Streaming forum. Questions will be split from this thread and moved to appropriate forums.


	Last edited by Jay on Mon Jan 16, 2006 10:41 am, edited 6 times in total. 
*/
#ifndef _CICY_H
#define _CICY_H 1

#include "window.h"
#include "win32api.h"
#include "CUrl.h"
#include "CHttp.h"
#include "CSock.h"

#define ICY_MAX_STATION		_MAX_FILEPATH
#define ICY_MAX_GENRE		_MAX_FILEPATH
#define ICY_MAX_URL			MAX_URL
#define ICY_MAX_CONTENT		HTTP_MAX_CONTENT
#define ICY_MAX_NOTICE		256

#define ICY_CODE_OK			HTTP_CODE_OK			// 200
#define ICY_CODE_SERVER_FULL	HTTP_CODE_BAD_REQUEST	// 400
#define ICY_CODE_FORBIDDEN	HTTP_CODE_FORBIDDEN		// 403

/*
	ICYDATA
*/
struct ICYDATA {
	char	station		[ICY_MAX_STATION+1];
	char	genre		[ICY_MAX_GENRE+1];
	char	url			[ICY_MAX_URL+1];
	char	contenttype	[ICY_MAX_CONTENT+1];
	int	metaint;
	int	bitrate;
	char	notice		[ICY_MAX_NOTICE+1];
	char	noticeinfo	[ICY_MAX_NOTICE+1];
	int	code;
};

/*
	CIcy
*/
class CIcy : public CUrl, public CSock
{
public:
	CIcy(LPCSTR pUrl = NULL,int nPort = HTTP_DEFAULT_PORT);
	virtual ~CIcy();

	void			Reset			(void);
	SOCKET		Open				(LPCSTR pUrl = NULL,int nPort = HTTP_DEFAULT_PORT,int icydata = 1);
	inline void	SetUserAgent		(LPCSTR lpcszUserAgent = NULL) {if(m_szUserAgent) strcpyn(m_szUserAgent,lpcszUserAgent,sizeof(m_szUserAgent)); else memset(m_szUserAgent,'\0',sizeof(m_szUserAgent));}
	void			Close			(void);

	// proxy
	inline LPCSTR	GetProxyAddress	(void)					{return(m_Proxy.address[0]!='\0' ? m_Proxy.address : NULL);}
	inline void	SetProxyAddress	(LPCSTR lpcszProxyAddress)	{strcpyn(m_Proxy.address,lpcszProxyAddress,HOSTNAME_SIZE+1);}
	inline UINT	GetProxyPortNumber	(void)					{return(m_Proxy.port);}
	inline void	SetProxyPortNumber	(UINT nProxyPortNumber)		{m_Proxy.port = nProxyPortNumber;}
	inline LPCSTR	GetProxyUser		(void)					{return(m_Proxy.user);}
	inline void	SetProxyUser		(LPCSTR lpcszUser)			{strcpyn(m_Proxy.user,lpcszUser,sizeof(m_Proxy.user));}
	inline LPCSTR	GetProxyPassword	(void)					{return(m_Proxy.password);}
	inline void	SetProxyPassword	(LPCSTR lpcszPassword)		{strcpyn(m_Proxy.password,lpcszPassword,sizeof(m_Proxy.password));}
	inline LPCSTR	GetProxyAuth		(void)					{return(m_Proxy.auth);}
	inline void	SetProxyAuth		(LPCSTR lpcszAuth)			{strcpyn(m_Proxy.auth,lpcszAuth,sizeof(m_Proxy.auth));}

	// imposta l'intero blocco
	void			GetIcyData		(ICYDATA* pIcyData);

	// ricava campo per campo
	inline LPCSTR	GetContentType		(void) {return(GetHeaderValue("Content-Type:"));}
	inline LPCSTR	GetAudioInfo		(void) {return(GetHeaderValue("ice-audio-info"));}
	inline int	GetBitRate		(void) {return(atoi(GetHeaderValue("icy-br")));}
	inline LPCSTR	GetDescription		(void) {return(GetHeaderValue("icy-description"));}
	inline LPCSTR	GetGenre			(void) {return(GetHeaderValue("icy-genre"));}
	inline int	GetMetaInterval	(void) {return(atoi(GetHeaderValue("icy-metaint")));}
	inline LPCSTR	GetStationName		(void) {return(GetHeaderValue("icy-name"));}
	inline LPCSTR	GetNotice			(void) {return(GetHeaderValue("icy-notice1"));}
	inline LPCSTR	GetNoticeInfo		(void) {return(GetHeaderValue("icy-notice2"));}
	inline LPCSTR	GetPub			(void) {return(GetHeaderValue("icy-pub"));}
	inline LPCSTR	GetUrl			(void) {return(GetHeaderValue("icy-url"));}
	inline LPCSTR	GetServer			(void) {return(GetHeaderValue("Server:"));}
	inline LPCSTR	GetLocation		(void) {return(GetHeaderValue("Location:"));}
	
	DWORD		GetLastError		(void);
	LPCSTR		GetLastErrorString	(void);

private:
	int			ParseHeader		(LPCSTR pIcyHeaderData);
	LPCSTR		GetHeaderValue		(LPCSTR lpcszFieldName);
	int			GetHeaderValue		(LPCSTR pIcyHeaderData,LPCSTR lpcszFieldName,LPSTR szValue,int cbValue);

	SOCKET		m_Socket;
	char			m_szFirstChunk[4096];
	int			m_nFirstChunkLen;
	char			m_szUrl[MAX_URL+1];
	char			m_szHost[_MAX_PATH+1];
	char			m_szDir[_MAX_PATH+1];
	char			m_szFile[_MAX_PATH+1];
	int			m_nPort;
	PROXY		m_Proxy;
	char			m_szUserAgent[HTTP_MAX_USERAGENT];
	char			m_szLocation[_MAX_PATH+1];
	int			m_nResponse;
	DWORD		m_dwLastError;
	char			m_szLastError[_MAX_PATH+1];
	CHttpHeaderList m_listICYHeaders;
};

#endif // _CICY_H
