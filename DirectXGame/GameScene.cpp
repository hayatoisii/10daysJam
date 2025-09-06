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

	delete skySprite1_;
	delete skySprite2_;
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
	modelEnd_ = KamataEngine::Model::CreateFromOBJ("end", true);
	hpModel_ = KamataEngine::Model::CreateFromOBJ("heart", true);
	// 上下それぞれのダメージモデルを読み込む
	modelDamageTop_ = KamataEngine::Model::CreateFromOBJ("platform_damage_top", true);       // 上向きダメージモデルのファイル名にしてください
	modelDamageBottom_ = KamataEngine::Model::CreateFromOBJ("platform_damage_bottom", true); // 下向きダメージモデルのファイル名にしてください
	modelDamageTop_ = KamataEngine::Model::CreateFromOBJ("platform_damage_top", true);
	modelDamageBottom_ = KamataEngine::Model::CreateFromOBJ("platform_damage_bottom", true);

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
		wt->translation_ = {-33.5f + i * 5.0f, 18.0f, 0.0f};
		wt->scale_ = {1.5f, 1.5f, 0.5f};
		wt->UpdateMatarix();
		hpWorldTransforms_.push_back(wt);
	}

	// プレイヤーのX軸移動範囲を可視化するモデル
	modelEnd_ = KamataEngine::Model::CreateFromOBJ("end", true);

	// 左端（-20, 0, 0）に配置
	endTransformLeft_.Initialize();
	endTransformLeft_.translation_ = Vector3(-19.3f, 0.0f, 0.0f); // 15でもいいかも 16  20

	// 右端（20, 0, 0）に配置
	endTransformRight_.Initialize();
	endTransformRight_.translation_ = Vector3(19.3f, 0.0f, 0.0f); // 15  16  20

	endTransformLeft_.UpdateMatarix();
	endTransformRight_.UpdateMatarix();

	// ▼▼▼ 重力反転ライン用スプライトの初期化を追加 ▼▼▼
	// 上ラインのスプライト（適切な画像ファイル名に置き換えてください。例: "gravityLineTop.png"）
	spriteGravityLineTopHandle_ = TextureManager::Load("Gravityline.png");
	spriteGravityLineTop_ = Sprite::Create(spriteGravityLineTopHandle_, {0.0f, 0.0f});               // 一旦白い画像で仮作成
	spriteGravityLineTop_->SetSize({640.0f, 100.0f});                                                 // スクリーンの幅いっぱいに細い線
	spriteGravityLineTop_->SetPosition({320.0f, -4.0f});                                             // 画面中央X、上部のY座標

	// 下ラインのスプライト
	spriteGravityLineBottomHandle_ = TextureManager::Load("Gravityline.png");
	spriteGravityLineBottom_ = Sprite::Create(spriteGravityLineBottomHandle_, {0.0f, 0.0f});            // 一旦白い画像で仮作成
	spriteGravityLineBottom_->SetSize({640.0f, 100.0f});                                                 // スクリーンの幅いっぱいに細い線
	spriteGravityLineBottom_->SetPosition({320.0f, 625.0f});              // 画面中央X、下部のY座標
	// ▲▲▲ ---------------------------------- ▲▲▲

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
		// Vector3 scale = {1.5f, 1.2f, 1.0f};
		Vector3 scale = {1.3f, 1.0f, 1.0f}; // 1.0でもいいかも
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
	worldTransform.Initialize();


	font_ = new BIt_Map_Font();
	font_->Initialize();
}

