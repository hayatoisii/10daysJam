#include "TutorialScene.h"

TutorialScene::~TutorialScene() {
	delete player_;
	for (auto platform : platforms_) {
		delete platform;
	}
	for (auto wt : hpWorldTransforms_) {
		delete wt;
	}

	delete skySprite1_;
	delete skySprite2_;

	delete font_;

	// ▼▼▼ ここから追加 ▼▼▼
	// チュートリアル用スプライトの解放
	delete tutorialStageSprite_;
	delete tutorialStageSprite_1;
	delete tutorialStageSprite_2;
	delete tutorialStageSprite_3;
	delete tutorialStageSprite_4;
	delete tutorialStageSprite_5;
	delete tutorialStageSprite_6;
	delete tutorialStageSprite_7;
	delete tutorialStageSprite_8;
	// ▲▲▲ ここまで追加 ▲▲▲
}

void TutorialScene::Initialize() {
	input_ = KamataEngine::Input::GetInstance();
	isFinished_ = false;

	//// チュートリアル用の画像（例: "tutorial.png"）を読み込む
	//// この画像はプロジェクトに用意してください
	//textureHandle_ = KamataEngine::TextureManager::Load("tutorial.png");
	//tutorialSprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});


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
	gameOverTimer_ = 0.0f; // ゲームオーバータイマーをリセット
	isTutorialPhase2_ = false;

	// 乱数エンジンの初期化
	std::random_device rd;
	randomEngine_ = std::mt19937(rd());

	// モデルの読み込み
	modelPlayer_ = KamataEngine::Model::CreateFromOBJ("player", true);
	modelPlatform_ = KamataEngine::Model::CreateFromOBJ("platform", true);
	modelEnd_ = KamataEngine::Model::CreateFromOBJ("end", true);
	hpModel_ = KamataEngine::Model::CreateFromOBJ("heart", true);
	modelDamageTop_ = KamataEngine::Model::CreateFromOBJ("platform_damage_top", true);
	modelDamageBottom_ = KamataEngine::Model::CreateFromOBJ("platform_damage_bottom", true);
	modelPlatformItemSpeedReset_ = KamataEngine::Model::CreateFromOBJ("time", true);
	modelItemHpRecovery_ = KamataEngine::Model::CreateFromOBJ("HPheal", true);

	modelBackground_ = KamataEngine::Model::CreateFromOBJ("backblack", true);
	transformBackground_.Initialize();
	transformBackground_.translation_ = {0.0f, 0.0f, 20.0f};
	transformBackground_.UpdateMatarix(); // ★タイポ修正

	// ▼▼▼ 以下を追加 ▼▼▼
	isWarningPlaying_ = false; // 警告音フラグをリセット
	warningVoiceHandle_ = 0;   // 警告音ハンドルをリセット

	// 効果音の読み込み
	sfxHealHandle_ = Audio::GetInstance()->LoadWave("audio/heal.wav");
	sfxClockHandle_ = Audio::GetInstance()->LoadWave("audio/clock.wav");
	// ▼▼▼ 以下を追加 ▼▼▼
	sfxJumpHandle_ = Audio::GetInstance()->LoadWave("audio/jump.wav");
	sfxDamageHandle_ = Audio::GetInstance()->LoadWave("audio/damage.wav");
	sfxWarningHandle_ = Audio::GetInstance()->LoadWave("audio/warning.wav");

	// 背景スプライトの初期化
	skyTextureHandle_ = TextureManager::Load("sky.png");
	skySprite1_ = Sprite::Create(skyTextureHandle_, {0.0f, 0.0f});
	skySprite2_ = Sprite::Create(skyTextureHandle_, {0.0f, 0.0f});


	// アクションフラグを初期化
	hasPlayerMoved_ = false;
	hasPlayerJumped_ = false;
	hasPlayerReversedGravity_ = false;

	// --- 1. ヘッダー画像 ---
	tutorialStageTexHandle_ = TextureManager::Load("tutorialStage/tutorialStage.png");
	tutorialStageSprite_ = Sprite::Create(tutorialStageTexHandle_, {0.0f, 0.0f});
	tutorialStageTexHandle_1 = TextureManager::Load("tutorialStage/tutorialStage1.png");
	tutorialStageSprite_1 = Sprite::Create(tutorialStageTexHandle_1, {0.0f, 0.0f});
	tutorialStageTexHandle_8 = TextureManager::Load("tutorialStage/tutorialStage2.png");
	tutorialStageSprite_8 = Sprite::Create(tutorialStageTexHandle_8, {0.0f, 0.0f});

	// --- 2. 重力反転ガイド ("teck") ---
	tutorialStageTexHandle_2 = TextureManager::Load("tutorialStage/teck1.png");
	tutorialStageSprite_2 = Sprite::Create(tutorialStageTexHandle_2, {0.0f, 0.0f});
	tutorialStageTexHandle_3 = TextureManager::Load("tutorialStage/teck1RED.png");
	tutorialStageSprite_3 = Sprite::Create(tutorialStageTexHandle_3, {0.0f, 0.0f});

	// --- 3. 移動ガイド ("idou") ---
	tutorialStageTexHandle_4 = TextureManager::Load("tutorialStage/idou2.png");
	tutorialStageSprite_4 = Sprite::Create(tutorialStageTexHandle_4, {0.0f, 0.0f});
	tutorialStageTexHandle_5 = TextureManager::Load("tutorialStage/idou2RED.png");
	tutorialStageSprite_5 = Sprite::Create(tutorialStageTexHandle_5, {0.0f, 0.0f});

	// --- 4. ジャンプガイド ("jump") ---
	tutorialStageTexHandle_6 = TextureManager::Load("tutorialStage/jump3.png");
	tutorialStageSprite_6 = Sprite::Create(tutorialStageTexHandle_6, {0.0f, 0.0f});
	tutorialStageTexHandle_7 = TextureManager::Load("tutorialStage/jump3RED.png");
	tutorialStageSprite_7 = Sprite::Create(tutorialStageTexHandle_7, {0.0f, 0.0f});

	// ▲▲▲ 修正ここまで ▲▲▲


	float windowWidth = 640;
	float windowHeight = 720;

	skySprite1_->SetSize({windowWidth, windowHeight});
	skySprite2_->SetSize({windowWidth, windowHeight});
	skySprite1_->SetAnchorPoint({0.5f, 0.5f});
	skySprite2_->SetAnchorPoint({0.5f, 0.5f});
	skySprite1_->SetPosition({windowWidth, windowHeight});
	skySprite2_->SetPosition({windowWidth, windowHeight - windowHeight});

	// HPのUI初期化
	for (int i = 0; i < playerHP_; i++) {
		WorldTransform* wt = new WorldTransform();
		wt->Initialize();
		wt->translation_ = {-33.5f + i * 5.0f, 18.0f, 0.0f};
		wt->scale_ = {1.5f, 1.5f, 0.5f};
		wt->UpdateMatarix(); // ★タイポ修正
		hpWorldTransforms_.push_back(wt);
	}

	// 左右の壁の初期化
	modelEnd_ = KamataEngine::Model::CreateFromOBJ("end", true);
	endTransformLeft_.Initialize();
	endTransformLeft_.translation_ = Vector3(-19.3f, 0.0f, 0.0f);
	endTransformRight_.Initialize();
	endTransformRight_.translation_ = Vector3(19.3f, 0.0f, 0.0f);
	endTransformLeft_.UpdateMatarix();  // ★タイポ修正
	endTransformRight_.UpdateMatarix(); // ★タイポ修正

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
	Vector3 playerPos = {0, 5, 0};
	player_ = new Player();
	player_->Initialize(modelPlayer_, &camera_, playerPos);

	// --- 初期足場生成 ---
	{
		Platform* firstPlatform = new Platform();
		Vector3 pos = {0.0f, -2.0f, 0.0f};
		Vector3 scale = {1.3f, 1.0f, 1.0f};
		firstPlatform->Initialize(pos, scale, DamageDirection::NONE, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, modelItemHpRecovery_, &camera_);
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
			platform->Initialize(pos, scale, dir, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, modelItemHpRecovery_, &camera_);
			platform->SetDamageColliderYOffset(0.05f);
			platform->SetDamageColliderScaleY(0.7f);
			platform->SetSafeSideScaleY(0.8f);
		} else {
			platform->Initialize(pos, scale, DamageDirection::NONE, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, modelItemHpRecovery_, &camera_);
		}
		platforms_.push_back(platform);
	}

	worldTransform.Initialize();
	font_ = new BIt_Map_Font();
	font_->Initialize();
}

