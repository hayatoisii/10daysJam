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

	isDead_ = false;
	deathRotation_ = 0.0f;
	deathAnimationTimer_ = 0.0f; // ▼▼▼ タイマーをリセット ▼▼▼

	// 初期AABB
	aabb_.Set(pos - halfSize_, pos + halfSize_);
}

void Player::UpdateWorldMatrix() {
	worldTransform_.UpdateMatarix();
	aabb_.Set(worldTransform_.translation_ - halfSize_, worldTransform_.translation_ + halfSize_);
}

void Player::Update() {
	// この関数は、キー入力やコントローラー入力を速度に変換する「だけ」の役割になります。
	// 物理計算（重力など）や、自身の状態（onGround_）の変更は行いません。
	// ▼▼▼ Update関数の先頭に、死亡状態なら専用処理を呼んで終了するロジックを追加 ▼▼▼
	if (isDead_) {
		UpdateDeathAnimation();
		return; // 通常の更新処理は行わない
	}

	// 前フレームの位置を保存
	prevPosition_ = worldTransform_.translation_;
	justJumped_ = false;

	Input* input = KamataEngine::Input::GetInstance();
	XINPUT_STATE xInputState;
	bool isControllerConnected = input->GetJoystickState(0, xInputState);

	// --- 1. 左右移動の入力受付 ---
	velocityX_ = 0.0f;
	float stickX = 0.0f;
	if (isControllerConnected) {
		stickX = static_cast<float>(xInputState.Gamepad.sThumbLX) / SHRT_MAX;
		if (fabsf(stickX) < 0.3f) {
			stickX = 0.0f;
		}
	}
	if (input->PushKey(DIK_A) || (isControllerConnected && (stickX < 0.0f || (xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)))) {
		velocityX_ = -speed;
	}
	if (input->PushKey(DIK_D) || (isControllerConnected && (stickX > 0.0f || (xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)))) {
		velocityX_ = speed;
	}

	// --- 2. ジャンプの入力受付 ---
	XINPUT_STATE xInputStatePrev;
	bool isControllerPrevConnected = input->GetJoystickStatePrevious(0, xInputStatePrev);
	bool isAButtonTriggered = false;
	if (isControllerConnected && isControllerPrevConnected) {
		if ((xInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(xInputStatePrev.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			isAButtonTriggered = true;
		}
	}

	// ★★★ ここからが重要な変更点です ★★★
	// ジャンプボタンが押された瞬間か？
	if (input->TriggerKey(DIK_SPACE) || isAButtonTriggered) {
		// もし地面にいるなら、1段目のジャンプとして上昇速度を与える
		if (onGround_) {
			velocityY_ = inversion ? -jumpPower : jumpPower;
			isJumping_ = true;
			jumpCount_ = 1;
			justJumped_ = true; // ★ジャンプしたフラグを立てる
		}
		// もし空中で、まだジャンプできる回数が残っているなら、2段目のジャンプ
		else if (jumpCount_ < maxJumpCount_) {
			velocityY_ = inversion ? -jumpPower : jumpPower;
			isJumping_ = true;
			jumpCount_++;
			justJumped_ = true; // ★ジャンプしたフラグを立てる
		}
	}
	// ★★★ 変更点はここまで ★★★

	// --- 3. 無敵時間の処理 ---
	if (isInvincible_) {
		invincibilityTimer_ -= 1.0f / 60.0f;
		if (invincibilityTimer_ <= 0.0f) {
			isInvincible_ = false;
		}
	}
}

// SetOnGround関数も、よりシンプルにします
void Player::SetOnGround(bool flag) {
	onGround_ = flag;
	if (flag) {
		// 地面に着いたらY速度を0にし、ジャンプ回数をリセット
		velocityY_ = 0.0f;
		jumpCount_ = 0;
		isJumping_ = false;
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

void Player::TriggerGravityReversal() {
	inversion = !inversion;                     // 重力反転フラグを切り替える
	velocityY_ = 0.0f;                          // 速度をリセット（これでジャンプが軽快になります）
	SetOnGround(true);                          // 境界線を「地面」とみなし、一度着地させる
	gravity = 0.0f;                             // 現在の重力をリセット
	targetGravity = inversion ? 0.05f : -0.05f; // 新しい目標重力を設定
}

void Player::InterpolateGravity() {
	// 目標の重力値(targetGravity)に向かって、現在の重力値(gravity)を少しずつ近づける
	gravity += (targetGravity - gravity) * gravityLerpSpeed;
}

// IsDamageをGetDamageDirectionに変更
DamageDirection Platform::GetDamageDirection() const { return damageDirection_; }


void Player::OnDeath() {
	if (isDead_) {
		return;
	}
	isDead_ = true;
	deathAnimationTimer_ = 0.0f;

	// ▼▼▼ この行を追加 ▼▼▼
	isInvincible_ = false; // 無敵状態を強制的に解除して、点滅を止める

	// 死亡時に少しだけ上に跳ねる演出
	velocityY_ = inversion ? -0.3f : 0.3f;
	velocityX_ = 0.0f;
}

// 死亡モーション中の更新処理
void Player::UpdateDeathAnimation() {
	// 1. アニメーションの進行度を計算
	deathAnimationTimer_ += 1.0f / 60.0f; // 経過時間を加算
	float progress = deathAnimationTimer_ / kDeathAnimationDuration_;

	// ▼▼▼ ここの数値を 4.0f から 1.0f に修正 ▼▼▼
	// これにより、アニメーション完了時に進行度がちょうど 1.0 になります
	if (progress > 1.0f) {
		progress = 1.0f; // 進行度が1を超えないようにする
	}

	// 2. スケーリング（だんだん小さくする処理）
	// イージング関数(progress * progress)を使って、滑らかに縮むように見せる
	float easedProgress = progress * progress;
	float currentScale = 1.0f - easedProgress; // 1.0から0.0へスケールが変化
	worldTransform_.scale_ = {currentScale, currentScale, currentScale};

	// 3. 回転
	const float kRotationSpeed = 0.3f; // 回転速度
	deathRotation_ += kRotationSpeed;
	worldTransform_.rotation_.z = deathRotation_;

	// 4. 落下
	velocityY_ += GetGravity();
	worldTransform_.translation_.y += velocityY_;
}