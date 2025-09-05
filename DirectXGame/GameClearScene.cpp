#include "GameClearScene.h"

GameClearScene::~GameClearScene() { delete gameClearSprite_; }

void GameClearScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// タイトルに戻るフラグを毎回リセット
	isReturnToTitle_ = false;

	textureHandle_ = KamataEngine::TextureManager::Load("gameclear/2.png"); // 適切なテクスチャに置き換えてください
	gameClearSprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});
	gameClearSprite_->SetPosition({0, 0});
}

void GameClearScene::Update() {
	// 修正後
	if (input_->TriggerKey(DIK_RETURN)) {
		isReturnToTitle_ = true;
	}
}

void GameClearScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);

	if (gameClearSprite_) {
		gameClearSprite_->Draw();
	}

	KamataEngine::Sprite::PostDraw();
}