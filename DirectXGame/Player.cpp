#include "Player.h"
#include <cassert>

Player::~Player() {}

void Player::Initialize(Model* model, Camera* camera, const Vector3& pos) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = pos;
	// modelDamage_ の読み込みを削除
	input_ = KamataEngine::Input::GetInstance();
	// damageTransform_.Initialize(); を削除
	worldTransform_.Initialize();

	// 初期AABB
	aabb_.Set(pos - halfSize_, pos + halfSize_);
}

void Player::Update() {
	prevPosition_ = worldTransform_.translation_; // ▼▼▼ この行をUpdate関数の最初に追加 ▼▼▼

	// ▼▼▼ 関数のはじめに、Inputインスタンスとコントローラーの状態変数を準備 ▼▼▼
	Input* input = KamataEngine::Input::GetInstance();
	XINPUT_STATE xInputState;
	bool isControllerConnected = input->GetJoystickState(0, xInputState);

	velocityX_ = 0.0f;

	// ▼▼▼ 左右移動の判定を修正 ▼▼▼
	// 左スティックのデッドゾーンを考慮した入力値
	float stickX = 0.0f;
	if (isControllerConnected) {
		stickX = static_cast<float>(xInputState.Gamepad.sThumbLX) / SHRT_MAX; // -1.0f～1.0fに正規化
		if (abs(stickX) < 0.3f) {                                             // デッドゾーン（入力の遊び）
			stickX = 0.0f;
		}
	}

	// 左右移動 (キーボード または コントローラー)
	if (input->PushKey(DIK_A) || (isControllerConnected && (stickX < 0.0f || (xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)))) {
		velocityX_ = -speed;
	}
	if (input->PushKey(DIK_D) || (isControllerConnected && (stickX > 0.0f || (xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)))) {
		velocityX_ = speed;
	}
	// ▲▲▲ ここまで修正 ▲▲▲

	worldTransform_.translation_.x += velocityX_;

	// 無敵時間中の処理
	if (isInvincible_) {
		// タイマーをフレーム時間分だけ減らす (60FPS想定)
		invincibilityTimer_ -= 1.0f / 60.0f;
		// タイマーが0以下になったら無敵状態を解除
		if (invincibilityTimer_ <= 0.0f) {
			isInvincible_ = false;
		}
	}

	//velocityX_ = 0.0f;

	//// 左右移動
	//if (input_->PushKey(DIK_A)) {
	//	velocityX_ = -speed;
	//}
	//if (input_->PushKey(DIK_D)) {
	//	velocityX_ = speed;
	//}
	//worldTransform_.translation_.x += velocityX_;

	// X座標の上限・下限を適用
	if (worldTransform_.translation_.x < minPlatformX) {
		worldTransform_.translation_.x = minPlatformX;
	}
	if (worldTransform_.translation_.x > maxPlatformX) {
		worldTransform_.translation_.x = maxPlatformX;
	}

	// ▼▼▼ ジャンプの判定を修正 ▼▼▼
	XINPUT_STATE xInputStatePrev;
	bool isControllerPrevConnected = input->GetJoystickStatePrevious(0, xInputStatePrev);

	// Aボタンが押された瞬間かどうかの判定
	bool isAButtonTriggered = false;
	if (isControllerConnected && isControllerPrevConnected) {
		if ((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			isAButtonTriggered = true;
		}
	}

	// ジャンプ (キーボード または コントローラー)
	if (input->TriggerKey(DIK_SPACE) || isAButtonTriggered) {
		if (jumpCount_ < maxJumpCount_) {

			// ▼▼▼ ここからが追加・修正部分 ▼▼▼
			// もし空中で、かつジャンプ回数が0回（つまり落下してきただけ）なら、
			// このジャンプを2段目として扱うために、回数を強制的に1にする。
			if (!isOnGround_ && jumpCount_ == 0) {
				jumpCount_ = 1;
			}
			// ▲▲▲ -------------------------- ▲▲▲

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
			targetGravity = 0.05f; // 目標重力を上向きに
			platformScrollSpeed = -fabs(platformScrollSpeed);
		}
	} else {
		if (worldTransform_.translation_.y >= 16.7f) { // 境界線の少し手前で反転
			worldTransform_.translation_.y = 16.7f; // 境界線の少し手前に配置
			velocityY_ = 0.0f; // 完全に停止
			SetOnGround(true);
			gravity = 0.0f;
			inversion = false;
			targetGravity = -0.05f; // 目標重力を下向きに
			platformScrollSpeed = fabs(platformScrollSpeed);
		}
	}

	// AABB 更新
	Vector3 pos = worldTransform_.translation_;
	aabb_.Set(pos - halfSize_, pos + halfSize_);

	worldTransform_.UpdateMatarix();
}

void Player::SetOnGround(bool flag) {

	isOnGround_ = flag;

	// もし地面にいる状態になったのなら、ジャンプ回数をリセット
	if (flag) {
		jumpCount_ = 0;
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

// ダメージ処理。GameSceneから呼ばれる
void Player::OnDamage() {
	// すでに無敵なら何もしない
	if (isInvincible_) {
		return;
	}

	// 無敵状態にしてタイマーをセット
	isInvincible_ = true;
	invincibilityTimer_ = 1.5f; // 1.5秒間の無敵時間
}

// 外部から無敵状態か問い合わせるための関数
bool Player::IsInvincible() const { return isInvincible_; }

// SetDamageをSetDamageDirectionに変更
void Platform::SetDamageDirection(DamageDirection direction) { damageDirection_ = direction; }

// IsDamageをGetDamageDirectionに変更
DamageDirection Platform::GetDamageDirection() const { return damageDirection_; }