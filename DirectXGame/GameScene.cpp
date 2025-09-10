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
	gameOverTimer_ = 0.0f; // ゲームオーバータイマーをリセット

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

	modelBlackBg_ = KamataEngine::Model::CreateFromOBJ("BackBlack", true);
	transformBlackBg_.Initialize();
	transformBlackBg_.translation_ = {0.0f, 0.0f, 20.0f};
	transformBlackBg_.UpdateMatarix();

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


	// 背景モデルの読み込みと初期化
	modelBackground_ = KamataEngine::Model::CreateFromOBJ("backrock", true);

	// 1つ目の背景の初期化
	transformBackground1_.Initialize();
	transformBackground1_.translation_ = {0.0f, 0.0f, -37.7f};
	transformBackground1_.UpdateMatarix();

	// 2つ目の背景の初期化
	transformBackground2_.Initialize();
	transformBackground2_.translation_ = {0.0f, backgroundHeight_, -37.7f};
	transformBackground2_.UpdateMatarix();


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
	endTransformLeft_.UpdateMatarix(); // ★タイポ修正
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

// GameScene.cpp

void GameScene::Update() {

	// --- 1. 時間経過と全体スピードの更新 ---
	gameTime_ += 1.0f / 60.0f;

	// 40秒経過したかどうかでスクロール速度の計算方法を切り替える
	if (gameTime_ < 15.0f) {
		// 40秒未満の場合、スクロール速度を一定に保つ
		speedMultiplier_ = 1.5f;
	} else {
		// 40秒経過後、経過時間に応じてだんだん速くする
		// 40秒時点から加速が始まるように、経過時間から40秒を引く
		float elapsedTimeAfter40s = gameTime_ - 15.0f;
		// 基本速度1.5fに、40秒後からの経過時間に応じて速度を加算（この計算式で加速具合を調整できます）
		speedMultiplier_ = 1.5f + (elapsedTimeAfter40s / 15.0f) * 1.5f;
	}

	// 速度に上限を設ける
	if (speedMultiplier_ > 200.0f) {
		speedMultiplier_ = 200.0f;
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
			Vector3 scale = {1.3f, 1.0f, 1.0f};
			Platform* platform = new Platform();
			std::uniform_int_distribution<int> dist10(0, 9);
			if (dist10(randomEngine_) < 6) {
				scale = {1.3f, 1.5f, 1.0f};
				DamageDirection dir = player_->IsInversion() ? DamageDirection::BOTTOM : DamageDirection::TOP;
				platform->Initialize(pos, scale, dir, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, modelItemHpRecovery_, &camera_);
				platform->SetDamageColliderScaleY(1.2f);
				platform->SetSafeSideScaleY(0.8f);
			} else {
				platform->Initialize(pos, scale, DamageDirection::NONE, modelPlatform_, modelDamageTop_, modelDamageBottom_, modelPlatformItemSpeedReset_, modelItemHpRecovery_, &camera_);

				std::uniform_int_distribution<int> itemDist(0, 99);
				int itemRoll = itemDist(randomEngine_);

				if (itemRoll < 5) {
					platform->SetItemType(ItemType::HP_RECOVERY, player_->IsInversion());
				} else if (itemRoll < 10) {
					platform->SetItemType(ItemType::SPEED_RESET, player_->IsInversion());
				}
			}
			platforms_.push_back(platform);
			lastPlatformX = x;
		}

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
			}
		} else {
			if (playerPos.y >= 16.7f) {
				playerPos.y = 16.7f;
				player_->SetPosition(playerPos);
				player_->TriggerGravityReversal();
			}
		}

		// --- 8. 背景の更新 ---
		if (modelBackground_) {
			// 2つの背景をスクロール速度に合わせて動かす
			transformBackground1_.translation_.y += scrollSpeed / 4;
			transformBackground2_.translation_.y += scrollSpeed / 4;

			// 背景2枚分の合計の高さ
			const float totalBackgroundHeight = backgroundHeight_ * 2;

			// 上にスクロールしていく場合（プレイヤーは下降中）のループ処理
			if (scrollSpeed > 0) {
				if (transformBackground1_.translation_.y >= backgroundHeight_ - 5.0f) {
					transformBackground1_.translation_.y -= totalBackgroundHeight;
				}
				if (transformBackground2_.translation_.y >= backgroundHeight_ - 5.0f) {
					transformBackground2_.translation_.y -= totalBackgroundHeight;
				}
			}
			// 下にスクロールしていく場合（プレイヤーは上昇中）のループ処理
			else {
				if (transformBackground1_.translation_.y <= -backgroundHeight_ - 5.0f) {
					transformBackground1_.translation_.y += totalBackgroundHeight;
				}
				if (transformBackground2_.translation_.y <= -backgroundHeight_ - 5.0f) {
					transformBackground2_.translation_.y += totalBackgroundHeight;
				}
			}

			// 座標を更新したので行列も更新する
			transformBackground1_.UpdateMatarix();
			transformBackground2_.UpdateMatarix();
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
	//	if (playerPos.y > 25.0f || playerPos.y < -25.0f) {
	//		isGameOver_ = true;
	//	}
	//}
}

void GameScene::Draw() {
	// (この関数の中身は変更ありません)
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	if (modelBlackBg_) {
		modelBlackBg_->Draw(transformBlackBg_, camera_);
	}

	if (modelBackground_) {
		modelBackground_->Draw(transformBackground1_, camera_);
		modelBackground_->Draw(transformBackground2_, camera_);
	}

	Model::PostDraw();

	dxCommon->ClearDepthBuffer();

	// 2D描画前準備
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 2D描画（必要ならここに描画処理を書く）
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