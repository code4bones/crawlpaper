/*
	Common.h
	"MPEG 1,2,3 decoder/MAPlay decoder"

	Riarrangiamento del codice originale (vedi sopra).
	Luca Piergentili, 07/06/03
	lpiergentili@yahoo.com
*/
#ifndef _COMMON_H
#define _COMMON_H 1

#include "audioconfig.h"
#ifdef MAPlay_MPEG_DECODER

#define _DIRECTX			1

#define NEW_DCT9			1

#define MPG_MD_STEREO		0
#define MPG_MD_JOINT_STEREO	1
#define MPG_MD_DUAL_CHANNEL	2
#define MPG_MD_MONO			3

struct gr_info_s {
	int		scfsi;
	unsigned	part2_3_length;
	unsigned	big_values;
	unsigned	scalefac_compress;
	unsigned	block_type;
	unsigned	mixed_block_flag;
	unsigned	table_select[3];
	unsigned	subblock_gain[3];
	unsigned	maxband[3];
	unsigned	maxbandl;
	unsigned	maxb;
	unsigned	region1start;
	unsigned	region2start;
	unsigned	preflag;
	unsigned	scalefac_scale;
	unsigned	count1table_select;
	double*	full_gain[3];
	double*	pow2gain;
};

struct bandInfoStruct {
	int	longIdx[23];
	int	longDiff[22];
	int	shortIdx[14];
	int	shortDiff[13];
};

struct III_sideinfo {
	unsigned	main_data_begin;
	unsigned	private_bits;
	struct {
		struct gr_info_s gr[2];
	} ch[2];
};

#endif // MAPlay_MPEG_DECODER
#endif // _COMMON_H
