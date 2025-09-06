#include "GameScene.h"
#include <random>

GameScene::~GameScene() {
	delete player_;
	for (auto platform : platforms_) {
		delete platform;
	}
	for (auto wt : hpWorldTransforms_) {
		delete wt;
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
	hpModel_ = KamataEngine::Model::CreateFromOBJ("cube", true);
	// 上下それぞれのダメージモデルを読み込む
	modelDamageTop_ = KamataEngine::Model::CreateFromOBJ("platform_damage_top", true);       // 上向きダメージモデルのファイル名にしてください
	modelDamageBottom_ = KamataEngine::Model::CreateFromOBJ("platform_damage_bottom", true); // 下向きダメージモデルのファイル名にしてください

	// HPワールドトランスフォームを3個作成
	for (int i = 0; i < playerHP_; i++) {
		WorldTransform* wt = new WorldTransform();
		wt->Initialize();
		wt->translation_ = {-34.0f + i * 4.0f, 18.0f, 0.0f};
		wt->scale_ = {1.5f, 1.5f, 0.5f};
		wt->UpdateMatarix();
		hpWorldTransforms_.push_back(wt);
	}

	// プレイヤーのX軸移動範囲を可視化するモデル
	modelEnd_ = KamataEngine::Model::CreateFromOBJ("end", true);

	// 左端（-20, 0, 0）に配置
	endTransformLeft_.Initialize();
	endTransformLeft_.translation_ = Vector3(-16.3f, 0.0f, 0.0f);//15でもいいかも

	// 右端（20, 0, 0）に配置
	endTransformRight_.Initialize();
	endTransformRight_.translation_ = Vector3(16.3f, 0.0f, 0.0f);//15

	endTransformLeft_.UpdateMatarix();
	endTransformRight_.UpdateMatarix();

	// 重力反転ライン用のcubeモデルを読み込み
	modelGravityLineTop_ = KamataEngine::Model::CreateFromOBJ("GravityInversionTop", true);
	modelGravityLineBottom_ = KamataEngine::Model::CreateFromOBJ("GravityInversionBottom", true); // 例：下部用のモデルファイル

	// 上ライン（Y=25）に配置
	gravityLineTop_.Initialize();
	gravityLineTop_.translation_ = Vector3(0.0f, 15.0f, 1.3f);
	gravityLineTop_.scale_ = Vector3(1.0f, 1.0f, 1.0f); // 通常サイズのcube

	// 下ライン（Y=-25）に配置
	gravityLineBottom_.Initialize();
	gravityLineBottom_.translation_ = Vector3(0.0f, -15.0f, 1.3f);
	gravityLineBottom_.scale_ = Vector3(1.0f, 1.0f, 1.0f); // 通常サイズのcube

	gravityLineTop_.UpdateMatarix();
	gravityLineBottom_.UpdateMatarix();

	// カメラ初期化
	camera_.Initialize();

	// プレイヤー初期化
	Vector3 playerPos = {0, 0, 0};
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPos);

	// --- 足場生成処理 ---

	// 1. プレイヤーの真下に安全な足場を1つ生成する
	{
		Platform* firstPlatform = new Platform();
		Vector3 pos = {0.0f, -2.0f, 0.0f};
		//Vector3 scale = {1.5f, 1.2f, 1.0f};
		Vector3 scale = {1.0f, 1.0f, 1.0f};
		// Initializeに3種類のモデルを全て渡す
		firstPlatform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, &camera_);
		firstPlatform->SetDamageDirection(DamageDirection::NONE); // 無害
		platforms_.push_back(firstPlatform);
	}

	// 2. 残りの足場をランダムに生成する
	std::uniform_real_distribution<float> posX(-20.0f, 20.0f);
	std::uniform_real_distribution<float> posY(0.0f, 40.0f);

	for (int i = 0; i < platformCount - 1; i++) {
		Vector3 pos = {posX(randomEngine_), posY(randomEngine_), 0.0f};
		Vector3 scale = {1.5f, 1.2f, 1.0f}; // 通常スケール

		Platform* platform = new Platform();

		// 50%の確率でダメージ足場を生成
		std::uniform_int_distribution<int> dist01(0, 1);
		if (dist01(randomEngine_) == 1) {
			// ダメージ床の場合、スケールを1.5倍に変更
			scale = {1.5f, 1.8f, 1.0f};
			platform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, &camera_);

			// プレイヤーの重力方向に応じて危険な面を設定
			if (player_->IsInversion()) {
				platform->SetDamageDirection(DamageDirection::BOTTOM);
			} else {
				platform->SetDamageDirection(DamageDirection::TOP);
			}
			// ダメージ足場の当たり判定を少しだけ上に（小さめ）+ 高さを薄く
			platform->SetDamageColliderYOffset(0.05f);
			platform->SetDamageColliderScaleY(0.7f);
			// 安全側（ダメージじゃない方）を少し小さく
			platform->SetSafeSideScaleY(0.8f);
		} else {
			// 通常の足場
			platform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, &camera_);
			platform->SetDamageDirection(DamageDirection::NONE);
		}
		platforms_.push_back(platform);
	}

	// ワールドトランスフォームの初期化
	worldTransform.Initialize();
}

