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

	textureHandle_ = KamataEngine::TextureManager::Load("Title/Title1.png");
	sprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});

	textureHandle2_ = KamataEngine::TextureManager::Load("Title/ShotGame.png");
	sprite2_ = KamataEngine::Sprite::Create(textureHandle2_, {0, 0});
	sprite2_->SetIsFlipX(false);
	sprite2_->SetAnchorPoint({0.5f, 0.5f});
	sprite2_->SetPosition({640, 400});

	textureHandle3_ = KamataEngine::TextureManager::Load("Title/HitEnter.png");
	sprite3_ = KamataEngine::Sprite::Create(textureHandle3_, {0, 0});

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

	if (input_->TriggerKey(DIK_RETURN)) {
		isFinished_ = true;
	}

		// 足場の生成タイミング管理
	platformSpawnTimer += 1.0f / 60.0f; // 60FPS想定
	if (platformSpawnTimer >= platformSpawnInterval) {
		platformSpawnTimer = 0.0f;

		std::uniform_real_distribution<float> posX(-15.0f, 15.0f);

		Vector3 pos;
		if (player_->IsInversion()) {
			pos = {posX(randomEngine_), 21.0f, 0.0f};
		} else {
			pos = {posX(randomEngine_), -20.0f, 0.0f};
		}

		Vector3 scale = {1.5f, 1.2f, 1.0f};
		Platform* platform = new Platform();
		platform->Initialize(pos, scale, modelPlatform_, &camera_);
		platforms_.push_back(platform);
	}

	// プレイヤーの重力に応じたスクロール
	float gravity = player_->GetGravity();
	float scrollSpeed = (gravity > 0.0f) ? -0.1f : 0.1f;

	for (auto platform : platforms_) {
		platform->SetScrollSpeed(scrollSpeed);
		platform->Update();
	}

	// 画面外の足場を削除
	for (auto it = platforms_.begin(); it != platforms_.end();) {
		Vector3 pos = (*it)->GetWorldPosition();

		if (player_->IsInversion()) {
			if (pos.y > 22.0f) {
				delete *it;
				it = platforms_.erase(it);
				continue;
			}
		} else {
			if (pos.y < -22.0f) {
				delete *it;
				it = platforms_.erase(it);
				continue;
			}
		}
		++it;
	}

	if (platformSpawnTimer >= platformSpawnInterval) {
		platformSpawnTimer = 0.0f;

		float x;
		if (platformSideFlag) {
			// 左側範囲
			std::uniform_real_distribution<float> posX(-20.0f, 0.0f);
			x = posX(randomEngine_);
		} else {
			// 右側範囲
			std::uniform_real_distribution<float> posX(0.0f, 20.0f);
			x = posX(randomEngine_);
		}
		platformSideFlag = !platformSideFlag; // 毎回交互に切り替え

		Vector3 pos = {x, player_->IsInversion() ? 21.0f : -20.0f, 0.0f};
		Vector3 scale = {1.5f, 1.2f, 1.0f};
		Platform* platform = new Platform();
		platform->Initialize(pos, scale, modelPlatform_, &camera_);
		platforms_.push_back(platform);

		lastPlatformX = x; // 必要なら記憶
	}

	// =====================
	// プレイヤー更新
	// =====================
	player_->Update();

	// =====================
	// 衝突判定（横 + 縦 全部ここで処理）
	// =====================
	for (auto platform : platforms_) {
		const AABB& platformAABB = platform->GetAABB();
		const AABB& playerAABB = player_->GetAABB();

		if (!playerAABB.IsColliding(platformAABB)) {
			continue;
		}

		// プレイヤー中心とプラットフォーム中心の差
		Vector3 playerPos = player_->GetPosition();
		Vector3 platPos = platform->GetWorldPosition();

		Vector3 overlap; // 重なり量
		overlap.x = std::fmin(playerAABB.GetMax().x, platformAABB.GetMax().x) - std::fmax(playerAABB.GetMin().x, platformAABB.GetMin().x);
		overlap.y = std::fmin(playerAABB.GetMax().y, platformAABB.GetMax().y) - std::fmax(playerAABB.GetMin().y, platformAABB.GetMin().y);

		// 衝突解決（重なりが小さい方に押し戻す）
		if (overlap.x < overlap.y) {
			// 横衝突
			if (playerPos.x < platPos.x) {
				// 左からぶつかった
				playerPos.x -= overlap.x;
			} else {
				// 右からぶつかった
				playerPos.x += overlap.x;
			}
			player_->SetVelocityX(0.0f);
		} else {
			// 縦衝突
			if (playerPos.y > platPos.y) {
				// 上から着地
				playerPos.y += overlap.y;
				player_->SetVelocityY(0.0f);
				player_->SetOnGround(true);
			} else {
				// 下からぶつかった
				playerPos.y -= overlap.y;
				player_->SetVelocityY(0.0f);
				player_->SetOnGround(true);
			}
		}

		// 修正した座標を反映
		player_->SetPosition(playerPos);
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