void TutorialScene::Update() {

	// --- 1. 時間経過と全体スピードの更新 ---
	gameTime_ += 1.0f / 60.0f; // 40秒タイマーのために時間は計測し続ける
	// ▼▼▼ 変更点: speedMultiplier_ を固定値にする ▼▼▼
	speedMultiplier_ = 1.5f; // これでスクロール速度が一定になります（値は好みで調整してください）

	if (!isTutorialPhase2_ && gameTime_ >= 30.0f) {
		isTutorialPhase2_ = true;
	}

	float scrollSpeed = 0.0f;


	if (!player_->IsDead()) {

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

			Platform* platform = new Platform();

			// ゲーム時間が30秒未満かそれ以上かで処理を分岐
			if (gameTime_ < 30.0f) {
				// --- 30秒未満（安全な期間） ---
				// トゲのない通常の足場のみを生成する
				Vector3 scale = {1.3f, 1.0f, 1.0f};
				platform->Initialize(pos, scale, DamageDirection::NONE, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, modelItemHpRecovery_, &camera_);

			} else {
				// --- 30秒経過後（危険な期間） ---
				// ▼▼▼ 修正点: 乱数変数の宣言を追加 ▼▼▼
				std::uniform_int_distribution<int> dist100(0, 99);
				if (dist100(randomEngine_) < 40) { // 40%の確率
					// トゲ足場を生成
					Vector3 scale = {1.3f, 1.5f, 1.0f};
					DamageDirection dir = player_->IsInversion() ? DamageDirection::BOTTOM : DamageDirection::TOP;
					platform->Initialize(pos, scale, dir, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, modelItemHpRecovery_, &camera_);
					platform->SetDamageColliderScaleY(1.2f);
					platform->SetSafeSideScaleY(0.8f);

				} else {
					// 通常の足場を生成
					Vector3 scale = {1.3f, 1.0f, 1.0f};
					platform->Initialize(pos, scale, DamageDirection::NONE, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, modelItemHpRecovery_, &camera_);

					// アイテム出現判定を「通常の足場」ブロック内に移動
					std::uniform_int_distribution<int> itemDist(0, 99);
					int itemRoll = itemDist(randomEngine_);

					if (itemRoll < 5) { // 5%の確率でHP回復
						platform->SetItemType(ItemType::HP_RECOVERY, player_->IsInversion());
					} else if (itemRoll < 10) { // さらに5%の確率で時間巻き戻し
						platform->SetItemType(ItemType::SPEED_RESET, player_->IsInversion());
					}
				}
			}

			// ▼▼▼ 修正点: 足場をリストに追加する処理を追加 ▼▼▼
			// この処理が抜けていたため、新しい足場がゲームに登場しませんでした。
			platforms_.push_back(platform);
			lastPlatformX = x;
		} // ▲▲▲ 修正点: if (platformSpawnTimer >= ...) の閉じカッコを正しい位置に修正 ▲▲▲

		// --- 3. 全ての足場をスクロールさせる ---
		float gravity = player_->GetGravity();
		float baseScrollSpeed = (gravity > 0.0f) ? -0.1f : 0.1f;
		scrollSpeed = baseScrollSpeed * speedMultiplier_;
		for (auto platform : platforms_) {
			platform->SetScrollSpeed(scrollSpeed);
			platform->Update();
		}
	}

	// --- 4. プレイヤーの入力処理 ---
	player_->Update();

	if (!player_->IsDead()) {

		// ▼▼▼ ジャンプ音の再生処理を追加 ▼▼▼
		if (player_->DidJustJump()) {
			Audio::GetInstance()->PlayWave(sfxJumpHandle_);
		}

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
			} else {
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

					if (platform->GetDamageDirection() == DamageDirection::TOP) {
						if (!player_->IsInvincible()) {
							if (playerHP_ > 0) {
								playerHP_--;
								player_->OnDamage();
								Audio::GetInstance()->PlayWave(sfxDamageHandle_);
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

					if (platform->GetDamageDirection() == DamageDirection::TOP && !player_->IsInversion()) {
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
					} else if (platform->GetDamageDirection() == DamageDirection::BOTTOM && player_->IsInversion()) {
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

				player_->SetPosition(playerPos);
			}

			if (platform->GetItemType() != ItemType::NONE) {
				const AABB& playerAABB = player_->GetAABB();
				const AABB& itemAABB = platform->GetItemAABB();

				if (playerAABB.IsColliding(itemAABB)) {
					switch (platform->GetItemType()) {
					case ItemType::SPEED_RESET:
						Audio::GetInstance()->PlayWave(sfxClockHandle_);
						gameTime_ -= 12.0f;
						if (gameTime_ < 5.0f) {
							gameTime_ = 5.0f;
						}
						platform->SetItemType(ItemType::NONE, player_->IsInversion());
						break;
					case ItemType::HP_RECOVERY:
						if (playerHP_ < 3) {
							Audio::GetInstance()->PlayWave(sfxHealHandle_);
							if (playerHP_ < (int)hpWorldTransforms_.size()) {
								hpWorldTransforms_[playerHP_]->scale_ = {1.5f, 1.5f, 0.5f};
								hpWorldTransforms_[playerHP_]->UpdateMatarix();
							}
							playerHP_++;
						}
						platform->SetItemType(ItemType::NONE, player_->IsInversion());
						break;
					default:
						break;
					}
				}
			}
		}


		// --- 7. 画面端での重力反転処理 ---
		playerPos = player_->GetPosition();
		if (!player_->IsInversion()) {
			if (playerPos.y <= -16.7f) {
				playerPos.y = -16.7f;
				player_->SetPosition(playerPos);
				player_->TriggerGravityReversal();
				hasPlayerReversedGravity_ = true; // ▼▼▼ この行を追加 ▼▼▼
			}
		} else {
			if (playerPos.y >= 16.7f) {
				playerPos.y = 16.7f;
				player_->SetPosition(playerPos);
				player_->TriggerGravityReversal();
				hasPlayerReversedGravity_ = true; // ▼▼▼ この行を追加 ▼▼▼
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

		// ▼▼▼ HP警告音の管理 ▼▼▼
		if (playerHP_ == 1 && !isWarningPlaying_) {
			warningVoiceHandle_ = Audio::GetInstance()->PlayWave(sfxWarningHandle_, true);
			isWarningPlaying_ = true;
		} else if (isWarningPlaying_ && playerHP_ != 1) {
			Audio::GetInstance()->StopWave(warningVoiceHandle_);
			isWarningPlaying_ = false;
			warningVoiceHandle_ = 0;
		}

		if (!hasPlayerMoved_) {
			// キーボードのA,Dキーまたはコントローラーの左スティックかD-Pad左右が入力されたら
			XINPUT_STATE xInputState;
			if ((input_->PushKey(DIK_A) || input_->PushKey(DIK_D)) ||
			    (input_->GetJoystickState(0, xInputState) && (abs(xInputState.Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || (xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ||
			                                                  (xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)))) {
				hasPlayerMoved_ = true;
			}
		}
		if (!hasPlayerJumped_ && player_->DidJustJump()) {
			hasPlayerJumped_ = true;
		}

	} // ★★★ ここが if (!player_->IsDead()) の本当の終わりです ★★★

	// ★★★ player_->UpdateWorldMatrix(); をここへ移動します ★★★
	// これにより、死亡中も行列が更新され、スケールや回転が正しく描画に反映されます。
	player_->UpdateWorldMatrix();

	// --- 12. ゲームオーバー判定 ---
	if (playerHP_ <= 0 && !player_->IsDead()) {
		player_->OnDeath();
	}

	if (player_->IsDead()) {
		gameOverTimer_ += 1.0f / 60.0f;
	}

	if (gameOverTimer_ >= kGameOverDelay_) {
		isGameOver_ = true;
	}

	//if (!player_->IsDead()) {
	//	Vector3 playerPos = player_->GetPosition();
	//	if (playerPos.y > 23.0f || playerPos.y < -23.0f) {
	//		isGameOver_ = true;
	//	}
	//}

}

void TutorialScene::Draw() {
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

	// 「チュートリアルステージ」のタイトルは常に表示
	if (tutorialStageSprite_) {
		tutorialStageSprite_->Draw();
	}

	// ゲームの経過時間に応じて表示を切り替える
	if (!isTutorialPhase2_) {
		// --- 前半 (30秒未満) ---

		// 説明文1を表示
		if (tutorialStageSprite_1) {
			tutorialStageSprite_1->Draw();
		}

		// 各操作ガイドを描画（アクションに応じて赤文字に切り替え）
		// 重力反転ガイド
		if (hasPlayerReversedGravity_) {
			if (tutorialStageSprite_3)
				tutorialStageSprite_3->Draw(); // RED
		} else {
			if (tutorialStageSprite_2)
				tutorialStageSprite_2->Draw(); // 通常
		}
		// 移動ガイド
		if (hasPlayerMoved_) {
			if (tutorialStageSprite_5)
				tutorialStageSprite_5->Draw(); // RED
		} else {
			if (tutorialStageSprite_4)
				tutorialStageSprite_4->Draw(); // 通常
		}
		// ジャンプガイド
		if (hasPlayerJumped_) {
			if (tutorialStageSprite_7)
				tutorialStageSprite_7->Draw(); // RED
		} else {
			if (tutorialStageSprite_6)
				tutorialStageSprite_6->Draw(); // 通常
		}

	} else {
		// --- 後半 (30秒経過後) ---

		// 説明文2を表示
		if (tutorialStageSprite_8) {
			tutorialStageSprite_8->Draw();
		}
		// 操作ガイドは描画しない
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