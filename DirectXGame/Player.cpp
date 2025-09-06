#include "Player.h"
#include <algorithm>
#include <cassert>

Player::~Player() {}

void Player::Initialize(Model* model, Camera* camera, const Vector3& pos) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = pos;
	input_ = KamataEngine::Input::GetInstance();
	worldTransform_.Initialize();
	aabb_.Set(pos - halfSize_, pos + halfSize_);
}

void Player::Update() {
	if (isInvincible_) {
		invincibleTimer_ -= 1.0f / 60.0f;
		if (invincibleTimer_ <= 0.0f) {
			isInvincible_ = false;
		}
	}

	velocityX_ = 0.0f;

	if (input_->PushKey(DIK_A)) {
		velocityX_ = -speed;
	}
	if (input_->PushKey(DIK_D)) {
		velocityX_ = speed;
	}
	worldTransform_.translation_.x += velocityX_;
	worldTransform_.translation_.x = (std::max)(minPlatformX, (std::min)(maxPlatformX, worldTransform_.translation_.x));

	if (input_->TriggerKey(DIK_SPACE)) {
		if (jumpCount_ < maxJumpCount_) {
			velocityY_ = inversion ? -jumpPower : jumpPower;
			isJumping_ = true;
			jumpCount_++;
		}
	}

	gravity += (targetGravity - gravity) * gravityLerpSpeed;
	velocityY_ += gravity;

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

	if (!inversion) {
		if (worldTransform_.translation_.y <= -16.7f) {
			worldTransform_.translation_.y = -16.7f;
			velocityY_ = 0.0f;
			SetOnGround(true);
			gravity = 0.0f;
			inversion = true;
			targetGravity = 0.07f;
			platformScrollSpeed = -fabs(platformScrollSpeed);
		}
	} else {
		if (worldTransform_.translation_.y >= 16.7f) {
			worldTransform_.translation_.y = 16.7f;
			velocityY_ = 0.0f;
			SetOnGround(true);
			gravity = 0.0f;
			inversion = false;
			targetGravity = -0.07f;
			platformScrollSpeed = fabs(platformScrollSpeed);
		}
	}

	Vector3 pos = worldTransform_.translation_;
	aabb_.Set(pos - halfSize_, pos + halfSize_);
	worldTransform_.UpdateMatarix();
}

void Player::OnDamage() {
	if (!isInvincible_) {
		isInvincible_ = true;
		invincibleTimer_ = kInvincibleTime;
	}
}

void Player::SetOnGround(bool flag) {
	if (flag) {
		isJumping_ = false;
		jumpCount_ = 0;
	} else {
		isJumping_ = true;
	}
}

void Player::Draw() {
	if (isInvincible_) {
		if (fmodf(invincibleTimer_, 0.2f) < 0.1f) {
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