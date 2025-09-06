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
	// ▼▼▼ コントローラー入力の判定を追加 ▼▼▼
	XINPUT_STATE xInputState = {};
	XINPUT_STATE xInputStatePrev = {};
	bool isControllerConnected = input_->GetJoystickState(0, xInputState) && input_->GetJoystickStatePrevious(0, xInputStatePrev);

	bool isConfirmTriggered = false;
	if (isControllerConnected) {
		// AボタンまたはSTARTボタンが押された瞬間
		if (((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_A)) ||
		    ((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_START))) {
			isConfirmTriggered = true;
		}
	}

	// Enterキーまたはコントローラーの決定ボタンでタイトルへ戻る
	if (input_->TriggerKey(DIK_RETURN) || isConfirmTriggered) {
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