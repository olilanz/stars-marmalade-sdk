#include "ParticleEffect.h"
#include "Debug.h"

ParticleEffect::ParticleEffect(const std::string& id, const ShapeTemplate& shapedef, const TextureTemplate& texturedef) : LocalEffect(id, shapedef, texturedef), m_pxSystem(NULL) {
	
	m_pxSystem = new ParticleSystem(texturedef, CIwFVec2(-5.0f, -5.0f), "", "");
	m_pxSystem->SetPosition(GetPosition());
	m_pxSystem->SetBirthRate(200);

	m_pxSystem->SetParticleSize(CIwFVec2(0.1f, 0.1f));
	m_pxSystem->SetParticleSpeed(CIwFVec2(10.0f, 10.0f));
	m_pxSystem->SetParticleLifetime(500);
}

ParticleEffect::~ParticleEffect() {
	if (m_pxSystem) {
		delete m_pxSystem;
	}
}

const char* ParticleEffect::GetTypeName() {
	return ParticleEffect::TypeName();
}

const char* ParticleEffect::TypeName() {
	static const char* type = "particleeffect";
	return type;
}

bool ParticleEffect::CanDispose() {
	return LocalEffect::CanDispose()
		&& m_pxSystem
		&& !m_pxSystem->HasParticles();
}

void ParticleEffect::OnUpdate(const FrameData& frame) {
	IW_CALLSTACK_SELF;
	if (m_pxSystem) {
		bool started = m_pxSystem->IsStarted();
		bool emit = GetRemainingFadeTime() > 0;
		if (!started && emit) {
			m_pxSystem->Start();
		}
		if (started && !emit) {
			m_pxSystem->Stop();
		}
		m_pxSystem->SetPosition(GetPosition());
		m_pxSystem->Update(frame);
	}
	
	LocalEffect::OnUpdate(frame);
}

void ParticleEffect::OnRender(Renderer& renderer, const FrameData& frame) {
	IW_CALLSTACK_SELF;
	if (m_pxSystem) {
		m_pxSystem->Render(renderer, frame);
	}
	LocalEffect::OnRender(renderer, frame);
}