void GameScene::Update() {
	// ゲーム時間の更新
	gameTime_ += 1.0f / 60.0f; // 60FPS想定

	// 速度倍率の計算（30秒ごとに0.1倍増加、最大3.0倍）
	speedMultiplier_ = 1.0f + (gameTime_ / 30.0f) * 0.5f;
	if (speedMultiplier_ > 100.0f) {
		speedMultiplier_ = 100.0f;
	}

	// 足場の生成タイミング管理
	platformSpawnTimer += 1.0f / 60.0f; // 60FPS想定

	// --- 足場生成ロジック ---
	if (platformSpawnTimer >= (platformSpawnInterval / speedMultiplier_) * spawnRateModifier) {
		platformSpawnTimer = 0.0f;

		// X座標を左右交互の範囲で決める
		float x;
		if (platformSideFlag) {
			std::uniform_real_distribution<float> posX(-14.0f, 0.0f);// 13でもいいかも
			x = posX(randomEngine_);
		} else {
			std::uniform_real_distribution<float> posX(0.0f, 14.0f);// 13ｄもいいかも
			x = posX(randomEngine_);
		}
		platformSideFlag = !platformSideFlag;

		Vector3 pos = {x, player_->IsInversion() ? 21.0f : -20.0f, 0.0f};
		//Vector3 scale = {1.5f, 1.2f, 1.0f};
		Vector3 scale = {1.0f, 1.0f, 1.0f};

		Platform* platform = new Platform();

        // 60%の確率でダメージ足場を生成
		std::uniform_int_distribution<int> dist10(0, 9); // 0から9の乱数を生成
		if (dist10(randomEngine_) < 6) {
			//scale = {1.5f, 1.8f, 1.0f};
			scale = {1.0f, 1.0f, 1.0f};
			platform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, &camera_);

			// プレイヤーの重力方向に応じて危険な面を設定
			if (player_->IsInversion()) {
				platform->SetDamageDirection(DamageDirection::BOTTOM);
			} else {
				platform->SetDamageDirection(DamageDirection::TOP);
			}
			// ダメージ足場の当たり判定の厚み（危険側のみ反映）
			platform->SetDamageColliderScaleY(1.4f);
			// 安全側（ダメージじゃない方）を少し小さく
			platform->SetSafeSideScaleY(1.0f);
		} else {
			platform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, &camera_);
			platform->SetDamageDirection(DamageDirection::NONE);
		}
		platforms_.push_back(platform);
		lastPlatformX = x;
	}

	// プレイヤーの重力に応じたスクロール（速度倍率を適用）
	float gravity = player_->GetGravity();
	float baseScrollSpeed = (gravity > 0.0f) ? -0.1f : 0.1f;
	float scrollSpeed = baseScrollSpeed * speedMultiplier_;

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

	player_->Update();

	// --- 衝突判定 ---
	for (auto platform : platforms_) {
		const AABB& platformAABB = platform->GetAABB();
		const AABB& playerAABB = player_->GetAABB();

		if (!playerAABB.IsColliding(platformAABB)) {
			continue;
		}

		Vector3 playerPos = player_->GetPosition();
		Vector3 platPos = platform->GetWorldPosition();

		Vector3 overlap;
		overlap.x = std::fmin(playerAABB.GetMax().x, platformAABB.GetMax().x) - std::fmax(playerAABB.GetMin().x, platformAABB.GetMin().x);
		overlap.y = std::fmin(playerAABB.GetMax().y, platformAABB.GetMax().y) - std::fmax(playerAABB.GetMin().y, platformAABB.GetMin().y);

		// 衝突解決
		if (overlap.x < overlap.y) {
			// 横衝突
			if (playerPos.x < platPos.x) {
				playerPos.x -= overlap.x;
			} else {
				playerPos.x += overlap.x;
			}
			player_->SetVelocityX(0.0f);
		} else {
			// 縦衝突
			if (playerPos.y > platPos.y) {
				// 上から着地した場合
				// プレイヤーの底辺を足場の上面に正確に配置
				Vector3 playerSize = playerAABB.GetMax() - playerAABB.GetMin();
				Vector3 platformSize = platformAABB.GetMax() - platformAABB.GetMin();
				float playerHalfHeight = playerSize.y * 0.5f;
				// float platformHalfHeight = platformSize.y * 0.5f;
				playerPos.y = platformAABB.GetMax().y + playerHalfHeight + 0.01f; // 完全に上に配置
				player_->SetVelocityY(0.0f);
				player_->SetOnGround(true);

				// 足場の上面が危険な場合、ダメージを受ける
				if (platform->GetDamageDirection() == DamageDirection::TOP) {
					if (!player_->IsInvincible()) {
						if (playerHP_ > 0) {
							playerHP_--;
							player_->OnDamage();
							if (playerHP_ < (int)hpWorldTransforms_.size()) {
								hpWorldTransforms_[playerHP_]->scale_ = {0, 0, 0};
								hpWorldTransforms_[playerHP_]->UpdateMatarix();
							}
						}
					}
				}
			} else {
				// 下からぶつかった場合
				// プレイヤーの上辺を足場の下面に正確に配置
				Vector3 playerSize = playerAABB.GetMax() - playerAABB.GetMin();
				Vector3 platformSize = platformAABB.GetMax() - platformAABB.GetMin();
				float playerHalfHeight = playerSize.y * 0.5f;
				// float platformHalfHeight = platformSize.y * 0.5f;
				playerPos.y = platformAABB.GetMin().y - playerHalfHeight - 0.01f; // 完全に下に配置
				player_->SetVelocityY(0.0f);
				player_->SetOnGround(true);

				// 足場の下面が危険な場合、ダメージを受ける
				if (platform->GetDamageDirection() == DamageDirection::BOTTOM) {
					if (!player_->IsInvincible()) {
						if (playerHP_ > 0) {
							playerHP_--;
							player_->OnDamage();
							if (playerHP_ < (int)hpWorldTransforms_.size()) {
								hpWorldTransforms_[playerHP_]->scale_ = {0, 0, 0};
								hpWorldTransforms_[playerHP_]->UpdateMatarix();
							}
						}
					}
				}
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

	// プレイヤーのX軸移動範囲を可視化
	if (modelEnd_) {
		modelEnd_->Draw(endTransformLeft_, camera_);
		modelEnd_->Draw(endTransformRight_, camera_);
	}

	// 上下の重力反転ラインをそれぞれ描画
	if (modelGravityLineTop_) {
		modelGravityLineTop_->Draw(gravityLineTop_, camera_);
	}
	if (modelGravityLineBottom_) {
		modelGravityLineBottom_->Draw(gravityLineBottom_, camera_);
	}

	// HP描画
	for (int i = 0; i < playerHP_; i++) {
		if (i < (int)hpWorldTransforms_.size()) {
			// ポインタを間接参照(*)してオブジェクトを渡す
			hpModel_->Draw(*hpWorldTransforms_[i], camera_);
		}
	}

	// 3D描画終了処理
	Model::PostDraw();
}
