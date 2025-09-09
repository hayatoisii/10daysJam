#include "SelectScene.h"
#include <cmath>

// イージング関数（EaseOutCubic）
float EaseOutCubic(float t) { return 1.0f - std::pow(1.0f - t, 3.0f); }

SelectScene::~SelectScene() {
	delete selectSprite_;
	delete selectSprite_1;
	delete selectSprite_2;
	delete selectSprite_3;
	delete arrowSprite_;
	delete aButtonSprite_;
}

void SelectScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// ▼▼▼ 効果音の読み込みを追加 ▼▼▼
	sfxSelectHandle_ = Audio::GetInstance()->LoadWave("audio/select.wav");
	sfxConfirmHandle_ = Audio::GetInstance()->LoadWave("audio/confirm.wav");


	// タイトルから戻ってきたときの再初期化に備えてフラグをリセット
	selectionResult_ = SelectionResult::None; // 選択状態をリセット

	textureHandle_ = KamataEngine::TextureManager::Load("select/1.png");
	selectSprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});
	selectSprite_->SetPosition({0, 0}); // 画面中央に配置

	textureHandle_1 = KamataEngine::TextureManager::Load("select/2.png");
	selectSprite_1 = KamataEngine::Sprite::Create(textureHandle_1, {0, 0});
	selectSprite_1->SetPosition({0, 0}); // 画面中央に配置

	textureHandle_2 = KamataEngine::TextureManager::Load("select/3.png");
	selectSprite_2 = KamataEngine::Sprite::Create(textureHandle_2, {0, 0});
	selectSprite_2->SetPosition({0, 0}); // 画面中央に配置

	textureHandle_3 = KamataEngine::TextureManager::Load("select/4.png");
	selectSprite_3 = KamataEngine::Sprite::Create(textureHandle_3, {0, 0});
	selectSprite_3->SetPosition({0, 0}); // 画面中央に配置

    KeytextureHandle_ = KamataEngine::TextureManager::Load("key.png");
	KeyselectSprite_ = KamataEngine::Sprite::Create(KeytextureHandle_, {0, 0});
	KeyselectSprite_->SetAnchorPoint({0.5f, 0.5f});

	// ▼▼▼ ここを調整します ▼▼▼
	// KeyselectSprite_のX座標を調整して、全体の配置の基準にする
	KeyselectSprite_->SetPosition({1280.0f / 2.0f - 100.0f, 720.0f - 100.0f}); // 例えば、-100.0fに大きくずらしてみる

	aButtonTextureHandle_ = KamataEngine::TextureManager::Load("a_button.png");
	aButtonSprite_ = KamataEngine::Sprite::Create(aButtonTextureHandle_, {0, 0});
	aButtonSprite_->SetAnchorPoint({0.5f, 0.5f});

	// KeyselectSprite_の右隣に配置
	Vector2 keySpritePos = KeyselectSprite_->GetPosition();
	// ▼▼▼ ここを調整します ▼▼▼
	// aButtonSprite_のX座標をKeyselectSprite_のX座標からさらに右にずらす
	aButtonSprite_->SetPosition({keySpritePos.x + 300.0f, keySpritePos.y}); // 例えば、+100.0fに大きくずらしてみる

	// 矢印スプライトの作成
	arrowTextureHandle_ = KamataEngine::TextureManager::Load("select/Sentaku.png");
	arrowSprite_ = KamataEngine::Sprite::Create(arrowTextureHandle_, {0, 0});
	// 回転の中心をスプライトの中心に設定
	arrowSprite_->SetAnchorPoint({0.5f, 0.5f});

	// 矢印の初期位置を設定
	float screenWidth = 1280.0f;
	float arrowY = 180.0f;
	currentArrowX_ = screenWidth * 0.23f; // 選択0の位置
	currentArrowY_ = arrowY;
	targetArrowX_ = currentArrowX_;
	targetArrowY_ = currentArrowY_;
	arrowSprite_->SetPosition({currentArrowX_, currentArrowY_});
}

