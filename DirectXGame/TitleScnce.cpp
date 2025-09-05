#include "TitleScnce.h"
#include <numbers>

TitleScnce::~TitleScnce() {
	delete titlemodel_;
	delete titlemodelFont_;
	delete TitleSkydome_;
	delete sprite_;
	delete sprite2_;
	delete sprite3_;
	delete sprite4_;
}

void TitleScnce::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();
	audio_ = KamataEngine::Audio::GetInstance();

	textureHandle_ = KamataEngine::TextureManager::Load("Title/Title1.png");
	sprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});

	textureHandle2_ = KamataEngine::TextureManager::Load("Title/ShotGame.png");
	sprite2_ = KamataEngine::Sprite::Create(textureHandle2_, {0, 0});

	// スプライト2の初期位置を画面下部に設定
	sprite2_->SetPosition({0, SCREEN_BOTTOM_Y});

	textureHandle3_ = KamataEngine::TextureManager::Load("Title/HitEnter.png");
	sprite3_ = KamataEngine::Sprite::Create(textureHandle3_, {0, 0});

	titleskydome.Initialize();
	Camera_.Initialize();

	Timer_ = 0.0f;
	isFinished_ = false;
	isMovingDown = false; // 最初は上へ移動
	isFlipped = false;    // 最初は反転しない

	// タイトルを中央に寄せるために調整
	titleWorldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	titleWorldTransformFont_.translation_ = {0.0f, 0.0f, 0.0f};

	titleskydome.translation_ = {0.0f, 0.0f, 0.0f};

	// スプライトの初期化
	InitializeSprites();
}


void TitleScnce::InitializeSprites() {
	sprites.push_back(sprite_);
	sprites.push_back(sprite2_);
	sprites.push_back(sprite3_);
	sprites.push_back(sprite4_);
}

void TitleScnce::Update() {
	Timer_ += 1.0f;

	if (input_->TriggerKey(DIK_RETURN)) {
		isFinished_ = true;
	}

	if (sprite2_) {
		// 現在のスプライトの位置を取得
		KamataEngine::Vector2 pos = sprite2_->GetPosition();

		// 移動方向のロジック
		if (isMovingDown) {
			// 下に移動
			pos.y += sprite2MoveSpeed;

			// 画面下端に達したら、上に切り替えて反転
			if (pos.y >= SCREEN_BOTTOM_Y) {
				isMovingDown = false;
				isFlipped = !isFlipped;
			}
		} else {
			// 上に移動
			pos.y -= sprite2MoveSpeed;

			// 中央に達したら、下に切り替えて反転
			if (pos.y <= FLIP_THRESHOLD) {
				isMovingDown = true;
				isFlipped = !isFlipped;
			}
		}

		// スプライトの位置を更新
		sprite2_->SetPosition(pos);

		// スプライトを反転
		sprite2_->SetIsFlipX(isFlipped);
	}
}

void TitleScnce::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 3Dオブジェクト描画
	KamataEngine::Model::PreDraw(commandList);
	KamataEngine::Model::PostDraw();

	// スプライト描画
	KamataEngine::Sprite::PreDraw(commandList);

	// ★ここで背景スプライトを全画面に描画
	// 例: 1280x720の青色背景
	// KamataEngine::Sprite::DrawRect({0, 0}, {1280, 720}, {0.4f, 0.6f, 0.9f, 1.0f}); // RGBA

	sprite_->Draw();

	if (sprite2_) {
		sprite2_->Draw();
	}

	// 「Hit Enter」点滅（30フレームごとにON/OFF）
	if (sprite3_ && static_cast<int>(Timer_) % 60 < 30) {
		sprite3_->SetPosition({0, 0});
		sprite3_->Draw();
	}

	KamataEngine::Sprite::PostDraw();
}