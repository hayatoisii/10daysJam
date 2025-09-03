#include "GameScene.h"
#include <random>

GameScene::~GameScene() {
	// 動的確保したオブジェクトの解放
	delete player_;
	for (auto platform : platforms_) {
		delete platform;
	}
}

void GameScene::Initialize() {
	// 乱数エンジンの初期化
	std::random_device rd;
	randomEngine_ = std::mt19937(rd());

	// モデルの読み込み
	modelPlayer_ = KamataEngine::Model::CreateFromOBJ("cube", true);
	modelPlatform_ = KamataEngine::Model::CreateFromOBJ("platform", true);
	modelEnd_ = KamataEngine::Model::CreateFromOBJ("end", true);

    // 左端（-20, 0, 0）に配置
	endTransformLeft_.Initialize();
	endTransformLeft_.translation_ = Vector3(-22.3f, 0.0f, 0.0f);

	// 右端（20, 0, 0）に配置
	endTransformRight_.Initialize();
	endTransformRight_.translation_ = Vector3(22.3f, 0.0f, 0.0f);

	endTransformLeft_.UpdateMatarix();
	endTransformRight_.UpdateMatarix();

	// カメラ初期化
	camera_.Initialize();

	// プレイヤー初期化
	Vector3 playerPos = {0, 0, 0};
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPos);

	// 足場をランダムに初期生成
	std::uniform_real_distribution<float> posX(-20.0f, 20.0f);
	std::uniform_real_distribution<float> posY(-10.0f, 40.0f);

	const int platformCount = 10; // 足場の数
	for (int i = 0; i < platformCount; i++) {
		Vector3 pos = {posX(randomEngine_), posY(randomEngine_), 0.0f};
		Vector3 scale = {1.5f, 1.2f, 1.0f};

		Platform* platform = new Platform();
		platform->Initialize(pos, scale, modelPlatform_, &camera_);
		platforms_.push_back(platform);
	}

	// ワールドトランスフォームの初期化
	worldTransform.Initialize();
}

void GameScene::Update() {
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

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 2D描画前準備
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 2D描画（必要ならここに描画処理を書く）
	Sprite::PostDraw();

	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();

	// 3D描画前準備
	Model::PreDraw(dxCommon->GetCommandList());

	// 足場を描画
	for (auto platform : platforms_) {
		platform->Draw();
	}
	// プレイヤーを描画
	player_->Draw();

	if (modelEnd_) {
		modelEnd_->Draw(endTransformLeft_, camera_);
		modelEnd_->Draw(endTransformRight_, camera_);
	}

	// 3D描画終了処理
	Model::PostDraw();
}
