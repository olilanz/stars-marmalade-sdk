#ifndef __TOUCHRECORDER_H__
#define __TOUCHRECORDER_H__

#include <vector>
#include "IwGeomFVec2.h"
#include "s3e.h"

#define TOUCH_REC_MAX_RESOLUTION	40		// samples / second

class TouchRecorder {
private:
	uint64 m_ulStartTimeMS;

	uint16 m_uiLastSample;
	std::vector<CIwFVec2> m_xPoints;

public:
	TouchRecorder();
	
public: 
	bool IsRecording();
	
	void StartRecording();
	void Record(const CIwFVec2& pos);
	void EndRecording();
	
	uint16 GetSampleCount();
	CIwFVec2* GetSamples();
};

#endif
