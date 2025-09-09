#include "TitleScnce.h"
#include <numbers>

TitleScnce::~TitleScnce() {
	delete titlemodel_;
	delete titlemodelFont_;
	delete modelSkydome_;
	delete skydome_;
	delete sprite_;
	delete sprite2_;
	delete sprite3_;
	delete sprite4_;
}

// イージング関数の実装
float TitleScnce::EaseInOutQuad(float t) { return t < 0.5f ? 2.0f * t * t : 1.0f - pow(-2.0f * t + 2.0f, 2.0f) / 2.0f; }

float TitleScnce::EaseOutBounce(float t) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (t < 1.0f / d1) {
		return n1 * t * t;
	} else if (t < 2.0f / d1) {
		return n1 * (t -= 1.5f / d1) * t + 0.75f;
	} else if (t < 2.5f / d1) {
		return n1 * (t -= 2.25f / d1) * t + 0.9375f;
	} else {
		return n1 * (t -= 2.625f / d1) * t + 0.984375f;
	}
}

void TitleScnce::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();
	audio_ = KamataEngine::Audio::GetInstance();

	camera_.Initialize();

	textureHandle_ = KamataEngine::TextureManager::Load("Title/Title1.png");
	sprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});

	textureHandle2_ = KamataEngine::TextureManager::Load("Title/ShotGame.png");
	sprite2_ = KamataEngine::Sprite::Create(textureHandle2_, {0, 0});
	sprite2_->SetIsFlipX(false);
	sprite2_->SetAnchorPoint({0.5f, 0.5f});
	sprite2_->SetPosition({640, 400});

	textureHandle3_ = KamataEngine::TextureManager::Load("Title/HitEnter.png");
	sprite3_ = KamataEngine::Sprite::Create(textureHandle3_, {0, 0});

	sfxConfirmHandle_ = Audio::GetInstance()->LoadWave("audio/confirm.wav");


	Timer_ = 0.0f;
	isFinished_ = false;
	isMovingDown = false;
	isFlipped = false;

	titleWorldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	titleWorldTransformFont_.translation_ = {0.0f, 0.0f, 0.0f};

	InitializeSprites();

	// 天球の初期化
	modelSkydome_ = KamataEngine::Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	// これで、正しく初期化されたカメラを渡すことができます
	skydome_->Initialize(modelSkydome_, &camera_);
}

void TitleScnce::InitializeSprites() {
	sprites.push_back(sprite_);
	sprites.push_back(sprite2_);
	sprites.push_back(sprite3_);
	sprites.push_back(sprite4_);
}

void TitleScnce::Update() {

	if (skydome_) {
		skydome_->Update();
	}

	Timer_ += 1.0f;


	// ▼▼▼ コントローラー入力の判定を追加 ▼▼▼
	XINPUT_STATE xInputState = {};     // {} を追加して初期化
	XINPUT_STATE xInputStatePrev = {}; // ← ★★★ここを修正★★★
	bool isControllerConnected = input_->GetJoystickState(0, xInputState) && input_->GetJoystickStatePrevious(0, xInputStatePrev);

	bool isConfirmTriggered = false;
	if (isControllerConnected) {
		// AボタンまたはSTARTボタンが押された瞬間
		if (((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_A)) ||
		    ((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_START))) {
			isConfirmTriggered = true;
		}
	}

	// Enterキーでタイトル終了
	if (input_->TriggerKey(DIK_RETURN) || isConfirmTriggered) {
		Audio::GetInstance()->PlayWave(sfxConfirmHandle_); // ★決定音を再生
		isFinished_ = true;
		// ... (以降のアニメーションリセット処理は省略) ...
	}

	// sprites[1] のアニメーション - イージング付き（タイトル中のみ）
	if (!isFinished_ && sprite2_) {
		// アニメーションタイマーを更新
		animationTimer += 0.016f; // 60FPSを想定した時間増分

		// 現在のスプライトの位置を取得
		KamataEngine::Vector2 pos = sprite2_->GetPosition();

		// 移動速度
		float slowMoveSpeed = 2.0f;

		// 反転アニメーション処理
		if (isFlipping) {
			flipProgress += 0.016f / flipDuration; // 進行度を更新

			if (flipProgress >= 1.0f) {
				// 反転アニメーション完了
				flipProgress = 1.0f;
				isFlipping = false;
				currentScale = 1.0f;
				// 実際の反転状態を切り替え
				isFlipped = !isFlipped;
				sprite2_->SetIsFlipX(isFlipped);
			} else {
				// イージングを適用してスケールを計算
				if (flipProgress < 0.5f) {
					// 前半：1.0f → 0.0f (縮小)
					float t = flipProgress * 2.0f; // 0.0f ～ 1.0f にマッピング
					currentScale = 1.0f - EaseInOutQuad(t);
				} else {
					// 後半：0.0f → 1.0f (拡大)
					float t = (flipProgress - 0.5f) * 2.0f; // 0.0f ～ 1.0f にマッピング
					currentScale = EaseOutBounce(t);
					// この時点でスプライトの反転状態を更新（1回だけ）
					static bool flippedThisCycle = false;
					if (!flippedThisCycle) {
						sprite2_->SetIsFlipX(!isFlipped);
						flippedThisCycle = true;
					}
					// サイクル終了時にフラグをリセット
					if (flipProgress >= 0.9f) {
						flippedThisCycle = false;
					}
				}
			}
		} else {
			currentScale = 1.0f;
		}

		// 移動方向の切り替えと反転開始判定
		if (isMovingDown) {
			pos.y += slowMoveSpeed;
			if (pos.y >= SCREEN_BOTTOM_Y && !isFlipping) {
				isMovingDown = false;
				// 反転アニメーション開始
				isFlipping = true;
				flipProgress = 0.0f;
			}
		} else {
			pos.y -= slowMoveSpeed;
			if (pos.y <= FLIP_THRESHOLD && !isFlipping) {
				isMovingDown = true;
				// 反転アニメーション開始
				isFlipping = true;
				flipProgress = 0.0f;
			}
		}

		// スプライトの位置を更新
		sprite2_->SetPosition(pos);

		// スケールを適用（イージング効果）
		// 元のサイズを保持しつつ、X軸のみスケール変更
		KamataEngine::Vector2 originalSize = {1280.0f, 720.0f};
		sprite2_->SetSize({originalSize.x * currentScale, originalSize.y});
	}
}

void TitleScnce::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Sprite::PostDraw();

	dxCommon->ClearDepthBuffer();

	Model::PreDraw(dxCommon->GetCommandList());

	if (skydome_) {
		skydome_->Draw();
	}

	Model::PostDraw();

	Sprite::PreDraw(commandList);

	if (sprite2_) {
		sprite2_->Draw();
	}

	if (sprite3_ && static_cast<int>(Timer_) % 60 < 30) {
		sprite3_->SetPosition({0, 0});
		sprite3_->Draw();
	}

	Sprite::PostDraw();
}