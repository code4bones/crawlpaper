/*
	CId3v1Tag.h
	Classe per i tag mp3 (ID3v1), implementata a partire dal codice di Gustav Munkby.
	Luca Piergentili, 07/08/03
	lpiergentili@yahoo.com

	ID3v1: 128 bytes
	----------------
	Field 		Length 		Description
	header		3 			"TAG"
	title 		30 			30 characters of the title
	artist 		30 			30 characters of the artist name
	album 		30 			30 characters of the album name
	year 		4 			A four-digit year
	comment		28[2] or 30	The comment.
	zero-byte[2]	1 			If a track number is stored, this byte contains a binary 0.
	track[2] 		1 			The number of the track on the album, or 0. Invalid, if previous byte is not a binary 0.
	genre 		1 			Index in a list of genres, or 255

	Extended tag (placed before the ID3v1 tag): 227 bytes
	-----------------------------------------------------
	Field 		Length 		Description
	header 		4 			"TAG+"
	title 		60 			Next 60 characters of the title (90 characters total)
	artist 		60 			Next 60 characters of the artist name
	album 		60 			Next 60 characters of the album name
	speed 		1 			0=unset, 1=slow, 2= medium, 3=fast, 4=hardcore
	genre 		30 			A free-text field for the genre
	start-time 	6 			the start of the music as mmm:ss
	end-time 		6 			the end of the music as mmm:ss
*/
#ifndef _CID3V1TAG_H
#define _CID3V1TAG_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"

#define ID3V1_TAG_SIZE			128
#define ID3V1_TAG_ZEROBYTE_OFS	125
#define ID3V1_TAG_TRACK_OFS		126
#define ID3V1_EXT_TAG_SIZE		227
#define ID3V1_HEADER_SIZE		3
#define ID3V1_TITLE_SIZE			30
#define ID3V1_ARTIST_SIZE		30
#define ID3V1_ALBUM_SIZE			30
#define ID3V1_YEAR_SIZE			4
#define ID3V1_COMMENT_SIZE		30
#define ID3V1_TRACK_SIZE			2
#define ID3V1_GENRE_SIZE			1

/*
	CId3v1Tag()
*/
class CId3v1Tag
{
public:
	CId3v1Tag(LPCSTR lpcszFileName = NULL);
	virtual ~CId3v1Tag();

	BOOL				Link			(LPCSTR lpcszFileName);
	void				Unlink		(void);
	BOOL				Update		(void);

	void				AutoSave		(BOOL bFlag)			{m_bSaveChangesWhenUnload = bFlag;}

	LPCSTR			GetTitle		(void)				{return(m_szTitle);}
	void				SetTitle		(LPCSTR lpcszTitle)		{strcpyn(m_szTitle,lpcszTitle,sizeof(m_szTitle));}

	LPCSTR			GetArtist		(void)				{return(m_szArtist);}
	void				SetArtist		(LPCSTR lpcszArtist)	{strcpyn(m_szArtist,lpcszArtist,sizeof(m_szArtist));}

	LPCSTR			GetAlbum		(void)				{return(m_szAlbum);}
	void				SetAlbum		(LPCSTR lpcszAlbum)		{strcpyn(m_szAlbum,lpcszAlbum,sizeof(m_szAlbum));}

	int				GetYear		(void)				{return(m_nYear);};
	void				SetYear		(int nYear)			{m_nYear = nYear;}

	LPCSTR			GetComment	(void)				{return(m_szComment);}
	void				SetComment	(LPCSTR lpcszComment)	{strcpyn(m_szComment,lpcszComment,sizeof(m_szComment));}

	int				GetTrack		(void)				{return((m_nVersion==(float)1.1) ? m_nTrackNumber : -1);}
	void				SetTrack		(int nTrackNumber);

	LPCSTR			GetGenre		(void);
	void				SetGenre		(LPCSTR lpcszGenre);

	int				GetGenreIndex	(void)				{return((int)m_nGenre);};
	void				SetGenreIndex	(int nGenre)			{m_nGenre = (unsigned char)(nGenre % 256);}

	float			GetVersion	(void)				{return(m_nVersion);}

	static LPCSTR		GetGenre		(int nIndex);

private:
	BOOL				SetTagsFromTag	(void);
	BOOL				UpdateTagFromTags(void);

	char				m_szTitle[ID3V1_TITLE_SIZE+1];
	char				m_szArtist[ID3V1_ARTIST_SIZE+1];
	char				m_szAlbum[ID3V1_ALBUM_SIZE+1];
	int				m_nYear;
	char				m_szComment[ID3V1_COMMENT_SIZE+1];
	int				m_nTrackNumber;
	unsigned char		m_nGenre;
	float			m_nVersion;

	char				m_szFileName[_MAX_PATH+1];
	char				m_ID3v1[ID3V1_TAG_SIZE];
	char				m_ID3v1ext[ID3V1_EXT_TAG_SIZE];
	BOOL				m_bSaveChangesWhenUnload;
};

#endif // _CID3V1TAG_H
