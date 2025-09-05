#include "PauseMenu.h"

PauseMenu::~PauseMenu() { delete pauseSprite_; }

void PauseMenu::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	textureHandle_ = KamataEngine::TextureManager::Load("pause.png"); // 適切なテクスチャに置き換えてください
	pauseSprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});
	pauseSprite_->SetPosition({640, 360});

	isResume_ = false;
	isQuit_ = false;
}

void PauseMenu::Update() {
	// Rキーでゲームに戻る
	if (input_->TriggerKey(DIK_R)) {
		isResume_ = true;
	}
	// Qキーでタイトルに戻る
	if (input_->TriggerKey(DIK_Q)) {
		isQuit_ = true;
	}
}

void PauseMenu::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);

	if (pauseSprite_) {
		pauseSprite_->Draw();
	}

	KamataEngine::Sprite::PostDraw();
}