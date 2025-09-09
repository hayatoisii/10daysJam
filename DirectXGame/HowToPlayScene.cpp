#include "HowToPlayScene.h"

HowToPlayScene::~HowToPlayScene() { delete howToPlaySprite_; }

void HowToPlayScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	isFinished_ = false; // シーン開始時に必ずリセット

	// 操作説明の画像を読み込む（"how_to_play.png"というファイル名は仮です）
	textureHandle_ = TextureManager::Load("how_to_play/how_to_play.png");
	howToPlaySprite_ = Sprite::Create(textureHandle_, {0, 0});
	howToPlaySprite_->SetPosition({0, 0}); // 全画面表示を想定

	// 決定音を読み込む
	sfxConfirmHandle_ = Audio::GetInstance()->LoadWave("audio/confirm.wav");
}

void HowToPlayScene::Update() {
	// コントローラーの状態を取得
	XINPUT_STATE xInputState = {};
	XINPUT_STATE xInputStatePrev = {};
	bool isControllerConnected = input_->GetJoystickState(0, xInputState) && input_->GetJoystickStatePrevious(0, xInputStatePrev);

	// Aボタンが押された瞬間かどうかの判定
	bool isAButtonTriggered = false;
	if (isControllerConnected) {
		if ((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			isAButtonTriggered = true;
		}
	}

	// スペースキーまたはAボタンが押されたらシーンを終了
	if (input_->TriggerKey(DIK_SPACE) || isAButtonTriggered) {
		Audio::GetInstance()->PlayWave(sfxConfirmHandle_);
		isFinished_ = true;
	}
}

void HowToPlayScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	Sprite::PreDraw(commandList);

	// 操作説明のスプライトを描画
	if (howToPlaySprite_) {
		howToPlaySprite_->Draw();
	}

	Sprite::PostDraw();
}