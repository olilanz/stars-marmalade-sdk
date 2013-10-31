#include "TitleScreen.h"
#include "FactoryManager.h"
#include "Debug.h"
#include "Configuration.h"

TitleScreen::TitleScreen() :
	Page("menu.group", Configuration::GetInstance().IntroSong),
	m_xButtonTitle(eButtonCommandIdOpenWorldMenu, s3eKeyAbsOk),
	m_xPanelOptions(eButtonCommandIdOptions, s3eKeyFirst),
	m_xPanelSocial(eButtonCommandIdSocial, s3eKeyFirst),
	m_pxBackground(NULL) {

		m_xPanelOptions.StateChanged.AddListener<TitleScreen>(this, &TitleScreen::ButtonPanelStateChangedEventHandler);
		m_xPanelSocial.StateChanged.AddListener<TitleScreen>(this, &TitleScreen::ButtonPanelStateChangedEventHandler);
}

TitleScreen::~TitleScreen() {
	m_xPanelOptions.StateChanged.RemoveListener<TitleScreen>(this, &TitleScreen::ButtonPanelStateChangedEventHandler);
	m_xPanelSocial.StateChanged.RemoveListener<TitleScreen>(this, &TitleScreen::ButtonPanelStateChangedEventHandler);

	if (m_pxBackground) {
		delete m_pxBackground;
	}
}

void TitleScreen::Initialize() {
	IW_CALLSTACK_SELF;
	
	m_pxBackground = FactoryManager::GetTextureFactory().Create("title_bg");

	m_xPanelOptions.Initialize();
	m_xPanelOptions.GetMainButton().SetTexture(FactoryManager::GetTextureFactory().Create("button_options"));
	
	m_xPanelSocial.Initialize();
	m_xPanelSocial.GetMainButton().SetTexture(FactoryManager::GetTextureFactory().Create("button_achievements"));

    m_xButtonTitle.SetTexture(FactoryManager::GetTextureFactory().Create("title_button"));
}

void TitleScreen::OnUpdate(const FrameData& frame) {
	IW_CALLSTACK_SELF;

	m_xButtonTitle.Update(frame);
	m_xPanelSocial.Update(frame);
	m_xPanelOptions.Update(frame);

	if (m_pxBackground) {
		m_pxBackground->Update(frame.GetRealDurationMs());
    }
}

void TitleScreen::OnRender(Renderer& renderer, const FrameData& frame) {
	IW_CALLSTACK_SELF;

	renderer.SetViewport(m_xCamera.GetViewport());

    // background
    if (m_pxBackground) {
        VertexStreamScreen shape;        
        const CIwSVec2& screensize = frame.GetScreensize();
        shape.SetRect(CIwRect(0, 0, screensize.x, screensize.y));
        renderer.Draw(shape, *m_pxBackground);
    }
    
    // buttons
	m_xButtonTitle.Render(renderer, frame);
	m_xPanelOptions.Render(renderer, frame);
	m_xPanelSocial.Render(renderer, frame);
}

void TitleScreen::OnDoLayout(const CIwSVec2& screensize) {
	CIwSVec2 screencenter(screensize.x / 2, screensize.y / 2);
	CIwRect button;
	int extents = GetScreenExtents();
	
	// main button
    button.w = extents * 0.9;
    button.h = button.w * 2 / 3;
    button.x = screencenter.x - (button.w / 2);
	button.y = extents / 6;
	m_xButtonTitle.SetPosition(button);
	
    // menu buttons
    button.w = extents / 7;
    button.h = extents / 7;
    button.x = screencenter.x - (extents / 2);
	button.y = screencenter.y + (extents / 3);
	m_xPanelOptions.GetMainButton().SetPosition(button);
    
	button.x = screencenter.x + (extents / 2) - button.w;
	m_xPanelSocial.GetMainButton().SetPosition(button);
}

void TitleScreen::ButtonPressedEventHandler(const Button& sender, const Button::EventArgs& args) {
    if (&sender == &m_xButtonTitle) {
		SetCompletionState(eCompleted);
    }
}

void TitleScreen::ButtonPanelStateChangedEventHandler(const ButtonPanel& sender, const ButtonPanel::EventArgs& args) {
	if (&sender == &m_xPanelOptions) {
		m_xPanelSocial.SetEnabled(!args.IsPanelOpen);
	} else if (&sender == &m_xPanelSocial) {
		m_xPanelOptions.SetEnabled(!args.IsPanelOpen);
	}
	m_xButtonTitle.SetEnabled(!args.IsPanelOpen);
}