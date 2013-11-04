#ifndef __TITLESCREEN_H__
#define __TITLESCREEN_H__

#include "Page.h"
#include "Button.h"
#include "SocialPanel.h"
#include "OptionsPanel.h"
#include "IwList.h"
#include "Body.h"
#include "Camera.h"

#include "s3eGyroscope.h"

class TitleScreen : public Page {
private:
	Camera m_xCamera;

	Texture* m_pxBackground;
	Button m_xButtonTitle;

	OptionsPanel m_xPanelOptions;
	SocialPanel m_xPanelSocial;

	s3eGyroscopeData m_xGyro;
	
public:
	TitleScreen();
	virtual ~TitleScreen();

	virtual void Initialize();
	
protected:
	virtual void OnUpdate(const FrameData& frame);
	virtual void OnRender(Renderer& renderer, const FrameData& frame);
	virtual void OnDoLayout(const CIwSVec2& screensize);

private:
	void ButtonPressedEventHandler(const Button& sender, const Button::EventArgs& args);
	void ButtonPanelStateChangedEventHandler(const ButtonPanel& sender, const ButtonPanel::EventArgs& args);

private:
	void SetGyroData(const s3eGyroscopeData& data);
	static void GyroscopeCallback(void* sysdata, void* usrdata);
};

#endif
