#include "Player.h"
#include <algorithm> // std::minとstd::maxのために追加
#include <cassert>

Player::~Player() {}

void Player::Initialize(Model* model, Camera* camera, const Vector3& pos) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = pos;
	input_ = KamataEngine::Input::GetInstance();
	worldTransform_.Initialize();

	// 初期AABB
	aabb_.Set(pos - halfSize_, pos + halfSize_);
}

void Player::Update() {

	// 無敵時間中の処理
	if (isInvincible_) {
		// タイマーをフレーム時間分だけ減らす (60FPS想定)
		invincibilityTimer_ -= 1.0f / 60.0f;
		// タイマーが0以下になったら無敵状態を解除
		if (invincibilityTimer_ <= 0.0f) {
			isInvincible_ = false;
		}
	}

	velocityX_ = 0.0f;

	// 左右移動
	if (input_->PushKey(DIK_A)) {
		velocityX_ = -speed;
	}
	if (input_->PushKey(DIK_D)) {
		velocityX_ = speed;
	}
	worldTransform_.translation_.x += velocityX_;

	// X座標の上限・下限を適用
	// 修正: min/maxマクロとの競合を避けるため、( ) で囲む
	worldTransform_.translation_.x = (std::max)(minPlatformX, (std::min)(maxPlatformX, worldTransform_.translation_.x));

	// ジャンプ
	if (input_->TriggerKey(DIK_SPACE)) {
		if (jumpCount_ < maxJumpCount_) {
			velocityY_ = inversion ? -jumpPower : jumpPower;
			isJumping_ = true;
			jumpCount_++;
		}
	}

	// --- 重力値を補間 ---
	gravity += (targetGravity - gravity) * gravityLerpSpeed;

	// 重力
	velocityY_ += gravity;

	// 落下速度の上限を適用
	// 修正: min/maxマクロとの競合を避けるため、( ) で囲む
	if (!inversion) {
		if (velocityY_ > maxFallSpeed) {
			velocityY_ = maxFallSpeed;
		}
	} else {
		if (velocityY_ < -maxFallSpeed) {
			velocityY_ = -maxFallSpeed;
		}
	}

	worldTransform_.translation_.y += velocityY_;

	// ワールド上下端で反転（慣性を大幅に減らして境界線直前で停止）
	if (!inversion) {
		if (worldTransform_.translation_.y <= -16.7f) { // 境界線の少し手前で反転
			worldTransform_.translation_.y = -16.7f; // 境界線の少し手前に配置
			velocityY_ = 0.0f; // 完全に停止
			SetOnGround(true);
			gravity = 0.0f;
			inversion = true;
			// gravity = -gravity; ← ここは削除
			targetGravity = 0.07f; // 目標重力を上向きに
			platformScrollSpeed = -fabs(platformScrollSpeed);
		}
	} else {
		if (worldTransform_.translation_.y >= 16.7f) { // 境界線の少し手前で反転
			worldTransform_.translation_.y = 16.7f; // 境界線の少し手前に配置
			velocityY_ = 0.0f; // 完全に停止
			SetOnGround(true);
			gravity = 0.0f;
			inversion = false;
			// gravity = -gravity; ← ここは削除
			targetGravity = -0.07f; // 目標重力を下向きに
			platformScrollSpeed = fabs(platformScrollSpeed);
		}
	}

	// AABB 更新
	Vector3 pos = worldTransform_.translation_;
	aabb_.Set(pos - halfSize_, pos + halfSize_);

	worldTransform_.UpdateMatarix();
}

void Player::SetOnGround(bool flag) {
	if (flag) {
		isJumping_ = false;
		jumpCount_ = 0; // ★着地時にジャンプ回数リセット
	} else {
		isJumping_ = true;
	}
}

void Player::Draw() {

	// 無敵状態なら点滅させる
	if (isInvincible_) {
		// タイマーの小数部分を利用して点滅を実現
		if (fmodf(invincibilityTimer_, 0.2f) < 0.1f) {
			// 一定周期で描画をスキップすることで点滅しているように見せる
			return;
		}
	}

	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Player::SetPosition(const Vector3& pos) {
	worldTransform_.translation_ = pos;
	aabb_.Set(pos - halfSize_, pos + halfSize_);
}

float Player::GetGravity() const { return gravity; }

//void Player::TakeDamage(const Vector3& playerPos) {
//	// 中身をすべて削除、またはコメントアウトする
//	// (playerPosは未使用になるので警告が出るかもしれませんが問題ありません)
//}