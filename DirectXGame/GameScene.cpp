#include "GameScene.h"
#include <random>

GameScene::~GameScene() {
	// 動的確保したオブジェクトの解放
	delete player_;
	for (auto platform : platforms_) {
		delete platform;
	}

	for (auto spike : spikes_) {
		delete spike;
	}
	delete graph_;
	graph_ = nullptr;
}


void GameScene::Initialize() {
	// 乱数エンジンの初期化
	std::random_device rd;
	randomEngine_ = std::mt19937(rd());

	// モデルの読み込み
	modelPlayer_ = KamataEngine::Model::CreateFromOBJ("cube", true);
	modelPlatform_ = KamataEngine::Model::CreateFromOBJ("platform", true);

	modelBluePorl_ = KamataEngine::Model::CreateFromOBJ("bluePorl", true);

	// とげのモデルを読み込む（例: spike.obj）
	modelSpike_ = KamataEngine::Model::CreateFromOBJ("toge", true); 

	bluePorlTransform_.Initialize();
	bluePorlTransform_.translation_ = bluePorlPos_;

	// カメラ初期化
	camera_.Initialize();

	// プレイヤー初期化
	Vector3 playerPos = {0, 0, 0};
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPos);

	prevPlayerPos_ = playerPos; // 追加

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

	graph_ = new Graph();
	graph_->Initialize();

	font_ = new BIt_Map_Font();
	font_->Initialize();
}

void GameScene::Update() {

	bluePorlTransform_.translation_.y += 720.0f;
	bluePorlTransform_.TransferMatrix();

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

	// とげの生成タイミング管理
	float spikeSpawnInterval = 3.0f; // とげの生成間隔（秒）
	static float spikeSpawnTimer = 0.0f;
	spikeSpawnTimer += 1.0f / 60.0f;

	if (spikeSpawnTimer >= spikeSpawnInterval) {
		spikeSpawnTimer = 0.0f;

		// プレイヤーの重力方向に応じて生成位置を調整
		float spawnY = player_->IsInversion() ? 21.0f : -20.0f;

		std::uniform_real_distribution<float> posX(-15.0f, 15.0f);
		Vector3 pos = {posX(randomEngine_), spawnY, 0.0f};
		Vector3 scale = {1.0f, 1.0f, 1.0f};

		Spike* spike = new Spike();
		spike->Initialize(pos, scale, modelSpike_, &camera_);
		spikes_.push_back(spike);
	}

	// とげを更新し、画面外のものを削除
	//float scrollSpeed = (player_->GetGravity() > 0.0f) ? -0.1f : 0.1f;
	for (auto it = spikes_.begin(); it != spikes_.end();) {
		(*it)->SetScrollSpeed(scrollSpeed);
		(*it)->Update();

		Vector3 pos = (*it)->GetWorldPosition();
		if (player_->IsInversion()) {
			if (pos.y > 22.0f) {
				delete *it;
				it = spikes_.erase(it);
				continue;
			}
		} else {
			if (pos.y < -22.0f) {
				delete *it;
				it = spikes_.erase(it);
				continue;
			}
		}
		++it;
	}

	// =====================
	// プレイヤー更新
	// =====================

	// ゲームオーバーなら更新を停止
	if (isGameOver_) {
		return;
	}


	// プレイヤーの更新
	player_->Update();

	// スパイクとの衝突判定
	for (auto spike : spikes_) {
		const AABB& spikeAABB = spike->GetAABB();
		const AABB& playerAABB = player_->GetAABB();

		if (playerAABB.IsColliding(spikeAABB)) {
			// とげに衝突した場合
			playerHP_--; // HPを1減らす

			if (playerHP_ <= 0) {
				isGameOver_ = true;
				// ここでゲームオーバー時の処理を記述する（例: リスタートボタンを表示するなど）
				OutputDebugStringA("Game Over!\n");
			}

			// 一時的にプレイヤーを無敵状態にするなど、連続ヒットを防ぐ処理を入れると良い
			// 現状はHPが減るだけ
		}
	}

	// HP表示の更新
	graph_->Update(playerHP_);

	// プレイヤーの現在位置
	Vector3 currentPlayerPos = player_->GetPosition();

	// 落下・ジャンプ中か判定
	bool isFallingOrJumping = (player_->GetVelocityY() != 0.0f) && !player_->IsOnGround();

	// Y座標が変化している場合のみスコア加算
	if (isFallingOrJumping && (currentPlayerPos.y != prevPlayerPos_.y)) {
		score_++;
	}

	prevPlayerPos_ = currentPlayerPos;
	prevOnGround_ = player_->IsOnGround();

	if (score_ != prevScore_) {
		font_->Set(score_);
		prevScore_ = score_;
	}

	// =====================
	// 衝突判定（横 + 縦 全部ここで処理）
	// =====================
	bool onGroundThisFrame = false; // ★追加：現在のフレームで着地しているかを判定するフラグ
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
				onGroundThisFrame = true; // ★修正：フラグを立てる
			} else {
				// 下からぶつかった
				playerPos.y -= overlap.y;
				player_->SetVelocityY(0.0f);
				onGroundThisFrame = true; // ★修正：フラグを立てる
			}
		}

		// 修正した座標を反映
		player_->SetPosition(playerPos);
	}

	// ★追加：ループの最後にまとめてプレイヤーの地面状態を更新
	player_->SetOnGround(onGroundThisFrame);
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 2D描画前準備
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 2D描画（必要ならここに描画処理を書く）

	graph_->Draw();

	Sprite::PostDraw();

	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();

	// 3D描画前準備
	Model::PreDraw(dxCommon->GetCommandList());

	// プレイヤーを描画
	player_->Draw();
	
	// 足場を描画
	for (auto platform : platforms_) {
		platform->Draw();
	}

	// とげを描画
	for (auto spike : spikes_) {
		spike->Draw();
	}


	// bluePorlモデルを描画
	modelBluePorl_->Draw(bluePorlTransform_, camera_);

	font_->Draw();

	// 3D描画終了処理
	Model::PostDraw();
}