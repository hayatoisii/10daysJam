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

	// ▼▼▼ font_の解放処理を追加 ▼▼▼
	delete font_;
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

	// ★ アイテムモデルの読み込み
	modelPlatformItemSpeedReset_ = KamataEngine::Model::CreateFromOBJ("time", true); // アイテムモデルのファイル名

	modelBackground_ = KamataEngine::Model::CreateFromOBJ("backblack", true);
	transformBackground_.Initialize();
	// Z軸を奥にずらして配置（数値が大きいほど奥になります）
	transformBackground_.translation_ = {0.0f, 0.0f, 20.0f};
	transformBackground_.UpdateMatarix();

	// ▼▼▼ 「sky」背景スプライトの初期化を追加 ▼▼▼
	skyTextureHandle_ = TextureManager::Load("sky.png"); // skyじゃなくて　背景の岩、に変更する後で
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
	skySprite1_->SetPosition({windowWidth, windowHeight});
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
	spriteGravityLineTop_ = Sprite::Create(spriteGravityLineTopHandle_, {0.0f, 0.0f}); // 一旦白い画像で仮作成
	spriteGravityLineTop_->SetSize({640.0f, 100.0f});                                  // スクリーンの幅いっぱいに細い線
	spriteGravityLineTop_->SetPosition({320.0f, -4.0f});                               // 画面中央X、上部のY座標

	// 下ラインのスプライト
	spriteGravityLineBottomHandle_ = TextureManager::Load("Gravityline.png");
	spriteGravityLineBottom_ = Sprite::Create(spriteGravityLineBottomHandle_, {0.0f, 0.0f}); // 一旦白い画像で仮作成
	spriteGravityLineBottom_->SetSize({640.0f, 100.0f});                                     // スクリーンの幅いっぱいに細い線
	spriteGravityLineBottom_->SetPosition({320.0f, 625.0f});                                 // 画面中央X、下部のY座標
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
		Vector3 scale = {1.3f, 1.0f, 1.0f}; // 1.0でもいいかも
		// Initializeにアイテムモデルも渡す
		firstPlatform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, &camera_);
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
			platform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, &camera_);

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
			platform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, &camera_);
			platform->SetDamageDirection(DamageDirection::NONE);
		}
		platforms_.push_back(platform);
	}
	worldTransform.Initialize();

	font_ = new BIt_Map_Font();
	font_->Initialize();
}

