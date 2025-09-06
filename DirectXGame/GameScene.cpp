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
	// (この関数の中身は変更ありません)
	// 再初期化時のクリーンアップ
	if (player_ != nullptr) {
		delete player_;
		player_ = nullptr;
	}
	for (auto platform : platforms_) {
		delete platform;
	}
	platforms_.clear();
	for (auto wt : hpWorldTransforms_) {
		delete wt;
	}
	hpWorldTransforms_.clear();

	// 状態をリセット
	isGameOver_ = false;
	isGameClear_ = false;
	platformSpawnTimer = 0.0f;
	platformSideFlag = false;
	lastPlatformX = 0.0f;
	playerHP_ = 3;

	// 乱数エンジンの初期化
	std::random_device rd;
	randomEngine_ = std::mt19937(rd());

	// モデルの読み込み
	modelPlayer_ = KamataEngine::Model::CreateFromOBJ("cube", true);
	modelPlatform_ = KamataEngine::Model::CreateFromOBJ("platform", true);
	hpModel_ = KamataEngine::Model::CreateFromOBJ("cube", true); // HPモデル
	modelEnd_ = KamataEngine::Model::CreateFromOBJ("end", true);
	hpModel_ = KamataEngine::Model::CreateFromOBJ("heart", true);
	// 上下それぞれのダメージモデルを読み込む
	modelDamageTop_ = KamataEngine::Model::CreateFromOBJ("platform_damage_top", true);       // 上向きダメージモデルのファイル名にしてください
	modelDamageBottom_ = KamataEngine::Model::CreateFromOBJ("platform_damage_bottom", true); // 下向きダメージモデルのファイル名にしてください

	modelBackground_ = KamataEngine::Model::CreateFromOBJ("backblack", true);
	transformBackground_.Initialize();
	// Z軸を奥にずらして配置（数値が大きいほど奥になります）
	transformBackground_.translation_ = {0.0f, 0.0f, 20.0f};
	transformBackground_.UpdateMatarix();


		// ▼▼▼ 「sky」背景スプライトの初期化を追加 ▼▼▼
	skyTextureHandle_ = TextureManager::Load("sky.png");
	skySprite1_ = Sprite::Create(skyTextureHandle_, {0.0f, 0.0f});
	skySprite2_ = Sprite::Create(skyTextureHandle_, {0.0f, 0.0f});

	float windowWidth = 640;
	float windowHeight = 720;

	// 画像サイズを画面に合わせる
	skySprite1_->SetSize({windowWidth, windowHeight});
	skySprite2_->SetSize({windowWidth, windowHeight});

	// アンカーポイント（基準点）を画像の中心に設定
	skySprite1_->SetAnchorPoint({0.5f, 0.5f});
	skySprite2_->SetAnchorPoint({0.5f, 0.5f});

	// 1枚目を画面ぴったりに配置
	skySprite1_->SetPosition({windowWidth , windowHeight});
	// 2枚目を1枚目のすぐ上に、画面外に配置
	skySprite2_->SetPosition({windowWidth, windowHeight - windowHeight});
	// ▲▲▲ ---------------------------------- ▲▲▲

	// HPワールドトランスフォームを3個作成
	for (int i = 0; i < playerHP_; i++) {
		WorldTransform* wt = new WorldTransform();
		wt->Initialize();
		wt->translation_ = {-32.5f + i * 4.0f, 18.0f, 0.0f};
		wt->scale_ = {1.5f, 1.5f, 0.5f};
		wt->UpdateMatarix();
		hpWorldTransforms_.push_back(wt);
	}

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

	spriteGravityLineBottomHandle_ = TextureManager::Load("Gravityline.png");
	spriteGravityLineBottom_ = Sprite::Create(spriteGravityLineBottomHandle_, {0.0f, 0.0f});
	spriteGravityLineBottom_->SetSize({640.0f, 100.0f});
	spriteGravityLineBottom_->SetPosition({320.0f, 625.0f});

	camera_.Initialize();
	Vector3 playerPos = {0, 0, 0};
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPos);

	// 足場をランダムに初期生成
	std::uniform_real_distribution<float> posX(-20.0f, 20.0f);
	std::uniform_real_distribution<float> posY(0.0f, 40.0f);

	for (int i = 0; i < platformCount - 1; i++) {
		Vector3 pos = {posX(randomEngine_), posY(randomEngine_), 0.0f};
		Vector3 scale = {1.5f, 1.2f, 1.0f};
		bool isDamage = false;

		// 50% の確率で縦2倍サイズに変更
		std::uniform_int_distribution<int> dist01(0, 1);
		if (dist01(randomEngine_) == 1) {
			scale = {1.5f, 2.4f, 1.0f}; // 横はそのまま、縦だけ2倍
			isDamage = true;            // ダメージ足場フラグON
		}
		platforms_.push_back(platform);
	}
	worldTransform.Initialize();


	font_ = new BIt_Map_Font();
	font_->Initialize();
}