void SelectScene::Update() {
	// ▼▼▼ 関数のはじめに、コントローラーの状態変数を準備 ▼▼▼
	XINPUT_STATE xInputState = {};     // {} を追加して初期化
	XINPUT_STATE xInputStatePrev = {}; // ← ★★★ここを修正★★★
	bool isControllerConnected = input_->GetJoystickState(0, xInputState) && input_->GetJoystickStatePrevious(0, xInputStatePrev);

	// 十字キーまたは左スティックの左右トリガー判定
	bool isLeftTriggered = false;
	bool isRightTriggered = false;
	if (isControllerConnected) {
		// D-Pad
		if ((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) {
			isLeftTriggered = true;
		}
		if ((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) {
			isRightTriggered = true;
		}
		// Left Stick
		if (xInputState.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && xInputStatePrev.Gamepad.sThumbLX >= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			isLeftTriggered = true;
		}
		if (xInputState.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && xInputStatePrev.Gamepad.sThumbLX <= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			isRightTriggered = true;
		}
	}

	// 左矢印キーが押されたら前の画面へ
	if (input_->TriggerKey(DIK_LEFT) || isLeftTriggered) {
		Audio::GetInstance()->PlayWave(sfxSelectHandle_); // ★選択音を再生
		currentSelectIndex_--;
		if (currentSelectIndex_ < 0) {
			currentSelectIndex_ = 2;
		}
		StartArrowMovement();
	}

	// 右矢印キーが押されたら次の画面へ
	if (input_->TriggerKey(DIK_RIGHT) || isRightTriggered) {
		Audio::GetInstance()->PlayWave(sfxSelectHandle_); // ★選択音を再生
		currentSelectIndex_++;
		if (currentSelectIndex_ > 2) {
			currentSelectIndex_ = 0;
		}
		StartArrowMovement();
	}

	// Aボタンが押された瞬間かどうかの判定
	bool isAButtonTriggered = false;
	if (isControllerConnected) {
		if ((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			isAButtonTriggered = true;
		}
	}

	//if (currentSelectIndex_ == 0) {
	//	// EnterキーまたはAボタンが押されたらゲーム開始
	//	if (input_->TriggerKey(DIK_SPACE) || isAButtonTriggered) {
	//		Audio::GetInstance()->PlayWave(sfxConfirmHandle_); // ★決定音を再生
	//		isGameStart_ = true;
	//	}
	//}


	if (input_->TriggerKey(DIK_RETURN) || input_->TriggerKey(DIK_SPACE) || isAButtonTriggered) {
		Audio::GetInstance()->PlayWave(sfxConfirmHandle_); // ★決定音を再生

		// 現在のカーソル位置によって結果をセットする
		switch (currentSelectIndex_) {
		case 0: // 1枚目の画像
			selectionResult_ = SelectionResult::StartGame;
			break;
		case 1: // 2枚目の画像 (select/2.png)
			selectionResult_ = SelectionResult::StartTutorial;
			break;
		case 2:                                          // 3枚目の画像
			selectionResult_ = SelectionResult::Option3; // とりあえずOption3としておく
			break;
		}
	}

	UpdateArrowAnimation();
}

// 矢印の移動を開始する関数
void SelectScene::StartArrowMovement() {
	float screenWidth = 1280.0f;
	float arrowY = 180.0f;

	// 現在の位置を目標位置に設定
	targetArrowX_ = currentArrowX_;
	targetArrowY_ = currentArrowY_;

	// 新しい目標位置を設定
	switch (currentSelectIndex_) {
	case 0:
		targetArrowX_ = screenWidth * 0.23f;
		targetArrowY_ = arrowY;
		break;
	case 1:
		targetArrowX_ = screenWidth * 0.50f;
		targetArrowY_ = arrowY;
		break;
	case 2:
		targetArrowX_ = screenWidth * 0.77f;
		targetArrowY_ = arrowY;
		break;
	}

	// アニメーション開始
	isArrowMoving_ = true;
	// 矢印が上下に浮遊するアニメーションのタイマーをリセット
	animationTimer_ = 0.0f;
}

// 矢印のアニメーション更新
void SelectScene::UpdateArrowAnimation() {
	// 上下の浮遊アニメーション
	float floatSpeed = 0.05f;     // 浮遊速度を調整
	float floatAmplitude = 10.0f; // 浮遊の振幅を調整
	animationTimer_ += floatSpeed;
	arrowVerticalOffset_ = floatAmplitude * std::sin(animationTimer_);

	// 位置移動のアニメーション
	if (isArrowMoving_) {
		float moveSpeed = 0.1f; // 移動速度
		float distance = static_cast<float>(std::sqrt(std::pow(targetArrowX_ - currentArrowX_, 2.0f) + std::pow(targetArrowY_ - currentArrowY_, 2.0f)));

		if (distance > 1.0f) {
			// イージングでスムーズに移動
			float moveT = moveSpeed;
			moveT = EaseOutCubic(moveT);

			currentArrowX_ += (targetArrowX_ - currentArrowX_) * moveT;
			currentArrowY_ += (targetArrowY_ - currentArrowY_) * moveT;
		} else {
			// 移動完了
			currentArrowX_ = targetArrowX_;
			currentArrowY_ = targetArrowY_;
			isArrowMoving_ = false;
		}
	}

	// 最終的な矢印の位置を設定
	if (arrowSprite_) {
		arrowSprite_->SetPosition({currentArrowX_, currentArrowY_ + arrowVerticalOffset_});
	}
}

void SelectScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 2D描画前準備
	KamataEngine::Sprite::PreDraw(commandList);

	// 現在の選択インデックスに応じてスプライトを描画
	switch (currentSelectIndex_) {
	case 0:
		if (selectSprite_) {
			selectSprite_->Draw();
		}
		break;
	case 1:
		if (selectSprite_1) {
			selectSprite_1->Draw();
		}
		break;
	case 2:
		if (selectSprite_2) {
			selectSprite_2->Draw();
		}
		break;
	case 3:
		if (selectSprite_3) {
			selectSprite_3->Draw();
		}
		break;
	}

	// 矢印スプライトを描画
	if (arrowSprite_) {
		arrowSprite_->Draw();
	}

	// キースプライトを描画
	if (KeyselectSprite_) {
		KeyselectSprite_->Draw();
	}

	if (aButtonSprite_) {
		aButtonSprite_->Draw();
	}

	// 2D描画終了
	KamataEngine::Sprite::PostDraw();
}