void GameScene::Update() {
	gameTime_ += 1.0f / 60.0f;
	speedMultiplier_ = 1.0f + (gameTime_ / 30.0f) * 1.0f;
	if (speedMultiplier_ > 200.0f) {
		speedMultiplier_ = 200.0f;
	}
	platformSpawnTimer += 1.0f / 60.0f;
	if (platformSpawnTimer >= (platformSpawnInterval / speedMultiplier_) * spawnRateModifier) {
		platformSpawnTimer = 0.0f;

		// X座標を左右交互の範囲で決める
		float x;
		if (platformSideFlag) {
			std::uniform_real_distribution<float> posX(-17.0f, 0.0f); // 13でもいいかも　14　　　20
			x = posX(randomEngine_);
		} else {
			std::uniform_real_distribution<float> posX(0.0f, 17.0f); // 13ｄもいいかも　14　　　20
			x = posX(randomEngine_);
		}
		platformSideFlag = !platformSideFlag;

		Vector3 pos = {x, player_->IsInversion() ? 21.0f : -21.0f, 0.0f};
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

	if (skySprite1_ && skySprite2_) {
		// スプライトはピクセル単位で動くため、速度を調整
		const float spriteSpeedModifier = 17.0f;
		float spriteScrollSpeed = -scrollSpeed * spriteSpeedModifier;

		// 1枚目のスプライトを動かす
		Vector2 skyPos1 = skySprite1_->GetPosition();
		skyPos1.y += spriteScrollSpeed;
		skySprite1_->SetPosition(skyPos1);

		// 2枚目のスプライトを動かす
		Vector2 skyPos2 = skySprite2_->GetPosition();
		skyPos2.y += spriteScrollSpeed;
		skySprite2_->SetPosition(skyPos2);

		// --- ループ処理 ---
		float windowHeight = (float)WinApp::kWindowHeight;
		// スクロールが下向きの場合
		if (spriteScrollSpeed > 0) {
			if (skyPos1.y > windowHeight + windowHeight / 2.0f) {
				skySprite1_->SetPosition({skyPos1.x, skyPos2.y - windowHeight});
			}
			if (skyPos2.y > windowHeight + windowHeight / 2.0f) {
				skySprite2_->SetPosition({skyPos2.x, skyPos1.y - windowHeight});
			}
		}
		// スクロールが上向きの場合
		else {
			if (skyPos1.y < -windowHeight / 2.0f) {
				skySprite1_->SetPosition({skyPos1.x, skyPos2.y + windowHeight});
			}
			if (skyPos2.y < -windowHeight / 2.0f) {
				skySprite2_->SetPosition({skyPos2.x, skyPos1.y + windowHeight});
			}
		}
	}

	// 画面外の足場を削除
	for (auto it = platforms_.begin(); it != platforms_.end();) {
		Vector3 pos = (*it)->GetWorldPosition();
		bool erased = false; // 削除されたかどうかのフラグ

		if (player_->IsInversion()) {
			// ▼▼▼ 条件を修正 ▼▼▼
			// プレイヤーが上向き ⇒ 足場は下にスクロールするので、画面の下端で消す
			if (pos.y < -22.0f) {
				delete *it;
				it = platforms_.erase(it);
				erased = true;
			}
		} else {
			// ▼▼▼ 条件を修正 ▼▼▼
			// プレイヤーが下向き ⇒ 足場は上にスクロールするので、画面の上端で消す
			if (pos.y > 22.0f) {
				delete *it;
				it = platforms_.erase(it);
				erased = true;
			}
		}

		// 削除されなかった場合のみ、イテレータを進める
		if (!erased) {
			++it;
		}
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
		if (overlap.x < overlap.y) {
			if (playerPos.x < platPos.x) {
				playerPos.x -= overlap.x;
			} else {
				playerPos.x += overlap.x;
			}
			player_->SetVelocityX(0.0f);
		} else {
			if (playerPos.y > platPos.y) {
				Vector3 playerSize = playerAABB.GetMax() - playerAABB.GetMin();
				float playerHalfHeight = playerSize.y * 0.5f;
				playerPos.y = platformAABB.GetMax().y + playerHalfHeight + 0.01f;
				player_->SetVelocityY(0.0f);
				player_->SetOnGround(true);
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
				Vector3 playerSize = playerAABB.GetMax() - playerAABB.GetMin();
				float playerHalfHeight = playerSize.y * 0.5f;
				playerPos.y = platformAABB.GetMin().y - playerHalfHeight - 0.01f;
				player_->SetVelocityY(0.0f);
				player_->SetOnGround(true);
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
		player_->SetPosition(playerPos);
	}

	// ▼▼▼ ここから修正・追加 ▼▼▼
	// HPが0以下になったらゲームオーバーフラグを立てる
	if (playerHP_ <= 0) {
		//isGameOver_ = true;
	}
	// ▲▲▲ ここまで修正・追加 ▲▲▲
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
	if (skySprite1_) {
		skySprite1_->Draw();
	}
	if (skySprite2_) {
		skySprite2_->Draw();
	}
	if (spriteGravityLineTop_) {
		spriteGravityLineTop_->Draw();
	}
	if (spriteGravityLineBottom_) {
		spriteGravityLineBottom_->Draw();
	}
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