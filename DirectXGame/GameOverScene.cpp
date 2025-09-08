#include "GameOverScene.h"

GameOverScene::~GameOverScene() {
	delete gameOverSprite_;
	delete font_;
	delete bestScoreFont_; 
	delete bestTextSprite_;
}


void GameOverScene::Initialize(int currentScore, int bestScore) {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	sfxConfirmHandle_ = KamataEngine::Audio::GetInstance()->LoadWave("audio/confirm.wav");

	finalScore_ = currentScore;
	bestScore_ = bestScore; // ベストスコアを保存
	isReturnToTitle_ = false;

	textureHandle_ = KamataEngine::TextureManager::Load("gameover/3.png");
	gameOverSprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});
	gameOverSprite_->SetPosition({0, 0});

	// 「BEST」の文字スプライトを初期化
	bestTextTextureHandle_ = KamataEngine::TextureManager::Load("we.png"); // ★★★「BEST」と書かれた画像を用意してください
	bestTextSprite_ = KamataEngine::Sprite::Create(bestTextTextureHandle_, {0.0f, 0.0f});
	bestTextSprite_->SetPosition({0.0f, 0.0f});

	// 今回のスコア用フォント
	font_ = new BIt_Map_Font();
	font_->Initialize();
	font_->Set(finalScore_);
	font_->ShowUnit(false); // mを表示しない

	// ベストスコア用フォント
	bestScoreFont_ = new BIt_Map_Font();
	bestScoreFont_->Initialize();
	bestScoreFont_->Set(bestScore_);
	bestScoreFont_->ShowUnit(false); // mを表示しない
}

void GameOverScene::Update() {
	XINPUT_STATE xInputState = {};
	XINPUT_STATE xInputStatePrev = {};
	bool isControllerConnected = input_->GetJoystickState(0, xInputState) && input_->GetJoystickStatePrevious(0, xInputStatePrev);

	bool isConfirmTriggered = false;
	if (isControllerConnected) {
		if (((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_A)) ||
		    ((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_START))) {
			isConfirmTriggered = true;
		}
	}

	if (input_->TriggerKey(DIK_RETURN) || isConfirmTriggered) {
		KamataEngine::Audio::GetInstance()->PlayWave(sfxConfirmHandle_); // ★決定音を再生
		isReturnToTitle_ = true;
	}
}

void GameOverScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	KamataEngine::Sprite::PreDraw(commandList);

	if (gameOverSprite_) {
		gameOverSprite_->Draw();
	}
	if (bestTextSprite_) {
		bestTextSprite_->SetPosition({440.0f, 105.0f});
		bestTextSprite_->SetSize({160.0f, 50.0f});
		bestTextSprite_->Draw();
	}
	if (bestScoreFont_) {
		bestScoreFont_->SetPosition({600.0f, 70.0f});
		bestScoreFont_->SetScale(1.5f);
		bestScoreFont_->Draw();
	}

	KamataEngine::Sprite::PostDraw();

	if (font_) {
		font_->SetPosition({480.0f, 170.0f});
		font_->SetScale(2.0f);
		font_->Draw();
	}
}