#include "SelectScene.h"

SelectScene::~SelectScene() { delete selectSprite_; }

void SelectScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// タイトルから戻ってきたときの再初期化に備えてフラグをリセット
	isGameStart_ = false;

	textureHandle_ = KamataEngine::TextureManager::Load("select/1.png");
	selectSprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});
	selectSprite_->SetPosition({0, 0}); // 画面中央に配置
}

void SelectScene::Update() {
	// Enterキーが押されたらゲーム開始
	if (input_->TriggerKey(DIK_RETURN)) {
		isGameStart_ = true;
	}
}

void SelectScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);

	if (selectSprite_) {
		selectSprite_->Draw();
	}

	KamataEngine::Sprite::PostDraw();
}