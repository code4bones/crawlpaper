/*
	id3v1.h
	Classe per i tag MP3 (basata sul codice presente in FreeAmp).
	Luca Piergentili, 02/03/03
	lpiergentili@yahoo.com
*/
#ifndef ID3V1_H
#define ID3V1_H 1

#define ID3V1_ID_LEN		3
#define ID3V1_TITLE_LEN		30
#define ID3V1_ARTIST_LEN		30
#define ID3V1_ALBUM_LEN		30
#define ID3V1_YEAR_LEN		4
#define ID3V1_DATA_LEN		28

typedef struct id3v1_0 {
	char id		[3];
	char title	[30];
	char artist	[30];
	char album	[30];
	char year		[4];
	// v.1.0 30 + 1 = 31
	char comment	[30];
	char genre;
} id3v1_0;

typedef struct id3v1_1 {
	char id		[3];
	char title	[30];
	char artist	[30];
	char album	[30];
	char year		[4];
	// v.1.0 28 + 1 + 1 + 1 = 31
	char comment	[28];
	char zero;
	char track;
	char genre;
} id3v1_1;

typedef struct id3v1 {
	union {
		struct id3v1_0 v1_0;
		struct id3v1_1 v1_1;
	} id3;
} id3v1;

#define v1_0 id3.v1_0
#define v1_1 id3.v1_1

class ID3v1
{
public:
	ID3v1();
	virtual ~ID3v1() {}
	
	virtual int		Read		(const char* pMp3Name);

	inline const char*	GetID	(void) const {return(m_id3.v1_0.id);}
	inline const char*	GetTitle	(void) const {return(m_id3.v1_0.title);}
	inline const char*	GetArtist	(void) const {return(m_id3.v1_0.artist);}
	inline const char*	GetAlbum	(void) const {return(m_id3.v1_0.album);}
	inline const char*	GetYear	(void) const {return(m_id3.v1_0.year);}
	inline const char*	GetComment(void) const {return(m_id3.v1_0.comment);}
	inline int		GetTrack	(void) const {return(m_nTrack);}
	inline const char*	GetGenre	(void) const {return(m_pGenre);}

private:
	id3v1	m_id3;
	int		m_nTrack;
	char*	m_pGenre;
};

#endif // ID3V1_H
