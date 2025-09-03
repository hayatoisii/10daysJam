#include "Player.h"
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
	velocityX_ = 0.0f;

	movedByInput_ = false;
	// 左右移動
	if (input_->PushKey(DIK_A)) {
		velocityX_ = -speed;

		movedByInput_ = true;
	}
	if (input_->PushKey(DIK_D)) {
		velocityX_ = speed;

		movedByInput_ = true;
	}

	worldTransform_.translation_.x += velocityX_;

	// ジャンプ
	if (input_->TriggerKey(DIK_SPACE)) {
		if (jumpCount_ < maxJumpCount_) {
			velocityY_ = inversion ? -jumpPower : jumpPower;
			isJumping_ = true;
			jumpCount_++;
			movedByInput_ = true; // ← ここを追加
		}
	}

	// --- 重力値を補間 ---
	gravity += (targetGravity - gravity) * gravityLerpSpeed;

	// 重力
	velocityY_ += gravity;

	// 落下速度の上限を適用
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

	// ワールド上下端で反転
	if (!inversion) {
		if (worldTransform_.translation_.y <= -20.0f) {
			worldTransform_.translation_.y = -20.0f;
			velocityY_ = 0.0f;
			SetOnGround(true);
			inversion = true;
			// gravity = -gravity; ← ここは削除
			targetGravity = 0.07f; // 目標重力を上向きに
			platformScrollSpeed = -fabs(platformScrollSpeed);
		}
	} else {
		if (worldTransform_.translation_.y >= 18.0f) {
			worldTransform_.translation_.y = 18.0f;
			velocityY_ = 0.0f;
			SetOnGround(true);
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
	// ★修正: onGround_ メンバー変数を正しく更新
	onGround_ = flag;
	if (flag) {
		isJumping_ = false;
		jumpCount_ = 0;
	} else {
		isJumping_ = true;
	}
}

void Player::Draw() {
	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Player::SetPosition(const Vector3& pos) {
	worldTransform_.translation_ = pos;
	aabb_.Set(pos - halfSize_, pos + halfSize_);
}

float Player::GetGravity() const { return gravity; }