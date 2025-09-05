#include "GameOverScene.h"

GameOverScene::~GameOverScene() { delete gameOverSprite_; }

void GameOverScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// タイトルに戻るフラグを毎回リセット
	isReturnToTitle_ = false;

	textureHandle_ = KamataEngine::TextureManager::Load("gameover/3.png"); // 適切なテクスチャに置き換えてください
	gameOverSprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});
	gameOverSprite_->SetPosition({0, 0});
}

void GameOverScene::Update() {

	// 修正後
	if (input_->TriggerKey(DIK_RETURN)) {
		isReturnToTitle_ = true;
	}
}

void GameOverScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);

	if (gameOverSprite_) {
		gameOverSprite_->Draw();
	}

	KamataEngine::Sprite::PostDraw();
}