void GameScene::Update() {
	// 足場の生成タイミング管理
	platformSpawnTimer += 1.0f / 60.0f; // 60FPS想定
	if (platformSpawnTimer >= platformSpawnInterval) {
		platformSpawnTimer = 0.0f;

		std::uniform_real_distribution<float> posX(-15.0f, 15.0f);
		std::uniform_int_distribution<int> dist01(0, 1);

		Vector3 pos;
		if (player_->IsInversion()) {
			pos = {posX(randomEngine_), 21.0f, 0.0f};
		} else {
			pos = {posX(randomEngine_), -20.0f, 0.0f};
		}

		pos = {x, player_->IsInversion() ? 21.0f : -21.0f, 0.0f};
		// Vector3 scale = {1.5f, 1.2f, 1.0f};
		Vector3 scale = {1.3f, 1.0f, 1.0f}; // 1.0でもいいかも

		Platform* platform = new Platform();

		// 60%の確率でダメージ足場を生成
		std::uniform_int_distribution<int> dist10(0, 9); // 0から9の乱数を生成
		if (dist10(randomEngine_) < 6) {
			// scale = {1.5f, 1.8f, 1.0f};
			scale = {1.3f, 1.5f, 1.0f}; // 1.0でもいいかも
			platform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, &camera_);

			// プレイヤーの重力方向に応じて危険な面を設定
			if (player_->IsInversion()) {
				platform->SetDamageDirection(DamageDirection::BOTTOM);
			} else {
				platform->SetDamageDirection(DamageDirection::TOP);
			}
			// ダメージ足場の当たり判定の厚み（危険側のみ反映）
			platform->SetDamageColliderScaleY(1.3f);
			// 安全側（ダメージじゃない方）を少し小さく
			platform->SetSafeSideScaleY(1.0f);
		} else {
			platform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, &camera_);
			platform->SetDamageDirection(DamageDirection::NONE);
		}
		platforms_.push_back(platform);
		lastPlatformX = x;
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

	player_->Update();

	// --- 衝突判定 ---
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
			if (playerPos.x < platPos.x) {
				playerPos.x -= overlap.x;
			} else {
				playerPos.x += overlap.x;
			}
			player_->SetVelocityX(0.0f);
		} else {
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

		if (platform->IsDamage()) {
			// プレイヤーにダメージ処理を通知
		//	player_->TakeDamage(player_->GetPosition());
		}

		// 修正した座標を反映
		player_->SetPosition(playerPos);

		if (platform->IsDamage()) {
			if (playerHP_ > 0) {
				playerHP_--;

				if (playerHP_ < (int)hpWorldTransforms_.size()) {
					// アロー演算子(->)でメンバにアクセス
					hpWorldTransforms_[playerHP_]->scale_ = {0, 0, 0}; // 消す代わりに縮小
					hpWorldTransforms_[playerHP_]->UpdateMatarix();
				}
			}
		}
	}
}

void GameScene::Draw() {
	// (この関数の中身は変更ありません)
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());


	if (modelBackground_) {
		modelBackground_->Draw(transformBackground_, camera_);
	}
	Model::PostDraw();

	dxCommon->ClearDepthBuffer();

	// 2D描画前準備
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 2D描画（必要ならここに描画処理を書く）
	Sprite::PostDraw();

	dxCommon->ClearDepthBuffer();

	Model::PreDraw(dxCommon->GetCommandList());

	for (auto platform : platforms_) {
		platform->Draw();
	}
	player_->Draw();

	if (modelEnd_) {
		modelEnd_->Draw(endTransformLeft_, camera_);
		modelEnd_->Draw(endTransformRight_, camera_);
	}
	for (int i = 0; i < playerHP_; i++) {
		if (i < (int)hpWorldTransforms_.size()) {
			hpModel_->Draw(*hpWorldTransforms_[i], camera_);
		}
	}
	font_->Draw();

	Model::PostDraw();
}