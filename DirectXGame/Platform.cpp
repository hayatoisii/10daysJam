#include "Platform.h"

// プラットフォームの初期化
void Platform::Initialize(const Vector3& pos, const Vector3& scale, Model* model, Camera* camera) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = pos; // 位置設定
	worldTransform_.scale_ = scale;     // スケール設定
	model_ = model;
	camera_ = camera;
	scale_ = scale;

	// AABB（当たり判定ボックス）初期化
	// 基準サイズにスケールを乗算し半分のサイズを計算
	Vector3 halfSize = (baseSize_ * scale_) * 0.5f;
	aabb_.Set(pos - halfSize, pos + halfSize);
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

// プラットフォームの描画
void Platform::Draw() {
	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}