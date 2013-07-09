#include "Level.h"

#include <string>

#include "Debug.h"
#include "Configuration.h"
#include "InputManager.h"
#include "SoundEngine.h"
#include "FactoryManager.h"

Level::Level(const CIwFVec2& worldsize, std::string background) :
	m_xWorldSize(worldsize),
	m_xGame(worldsize), 
	m_xBackground(background, m_xGame), 
	m_xInteractor(m_xCamera, m_xGame),
	m_iCompletionTimer(0),
	m_xButtonBlock(eButtonCommandIdStarBlock, s3eKey1),
	m_xButtonAttack(eButtonCommandIdStarAttack, s3eKey2),
	m_xAppPanel(eButtonCommandIdToggleHud, s3eKeyP),
	m_pxBackdrop(NULL),
	m_pxParticles(NULL) {

	// attach event handlers
	s3eDeviceRegister(S3E_DEVICE_PAUSE, AppPausedCallback, this);
	m_xInteractor.BeginDrawPath.AddListener(this, &Level::BeginDrawPathEventHandler);
	m_xInteractor.EndDrawPath.AddListener(this, &Level::EndDrawPathHandler);
		
	m_xButtonBlock.PressedEvent.AddListener(this, &Level::ButtonPressedEventHandler);
	m_xButtonBlock.ReleasedEvent.AddListener(this, &Level::ButtonReleasedEventHandler);
	m_xButtonAttack.PressedEvent.AddListener(this, &Level::ButtonPressedEventHandler);
	m_xButtonAttack.ReleasedEvent.AddListener(this, &Level::ButtonReleasedEventHandler);
}

Level::~Level() {
	// detach event handlers
	m_xButtonBlock.PressedEvent.RemoveListener(this, &Level::ButtonPressedEventHandler);
	m_xButtonBlock.ReleasedEvent.RemoveListener(this, &Level::ButtonReleasedEventHandler);
	m_xButtonAttack.PressedEvent.RemoveListener(this, &Level::ButtonPressedEventHandler);
	m_xButtonAttack.ReleasedEvent.RemoveListener(this, &Level::ButtonReleasedEventHandler);

	if (m_pxBackdrop) {
		delete m_pxBackdrop;
	}
	if (m_pxParticles) {
		delete m_pxParticles;
	}
	
	m_xInteractor.EndDrawPath.RemoveListener(this, &Level::EndDrawPathHandler);
	m_xInteractor.BeginDrawPath.RemoveListener(this, &Level::BeginDrawPathEventHandler);

	s3eDeviceUnRegister(S3E_DEVICE_PAUSE, AppPausedCallback);

	SoundEngine::StopMusicFile();
}

void Level::Initialize() {
	m_xAppPanel.Initialize();
	m_xAppPanel.GetMainButton().SetTexture(FactoryManager::GetTextureFactory().Create("button_toggle_hud"));
	m_xButtonBlock.SetTexture(FactoryManager::GetTextureFactory().Create("button_action_block"));
	m_xButtonAttack.SetTexture(FactoryManager::GetTextureFactory().Create("button_action_attack"));
	
	m_pxBackdrop = FactoryManager::GetTextureFactory().Create("backdrop");
	
	m_xStatsPanel.Initialize();

	SoundEngine::PlayMusicFileLoop(Configuration::GetInstance().LevelSong);

	m_pxParticles = new ParticleSystem(FactoryManager::GetTextureFactory().GetConfig("button_play"));
	
	CreateStar();
}

GameFoundation& Level::GetGameFoundation() {
	return m_xGame;
}

void Level::CreateStar() {
	IW_CALLSTACK_SELF;
	Star* star = dynamic_cast<Star*>(FactoryManager::GetBodyFactory().Create("star"));
	IwAssertMsg(MYAPP, star, ("Could not add star to level. It seems to be of a wrong type. Check its body definition."));
	if (star) {
		star->SetPosition(GetStarStartPosition(), 0.0f);
		Add(star);
		IwAssertMsg(MYAPP, star->CanDrag(), ("Stars must be draggable. Cannot create star..."));
		star->BeginDragging(GetStarStartPosition());
		star->SetAnchorLine(GetStarStartPosition().x);
	}
}

void Level::Add(Body* body) {
	IW_CALLSTACK_SELF;
	IwAssertMsg(MYAPP, body, ("Could not add body to level. Seems to be NULL..."));
	if (body) {
		m_xGame.Add(body);
	}
}

void Level::SetPaused() {
	m_xAppPanel.ShowPanel();
	IwAssertMsg(MYAPP, IsPaused(), ("Pause requested, but did not work."));
}

bool Level::IsPaused() {
	return m_xAppPanel.IsPanelVisible();
}

bool Level::GetCompletionInfo(GameFoundation::CompletionInfo& info) {
	if (Page::GetCompletionState() == eCompleted) {
		info = m_xGame.GetCompletionInfo();
		return true;
	}
	return false;
}

float Level::GetStarMoveForce() {
	return 10.0f;
}

float Level::GetStarRestForce() {
	return 10.0f;	
}

CIwFVec2 Level::GetStarStartPosition() {
	return CIwFVec2(0.0f, m_xWorldSize.y / 2.0f);
}

void Level::BeginDrawPathEventHandler(const LevelInteractor& sender, const CIwFVec2& pos) {
	IW_CALLSTACK_SELF;
	if (Star* star = m_xGame.GetStar()) {
		IwAssertMsg(MYAPP, star->IsDragging(), ("Star is not being dragged. Something's wrong!"));
		star->MoveDragging(star->GetPosition());
	}
}

