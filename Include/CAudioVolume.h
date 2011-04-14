/*
	CAudioVolume.h
	Classe per l'impostazione del volume.
	Luca Piergentili, 22/08/03
	lpiergentili@yahoo.com
*/
#ifndef _CAUDIOVOLUME_H
#define _CAUDIOVOLUME_H 1

#include "window.h"

/*
	CAudioVolume
*/
class CAudioVolume : public CWnd
{
public:
	CAudioVolume();
	virtual ~CAudioVolume();

	inline void	SetStep					(long lValue) {if(lValue >= m_lMinLevel && lValue <= m_lMaxLevel) m_lStepValue = lValue;}
	inline long	GetStep					(void) const {return(m_lStepValue);}
	
	BOOL			Increase					(long lValue = -1L);
	BOOL			Decrease					(long lValue = -1L);

protected:
	void			OnDestroy					(void);
	LRESULT		OnMixerControlChange		(WPARAM wParam,LPARAM lParam);

private:
	BOOL			mixerInitialize			(void);
	BOOL			mixerUninitialize			(void);
	BOOL			mixerGetMasterVolumeControl	(void);
	BOOL			mixerGetMasterVolumeValue	(long &lValue);
	BOOL			mixerSetMasterVolumeValue	(long lValue);

	UINT			m_nMixersCount;
	HMIXER		m_hMixer;
	MIXERCAPS		m_stMixerCaps;
	long			m_lLevel;
	long			m_lMinLevel;
	long			m_lMaxLevel;
	long			m_lStepValue;
	DWORD		m_dwVolumeControlID;

	DECLARE_MESSAGE_MAP()
};

#endif // _CAUDIOVOLUME_H
