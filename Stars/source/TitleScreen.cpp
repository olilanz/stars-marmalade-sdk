#include "TitleScreen.h"
#include "FactoryManager.h"
#include "Debug.h"
#include "Configuration.h"

#include "spine/extension.h"

TitleScreen::TitleScreen() :
Page("menu.group", Configuration::GetInstance().MenuSong),
m_xButtonTitle(eButtonCommandIdOpenWorldMenu, s3eKeyAbsOk),
m_xButtonMovie(eButtonCommandIdOpenIntroMovie, s3eKeyM),
m_xPanelOptions(eButtonCommandIdOptions, s3eKeyFirst),
m_xPanelSocial(eButtonCommandIdSocial, s3eKeyFirst) {
	m_xPanelOptions.StateChanged.AddListener<TitleScreen>(this, &TitleScreen::ButtonPanelStateChangedEventHandler);
	m_xPanelSocial.StateChanged.AddListener<TitleScreen>(this, &TitleScreen::ButtonPanelStateChangedEventHandler);
}


TitleScreen::~TitleScreen() {
	m_xPanelOptions.StateChanged.RemoveListener<TitleScreen>(this, &TitleScreen::ButtonPanelStateChangedEventHandler);
	m_xPanelSocial.StateChanged.RemoveListener<TitleScreen>(this, &TitleScreen::ButtonPanelStateChangedEventHandler);
}

void TitleScreen::Initialize() {
	IW_CALLSTACK_SELF;
	
	m_xCamera.SetGeometry(CIwFVec2(1500.0f, 500.0f), CIwSVec2(IwGxGetScreenWidth(), IwGxGetScreenHeight()), 1000.0f);
	m_xCamera.SetWorldFocus(CIwFVec2(0.0f, 0.0f));
	m_xCamera.ZoomOut();
	
	m_xBackground.Initialize();
	m_xPanelOptions.Initialize();
	m_xPanelOptions.GetMainButton().SetTexture(FactoryManager::GetTextureFactory().Create("button_options"));
	
	m_xPanelSocial.Initialize();
	m_xPanelSocial.GetMainButton().SetTexture(FactoryManager::GetTextureFactory().Create("button_achievements"));

    m_xButtonTitle.SetTexture(FactoryManager::GetTextureFactory().Create("title_button"));
    m_xButtonMovie.SetTexture(FactoryManager::GetTextureFactory().Create("button_movie"));

	PageSettings ps;
	ps.SetWorld(LevelIterator::eWorldIdEarth);
	SetBackground(
		ps.GetWorldColours().LowerLeft,
		ps.GetWorldColours().LowerRight,
		ps.GetWorldColours().UpperRight,
		ps.GetWorldColours().UpperLeft);
	
	
	/*
	 std::string atlasfile("sprites/characters/spine/spineboy.atlas");
	 std::string skeletonfile("sprites/characters/spine/spineboy.json");
	 */
	
	m_xAnim.Load("spine/title/output/title.atlas", "spine/title/output/title.json", 0.25);
	m_xAnim.SetAnimation("enter");
	m_xAnim.SetPosition(CIwFVec2(-300.0f, -100.0f));
	
	m_xAnim2.Load("spine/title/output/title.atlas", "spine/title/output/title.json", 0.25);
	m_xAnim2.SetAnimation("exit");
	m_xAnim2.SetPosition(CIwFVec2(300.0f, -100.0f));
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
    
	button.x = screencenter.x - (button.w / 2);
	m_xButtonMovie.SetPosition(button);

	button.x = screencenter.x + (extents / 2) - button.w;
	m_xPanelSocial.GetMainButton().SetPosition(button);
}

void TitleScreen::OnUpdate(const FrameData& frame) {
	IW_CALLSTACK_SELF;

	m_xButtonTitle.Update(frame);
	m_xPanelSocial.Update(frame);
	m_xButtonMovie.Update(frame);
	m_xPanelOptions.Update(frame);
	m_xBackground.Update(frame);
	m_xAnim.Update(frame.GetSimulatedDurationMs());
	m_xAnim2.Update(frame.GetSimulatedDurationMs());
}

void TitleScreen::OnRender(Renderer& renderer, const FrameData& frame) {
	IW_CALLSTACK_SELF;

	renderer.SetViewport(m_xCamera.GetViewport());
	m_xBackground.Render(renderer, frame);

	int length = std::max<int>(m_xAnim.GetVertexCount(), m_xAnim2.GetVertexCount());
	CIwFVec2* xys = new CIwFVec2[length];
	CIwFVec2* uvs = new CIwFVec2[length];
	uint32* cols = new uint32[length];
	if (CIwTexture* texture = m_xAnim.GetStreams(length, xys, uvs, cols)) {
		renderer.DrawImage(texture, xys, uvs, cols, length);
	}
	if (CIwTexture* texture = m_xAnim2.GetStreams(length, xys, uvs, cols)) {
		renderer.DrawImage(texture, xys, uvs, cols, length);
	}
	delete [] xys;
	delete [] uvs;
	delete [] cols;
	
    // buttons
	m_xButtonTitle.Render(renderer, frame);
	m_xPanelOptions.Render(renderer, frame);
	m_xButtonMovie.Render(renderer, frame);
	m_xPanelSocial.Render(renderer, frame);
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
