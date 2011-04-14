/*
	CWhyMicrosoftProgrammersSuckBallsThisWay
*/
#ifndef _CWHYMICROSOFTPROGRAMMERSSUCKBALLSTHISWAY_H
#define _CWHYMICROSOFTPROGRAMMERSSUCKBALLSTHISWAY_H 1

class CWhyMicrosoftProgrammersSuckBallsThisWay {
public:
	CWhyMicrosoftProgrammersSuckBallsThisWay() {}
	~CWhyMicrosoftProgrammersSuckBallsThisWay() {}

	virtual LPARAM SoTellMeWhy(WPARAM wParam = 0L,LPARAM lParam = 0L) = 0;
	virtual LPARAM IDontKnow(WPARAM wParam = 0L,LPARAM lParam = 0L) = 0;
};

#endif // _CWHYMICROSOFTPROGRAMMERSSUCKBALLSTHISWAY_H