void GameScene::Update() {
	// --- 1. 時間経過と全体スピードの更新 ---
	gameTime_ += 1.0f / 60.0f;
	speedMultiplier_ = 1.0f + (gameTime_ / 15.0f) * 1.5f;
	if (speedMultiplier_ > 200.0f) {
		speedMultiplier_ = 200.0f;
	}

	// --- 2. 足場の生成 ---
	platformSpawnTimer += 1.0f / 60.0f;
	if (platformSpawnTimer >= (platformSpawnInterval / speedMultiplier_) * spawnRateModifier) {
		platformSpawnTimer = 0.0f;
		float x;
		if (platformSideFlag) {
			std::uniform_real_distribution<float> posX(-17.0f, 0.0f);
			x = posX(randomEngine_);
		} else {
			std::uniform_real_distribution<float> posX(0.0f, 17.0f);
			x = posX(randomEngine_);
		}
		platformSideFlag = !platformSideFlag;
		Vector3 pos = {x, player_->IsInversion() ? 21.0f : -21.0f, 0.0f};
		Vector3 scale = {1.3f, 1.0f, 1.0f};
		Platform* platform = new Platform();
		std::uniform_int_distribution<int> dist10(0, 9);
		if (dist10(randomEngine_) < 6) {
			scale = {1.3f, 1.5f, 1.0f};
			platform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, &camera_);
			if (player_->IsInversion()) {
				platform->SetDamageDirection(DamageDirection::BOTTOM);
			} else {
				platform->SetDamageDirection(DamageDirection::TOP);
			}
			platform->SetDamageColliderScaleY(1.2f);
			platform->SetSafeSideScaleY(1.0f);
		} else {
			platform->Initialize(pos, scale, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, &camera_);
			platform->SetDamageDirection(DamageDirection::NONE);
			std::uniform_int_distribution<int> itemDist(0, 19);
			if (itemDist(randomEngine_) == 0) {
				platform->SetItemType(ItemType::SPEED_RESET);
			}
		}
		platforms_.push_back(platform);
		lastPlatformX = x;
	}

	// --- 3. 全ての足場をスクロールさせる ---
	float gravity = player_->GetGravity();
	float baseScrollSpeed = (gravity > 0.0f) ? -0.1f : 0.1f;
	float scrollSpeed = baseScrollSpeed * speedMultiplier_;
	for (auto platform : platforms_) {
		platform->SetScrollSpeed(scrollSpeed);
		platform->Update(player_->IsInversion());
	}

	// --- 4. プレイヤーの入力処理 ---
	player_->Update();

	// --- 5. プレイヤーの物理演算と移動 ---
	player_->InterpolateGravity();
	float velocityX = player_->GetVelocityX();
	float velocityY = player_->GetVelocityY();
	Vector3 playerPos = player_->GetPosition();
	if (!player_->IsOnGround()) {
		velocityY += player_->GetGravity();
	}
	if (!player_->IsInversion()) {
		if (velocityY < -player_->GetMaxFallSpeed()) {
			velocityY = -player_->GetMaxFallSpeed();
		}
	} else {
		if (velocityY > player_->GetMaxFallSpeed()) {
			velocityY = player_->GetMaxFallSpeed();
		}
	}
	player_->SetVelocityY(velocityY);
	playerPos.x += velocityX;
	playerPos.y += velocityY;
	player_->SetPosition(playerPos);

	// ★★★ ここからが追加ブロックです ★★★
	// X軸の移動範囲制限を適用する
	playerPos = player_->GetPosition(); // 最新の位置を取得
	if (playerPos.x < player_->GetMinX()) {
		playerPos.x = player_->GetMinX();
	}
	if (playerPos.x > player_->GetMaxX()) {
		playerPos.x = player_->GetMaxX();
	}
	player_->SetPosition(playerPos);
	// ★★★ 追加ブロックはここまで ★★★

	// --- 6. 当たり判定 ---
	player_->SetOnGround(false);

	for (auto platform : platforms_) {
		const AABB& platformAABB = platform->GetAABB();
		const AABB& playerAABB = player_->GetAABB();

		// このフレームの移動を包括するAABBを作成し、すり抜けを防ぐ
		playerPos = player_->GetPosition();
		Vector3 prevPlayerPos = player_->GetPrevPosition();
		Vector3 playerHalfSize = player_->GetHalfSize();
		AABB broadPhaseAABB;
		Vector3 minPoint = {fminf(playerPos.x, prevPlayerPos.x) - playerHalfSize.x, fminf(playerPos.y, prevPlayerPos.y) - playerHalfSize.y, 0.0f};
		Vector3 maxPoint = {fmaxf(playerPos.x, prevPlayerPos.x) + playerHalfSize.x, fmaxf(playerPos.y, prevPlayerPos.y) + playerHalfSize.y, 0.0f};
		broadPhaseAABB.Set(minPoint, maxPoint);

		if (!broadPhaseAABB.IsColliding(platformAABB)) {
			continue;
		}

		// 前フレームのプレイヤーのAABBを計算
		AABB prevPlayerAABB;
		prevPlayerAABB.Set(prevPlayerPos - playerHalfSize, prevPlayerPos + playerHalfSize);

		// ★★★ ここからが重要な変更点です ★★★
		// 浮動小数点数の誤差を許容するための「許容値(epsilon)」
		float epsilon = 0.01f;

		// 前フレームで、プレイヤーが足場の上下左右のどの位置にいたかを判定（許容値を追加）
		bool wasAbove = prevPlayerAABB.GetMin().y >= platformAABB.GetMax().y - epsilon;
		bool wasBelow = prevPlayerAABB.GetMax().y <= platformAABB.GetMin().y + epsilon;
		bool wasLeft = prevPlayerAABB.GetMax().x <= platformAABB.GetMin().x + epsilon;
		bool wasRight = prevPlayerAABB.GetMin().x >= platformAABB.GetMax().x - epsilon;

		playerPos = player_->GetPosition();
		bool collisionHandled = false;

		// --- 優先度1: 上下からの着地を判定 ---
		if (wasAbove && player_->GetVelocityY() <= 0.0f) {
			playerPos.y = platformAABB.GetMax().y + playerHalfSize.y;
			player_->SetOnGround(true);
			// (着地時の処理...)
			switch (platform->GetItemType()) {
			case ItemType::SPEED_RESET:
				gameTime_ -= 12.0f;
				if (gameTime_ < 0.0f) {
					gameTime_ = 0.0f;
				}
				platform->SetItemType(ItemType::NONE);
				break;
			default:
				break;
			}
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
			collisionHandled = true;
		} else if (wasBelow && player_->GetVelocityY() >= 0.0f) {
			playerPos.y = platformAABB.GetMin().y - playerHalfSize.y;
			player_->SetOnGround(true);
			// (着地時の処理...)
			switch (platform->GetItemType()) {
			case ItemType::SPEED_RESET:
				gameTime_ -= 12.0f;
				if (gameTime_ < 0.0f) {
					gameTime_ = 0.0f;
				}
				platform->SetItemType(ItemType::NONE);
				break;
			default:
				break;
			}
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
			collisionHandled = true;
		}

		// --- 優先度2: 左右からの壁衝突を判定 ---
		if (!collisionHandled && (wasLeft || wasRight)) {
			if (playerPos.x < platform->GetWorldPosition().x) {
				playerPos.x = platformAABB.GetMin().x - playerHalfSize.x;
			} else {
				playerPos.x = platformAABB.GetMax().x + playerHalfSize.x;
			}
			player_->SetVelocityX(0.0f);
			collisionHandled = true;
		}

		// --- 優先度3: フォールバック（緊急脱出）処理 ---
		if (!collisionHandled && playerAABB.IsColliding(platformAABB)) {
			// 上にも下にも横にもいなかった＝既にめり込んでいた場合、強制的に押し出す
			if (!player_->IsInversion()) {
				playerPos.y = platformAABB.GetMax().y + playerHalfSize.y;
				player_->SetOnGround(true);
			} else {
				playerPos.y = platformAABB.GetMin().y - playerHalfSize.y;
				player_->SetOnGround(true);
			}
		}

		player_->SetPosition(playerPos);
	}

	// --- 7. 画面端での重力反転処理 ---
	playerPos = player_->GetPosition();
	if (!player_->IsInversion()) {
		if (playerPos.y <= -16.7f) {
			playerPos.y = -16.7f;
			player_->SetPosition(playerPos);
			player_->TriggerGravityReversal();
		}
	} else {
		if (playerPos.y >= 16.7f) {
			playerPos.y = 16.7f;
			player_->SetPosition(playerPos);
			player_->TriggerGravityReversal();
		}
	}

	// --- 8. 背景の更新 ---
	if (skySprite1_ && skySprite2_) {
		const float spriteSpeedModifier = 17.0f;
		float spriteScrollSpeed = -scrollSpeed * spriteSpeedModifier;
		Vector2 skyPos1 = skySprite1_->GetPosition();
		skyPos1.y += spriteScrollSpeed;
		skySprite1_->SetPosition(skyPos1);
		Vector2 skyPos2 = skySprite2_->GetPosition();
		skyPos2.y += spriteScrollSpeed;
		skySprite2_->SetPosition(skyPos2);
		float windowHeight = (float)WinApp::kWindowHeight;
		if (spriteScrollSpeed > 0) {
			if (skyPos1.y > windowHeight + windowHeight / 2.0f) {
				skySprite1_->SetPosition({skyPos1.x, skyPos2.y - windowHeight});
			}
			if (skyPos2.y > windowHeight + windowHeight / 2.0f) {
				skySprite2_->SetPosition({skyPos2.x, skyPos1.y - windowHeight});
			}
		} else {
			if (skyPos1.y < -windowHeight / 2.0f) {
				skySprite1_->SetPosition({skyPos1.x, skyPos2.y + windowHeight});
			}
			if (skyPos2.y < -windowHeight / 2.0f) {
				skySprite2_->SetPosition({skyPos2.x, skyPos1.y + windowHeight});
			}
		}
	}

	// --- 9. 画面外の足場を削除 ---
	for (auto it = platforms_.begin(); it != platforms_.end();) {
		Vector3 pos = (*it)->GetWorldPosition();
		bool erased = false;
		if (player_->IsInversion()) {
			if (pos.y < -22.0f) {
				delete *it;
				it = platforms_.erase(it);
				erased = true;
			}
		} else {
			if (pos.y > 22.0f) {
				delete *it;
				it = platforms_.erase(it);
				erased = true;
			}
		}
		if (!erased) {
			++it;
		}
	}

	// --- 10. プレイヤーの行列を最終更新 ---
	player_->UpdateWorldMatrix();

	// --- 11. スコア加算 ---
	Vector3 currentPlayerPos = player_->GetPosition();
	bool isFallingOrJumping = (player_->GetVelocityY() != 0.0f) && !player_->IsOnGround();
	if (isFallingOrJumping && (currentPlayerPos.y != prevPlayerPos_.y)) {
		score_++;
	}
	prevPlayerPos_ = currentPlayerPos;
	prevOnGround_ = player_->IsOnGround();
	if (score_ != prevScore_) {
		font_->Set(score_);
		prevScore_ = score_;
	}

	// --- 12. HPが0になったらゲームオーバー ---
	if (playerHP_ <= 0) {
		// isGameOver_ = true;
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

	for (int i = 0; i < (int)hpWorldTransforms_.size(); i++) {
		if (i < (int)hpWorldTransforms_.size()) {
			// ポインタを間接参照(*)してオブジェクトを渡す
			hpModel_->Draw(*hpWorldTransforms_[i], camera_);
		}
	}

	font_->Draw();

	Model::PostDraw();
}