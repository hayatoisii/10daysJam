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
	score_ = 0;
	gameTime_ = 0.0f;

	// 乱数エンジンの初期化
	std::random_device rd;
	randomEngine_ = std::mt19937(rd());

	// モデルの読み込み
	modelPlayer_ = KamataEngine::Model::CreateFromOBJ("cube", true);
	modelPlatform_ = KamataEngine::Model::CreateFromOBJ("platform", true);
	modelEnd_ = KamataEngine::Model::CreateFromOBJ("end", true);
	hpModel_ = KamataEngine::Model::CreateFromOBJ("heart", true);
	modelDamageTop_ = KamataEngine::Model::CreateFromOBJ("platform_damage_top", true);
	modelDamageBottom_ = KamataEngine::Model::CreateFromOBJ("platform_damage_bottom", true);
	modelPlatformItemSpeedReset_ = KamataEngine::Model::CreateFromOBJ("time", true);

	modelBackground_ = KamataEngine::Model::CreateFromOBJ("backblack", true);
	transformBackground_.Initialize();
	transformBackground_.translation_ = {0.0f, 0.0f, 20.0f};
	transformBackground_.UpdateMatarix();

	// 背景スプライトの初期化
	skyTextureHandle_ = TextureManager::Load("sky.png");
	skySprite1_ = Sprite::Create(skyTextureHandle_, {0.0f, 0.0f});
	skySprite2_ = Sprite::Create(skyTextureHandle_, {0.0f, 0.0f});

	float windowWidth = (float)WinApp::kWindowWidth;
	float windowHeight = (float)WinApp::kWindowHeight;

	skySprite1_->SetSize({windowWidth, windowHeight});
	skySprite2_->SetSize({windowWidth, windowHeight});
	skySprite1_->SetAnchorPoint({0.5f, 0.5f});
	skySprite2_->SetAnchorPoint({0.5f, 0.5f});
	skySprite1_->SetPosition({windowWidth / 2.0f, windowHeight / 2.0f});
	skySprite2_->SetPosition({windowWidth / 2.0f, windowHeight / 2.0f - windowHeight});

	// HPのUI初期化
	for (int i = 0; i < playerHP_; i++) {
		WorldTransform* wt = new WorldTransform();
		wt->Initialize();
		wt->translation_ = {-33.5f + i * 5.0f, 18.0f, 0.0f};
		wt->scale_ = {1.5f, 1.5f, 0.5f};
		wt->UpdateMatarix();
		hpWorldTransforms_.push_back(wt);
	}

	// 左右の壁の初期化
	modelEnd_ = KamataEngine::Model::CreateFromOBJ("end", true);
	endTransformLeft_.Initialize();
	endTransformLeft_.translation_ = Vector3(-19.3f, 0.0f, 0.0f);
	endTransformRight_.Initialize();
	endTransformRight_.translation_ = Vector3(19.3f, 0.0f, 0.0f);
	endTransformLeft_.UpdateMatarix();
	endTransformRight_.UpdateMatarix();

	// 重力反転ラインのスプライト初期化
	spriteGravityLineTopHandle_ = TextureManager::Load("Gravityline.png");
	spriteGravityLineTop_ = Sprite::Create(spriteGravityLineTopHandle_, {0.0f, 0.0f});
	spriteGravityLineTop_->SetSize({640.0f, 100.0f});
	spriteGravityLineTop_->SetPosition({320.0f, -4.0f});

	spriteGravityLineBottomHandle_ = TextureManager::Load("Gravityline.png");
	spriteGravityLineBottom_ = Sprite::Create(spriteGravityLineBottomHandle_, {0.0f, 0.0f});
	spriteGravityLineBottom_->SetSize({640.0f, 100.0f});
	spriteGravityLineBottom_->SetPosition({320.0f, 625.0f});

	// カメラ初期化
	camera_.Initialize();

	// プレイヤー初期化
	Vector3 playerPos = {0, 5, 0}; // 少し上から開始
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPos);

	// --- 初期足場生成 ---
	{
		Platform* firstPlatform = new Platform();
		Vector3 pos = {0.0f, -2.0f, 0.0f};
		Vector3 scale = {1.3f, 1.0f, 1.0f};
		firstPlatform->Initialize(pos, scale, DamageDirection::NONE, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, &camera_);
		platforms_.push_back(firstPlatform);
	}

	std::uniform_real_distribution<float> posX(-20.0f, 20.0f);
	std::uniform_real_distribution<float> posY(0.0f, 40.0f);

	for (int i = 0; i < platformCount - 1; i++) {
		Vector3 pos = {posX(randomEngine_), posY(randomEngine_), 0.0f};
		Vector3 scale = {1.5f, 1.2f, 1.0f};
		Platform* platform = new Platform();

		std::uniform_int_distribution<int> dist01(0, 1);
		if (dist01(randomEngine_) == 1) {
			scale = {1.5f, 1.8f, 1.0f};
			DamageDirection dir = player_->IsInversion() ? DamageDirection::BOTTOM : DamageDirection::TOP;
			platform->Initialize(pos, scale, dir, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, &camera_);
			platform->SetDamageColliderYOffset(0.05f);
			platform->SetDamageColliderScaleY(0.7f);
			platform->SetSafeSideScaleY(0.8f);
		} else {
			platform->Initialize(pos, scale, DamageDirection::NONE, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, &camera_);
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
			DamageDirection dir = player_->IsInversion() ? DamageDirection::BOTTOM : DamageDirection::TOP;
			platform->Initialize(pos, scale, dir, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, &camera_);
			platform->SetDamageColliderScaleY(1.2f);
			platform->SetSafeSideScaleY(0.8f);
		} else {
			platform->Initialize(pos, scale, DamageDirection::NONE, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, &camera_);
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

	// X軸の移動範囲制限を適用する
	playerPos = player_->GetPosition();
	if (playerPos.x < player_->GetMinX()) {
		playerPos.x = player_->GetMinX();
	}
	if (playerPos.x > player_->GetMaxX()) {
		playerPos.x = player_->GetMaxX();
	}
	player_->SetPosition(playerPos);

	// --- 6. 当たり判定 ---
	player_->SetOnGround(false);

	for (auto platform : platforms_) {
		const AABB& platformAABB = platform->GetAABB();

		Vector3 prevPlayerPos = player_->GetPrevPosition();
		Vector3 playerHalfSize = player_->GetHalfSize();
		AABB broadPhaseAABB;
		Vector3 minPoint = {fminf(playerPos.x, prevPlayerPos.x) - playerHalfSize.x, fminf(playerPos.y, prevPlayerPos.y) - playerHalfSize.y, 0.0f};
		Vector3 maxPoint = {fmaxf(playerPos.x, prevPlayerPos.x) + playerHalfSize.x, fmaxf(playerPos.y, prevPlayerPos.y) + playerHalfSize.y, 0.0f};
		broadPhaseAABB.Set(minPoint, maxPoint);

		if (!broadPhaseAABB.IsColliding(platformAABB)) {
			continue;
		}

		const AABB& playerAABB = player_->GetAABB();
		AABB prevPlayerAABB;
		prevPlayerAABB.Set(prevPlayerPos - playerHalfSize, prevPlayerPos + playerHalfSize);

		float epsilon = 0.01f;
		bool wasAbove = prevPlayerAABB.GetMin().y >= platformAABB.GetMax().y - epsilon;
		bool wasBelow = prevPlayerAABB.GetMax().y <= platformAABB.GetMin().y + epsilon;
		bool wasLeft = prevPlayerAABB.GetMax().x <= platformAABB.GetMin().x + epsilon;
		bool wasRight = prevPlayerAABB.GetMin().x >= platformAABB.GetMax().x - epsilon;

		playerPos = player_->GetPosition();
		bool collisionHandled = false;

		if (wasAbove && player_->GetVelocityY() <= 0.0f) {
			playerPos.y = platformAABB.GetMax().y + playerHalfSize.y;
			player_->SetOnGround(true);
			collisionHandled = true;

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
		} else if (wasBelow && player_->GetVelocityY() >= 0.0f) {
			playerPos.y = platformAABB.GetMin().y - playerHalfSize.y;
			player_->SetOnGround(true);
			collisionHandled = true;

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
		}

		if (!collisionHandled && (wasLeft || wasRight)) {
			if (playerPos.x < platform->GetWorldPosition().x) {
				playerPos.x = platformAABB.GetMin().x - playerHalfSize.x;
			} else {
				playerPos.x = platformAABB.GetMax().x + playerHalfSize.x;
			}
			player_->SetVelocityX(0.0f);
			collisionHandled = true;
		}

		if (!collisionHandled && playerAABB.IsColliding(platformAABB)) {
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

	// --- 12. ゲームオーバー判定 ---
	if (playerHP_ <= 0) {
		isGameOver_ = true;
	}
	playerPos = player_->GetPosition();
	if (playerPos.y > 23.0f || playerPos.y < -23.0f) {
		isGameOver_ = true;
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