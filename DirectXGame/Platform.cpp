#include "Platform.h"

// プラットフォームの初期化
void Platform::Initialize(const Vector3& pos, const Vector3& scale, Model* normalModel, Model* damageTopModel, Model* damageBottomModel, Camera* camera) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = scale;
	// Keep an internal copy of scale for AABB updates
	scale_ = scale;

	// 3つのモデルへのポインタを保持
	this->normalModel_ = normalModel;
	this->damageTopModel_ = damageTopModel;
	this->damageBottomModel_ = damageBottomModel;
}

// スクロール速度の設定
void Platform::SetScrollSpeed(float speed) { platformScrollSpeed = speed; }

// プラットフォームの現在位置を取得
Vector3 Platform::GetWorldPosition() const { return worldTransform_.translation_; }

// プラットフォームの更新処理
void Platform::Update() {
	// Y軸方向にスクロール速度分移動
	worldTransform_.translation_.y += platformScrollSpeed;

	// AABBを現在の位置・スケールに合わせて更新
	Vector3 pos = worldTransform_.translation_;
	Vector3 halfSize = (baseSize_ * scale_) * 0.5f;
	aabb_.Set(pos - halfSize, pos + halfSize);

	// ワールド行列の更新
	worldTransform_.UpdateMatarix();
}

// Platform.cpp

void Platform::Draw() {
	// どのモデルを描画するかを damageDirection_ に基づいて決定する
	switch (damageDirection_) {
	case DamageDirection::TOP:
		// 上面が危険な場合 -> 上向きダメージモデルを描画
		if (damageTopModel_) {
			damageTopModel_->Draw(worldTransform_, *camera_);
		}
		break;

	case DamageDirection::BOTTOM:
		// 下面が危険な場合 -> 下向きダメージモデルを描画
		if (damageBottomModel_) {
			damageBottomModel_->Draw(worldTransform_, *camera_);
		}
		break;

	case DamageDirection::NONE:
	default:
		// 無害な場合 -> 通常モデルを描画
		if (normalModel_) {
			normalModel_->Draw(worldTransform_, *camera_);
		}
		break;
	}
}