void Level::EndDrawPathHandler(const LevelInteractor& sender, const LevelInteractor::PathEventArgs& path) {
	IW_CALLSTACK_SELF;
	if (Star* star = m_xGame.GetStar()) {
		if (path.count > 0) {
			IwAssertMsg(MYAPP, star->IsDragging(), ("Star is not being dragged. Something's wrong!"));
			star->FollowPath(path.count, path.samplepos, (float)Configuration::GetInstance().PathSpeed);
		}
	}
}

void Level::OnDoLayout(const CIwSVec2& screensize) {
	// set up the optics according to level definition
	float maxvisibleworldsize = (float)Configuration::GetInstance().MaxVisibleWorldSize;
	m_xCamera.SetGeometry(m_xWorldSize, screensize, maxvisibleworldsize);
	m_xCamera.SetWorldFocus(m_xWorldSize / 2.0f);
	m_xCamera.ZoomOut();
	
	// create background image to fit the level
	float bgmargin = (float) Configuration::GetInstance().WorldMargin;
	m_xBackground.SetGeometry(m_xWorldSize, screensize, bgmargin);

	// action buttons
	const int spacing = 20;
	CIwRect rect;
	rect.w = 120;
	rect.h = 80;
	rect.x = 0;
	rect.y = screensize.y - (2 * rect.h) - (2 * spacing);
	m_xButtonBlock.SetPosition(rect);
	rect.y += rect.h + spacing;
	m_xButtonAttack.SetPosition(rect);
	
	// app panel
	uint32 btnsize = 60;
	uint32 btnmargin = 15;
	m_xAppPanel.GetMainButton().SetPosition(
		CIwRect(screensize.x - (btnsize + btnmargin),
		btnmargin, btnsize, btnsize));
	
	// backdrop
	m_xBackdropShape.SetRect(0, 0, 150, screensize.y);
}

void Level::OnUpdate(const FrameData& frame) {
	IW_CALLSTACK_SELF;

	m_xAppPanel.Update(frame);

	if (IsPaused()) {
		return;
	}

	// update game logic (remove dead, etc...)
	m_xGame.Update(frame);

	// check if level is finished
	if (m_xGame.IsCompleted()) {
		// delay the completion for a short time
		m_iCompletionTimer += frame.GetSimulatedDurationMs();
		if (LEVEL_COMPLETION_DELAY < m_iCompletionTimer) {
			SetCompletionState(eCompleted);
		}
	}

	// interaction
	m_xInteractor.Update(frame);
	
	// scene and widgets
	m_xCamera.Update(frame.GetScreensize(), frame.GetSimulatedDurationMs());
	m_xBackground.Update(frame);
	
	if (m_pxParticles) {
		m_pxParticles->Update(frame);
	}
	
	// buttons
	m_xButtonBlock.Update(frame);
	m_xButtonAttack.Update(frame);
	
	// progress indicator
	float progress = m_xGame.GetCompletionDegree();
	m_xStatsPanel.SetProgress(progress);
	m_xStatsPanel.Update(frame);
	
	// other
	if (m_pxBackdrop) {
		m_pxBackdrop->Update(frame.GetRealDurationMs());
	}
}

void Level::OnRender(Renderer& renderer, const FrameData& frame) {
	IW_CALLSTACK_SELF;

	renderer.SetViewport(m_xCamera.GetViewport());
	m_xBackground.Render(renderer, frame);
	m_xGame.Render(renderer, frame);

	m_xStatsPanel.Render(renderer, frame);
	m_xAppPanel.Render(renderer, frame);
	
	m_xInteractor.Render(renderer, frame);
	
	if (m_pxParticles) {
		m_pxParticles->Render(renderer, frame);
	}
	
	// other
	if (m_pxBackdrop) {
		renderer.Draw(m_xBackdropShape, *m_pxBackdrop);
	}
	m_xButtonBlock.Render(renderer, frame);
	m_xButtonAttack.Render(renderer, frame);
}

CIwFVec2 Level::CalculateRelativeSoundPosition(const CIwFVec2& worldpos) {
	CIwSVec2 soundpixelpos = m_xCamera.GetViewport().WorldToView(worldpos);
	CIwSVec2 centeroffset(IwGxGetScreenWidth() / 2, IwGxGetScreenHeight() / 2);
	soundpixelpos -= centeroffset;
	return CIwFVec2(soundpixelpos.x / (float)centeroffset.x, soundpixelpos.y / (float)centeroffset.y);
}

int32 Level::AppPausedCallback(void* systemData, void* userData) {
	IW_CALLSTACK_SELF;

	Level* lvl = (Level*) userData;
	lvl->SetPaused();
	return 0;
}

void Level::ButtonPressedEventHandler(const Button& sender, const Button::EventArgs& args) {
	Star* star = m_xGame.GetStar();
	if (!star) {
		IwAssertMsg(MYAPP, star, ("Cannot initiate action. Star not founnd."));
		return;
	}
	
	switch (args.id) {
		case eButtonCommandIdStarBlock:
			m_xButtonAttack.SetEnabled(false);
			star->BeginBlock();
			break;
		case eButtonCommandIdStarAttack:
			m_xButtonBlock.SetEnabled(false);
			star->BeginAttack();
			break;
		default:
			break;
	}
}

void Level::ButtonReleasedEventHandler(const Button& sender, const Button::EventArgs& args) {
	Star* star = m_xGame.GetStar();
	if (!star) {
		IwAssertMsg(MYAPP, star, ("Cannot finish action. Star not founnd."));
		return;
	}
	
	switch (args.id) {
		case eButtonCommandIdStarBlock:
			m_xButtonAttack.SetEnabled(true);
			star->EndBlock();
			break;
		case eButtonCommandIdStarAttack:
			m_xButtonBlock.SetEnabled(true);
			star->EndAttack();
			break;
		default:
			break;
	}
}
