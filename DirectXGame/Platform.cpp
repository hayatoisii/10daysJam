#include "Platform.h"

// プラットフォームの初期化
void Platform::Initialize(const Vector3& pos, const Vector3& scale, Model* normalModel, Model* damageTopModel, Model* damageBottomModel, Camera* camera) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = scale;
	// Keep an internal copy of scale for AABB updates
	scale_ = scale;
	worldTransform_.UpdateMatarix();
	// 初期AABBもUpdateと同じ式で計算し、ダメージ足場ならオフセット
	{
		Vector3 initPos = pos;
		Vector3 baseHalf = (baseSize_ * scale_) * 0.5f;
		Vector3 minV = initPos - baseHalf;
		Vector3 maxV = initPos + baseHalf;
		if (damageDirection_ == DamageDirection::TOP || damageDirection_ == DamageDirection::BOTTOM) {
			// 片側のみ高さを拡張/縮小（危険面の方向に拡張）+ 安全側調整
			float delta = baseHalf.y * (damageColliderScaleY_ - 1.0f);
			if (delta > 0.0f) {
				if (damageDirection_ == DamageDirection::TOP) { maxV.y += delta; } else { minV.y -= delta; }
			} else if (delta < 0.0f) {
				float shrink = -delta;
				if (damageDirection_ == DamageDirection::TOP) { maxV.y -= shrink; } else { minV.y += shrink; }
			}
			// 安全側（反対側）をスケール
			float safeDelta = baseHalf.y * (safeSideScaleY_ - 1.0f);
			if (safeDelta < 0.0f) {
				float safeShrink = -safeDelta;
				if (damageDirection_ == DamageDirection::TOP) { minV.y += safeShrink; } else { maxV.y -= safeShrink; }
			} else if (safeDelta > 0.0f) {
				if (damageDirection_ == DamageDirection::TOP) { minV.y -= safeDelta; } else { maxV.y += safeDelta; }
			}
			// オフセット（TOPは上へ、BOTTOMは下へ）
			if (damageDirection_ == DamageDirection::TOP) {
				minV.y += damageColliderYOffset_;
				maxV.y += damageColliderYOffset_;
			} else {
				minV.y -= damageColliderYOffset_;
				maxV.y -= damageColliderYOffset_;
			}
		}
		aabb_.Set(minV, maxV);
	}
	this->camera_ = camera;

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
	Vector3 baseHalf = (baseSize_ * scale_) * 0.5f;
	Vector3 minV = pos - baseHalf;
	Vector3 maxV = pos + baseHalf;
	if (damageDirection_ == DamageDirection::TOP || damageDirection_ == DamageDirection::BOTTOM) {
		float delta = baseHalf.y * (damageColliderScaleY_ - 1.0f);
		if (delta > 0.0f) {
			if (damageDirection_ == DamageDirection::TOP) { maxV.y += delta; } else { minV.y -= delta; }
		} else if (delta < 0.0f) {
			float shrink = -delta;
			if (damageDirection_ == DamageDirection::TOP) { maxV.y -= shrink; } else { minV.y += shrink; }
		}
		// 安全側（反対側）をスケール
		float safeDelta = baseHalf.y * (safeSideScaleY_ - 1.0f);
		if (safeDelta < 0.0f) {
			float safeShrink = -safeDelta;
			if (damageDirection_ == DamageDirection::TOP) { minV.y += safeShrink; } else { maxV.y -= safeShrink; }
		} else if (safeDelta > 0.0f) {
			if (damageDirection_ == DamageDirection::TOP) { minV.y -= safeDelta; } else { maxV.y += safeDelta; }
		}
		// オフセット（TOPは上へ、BOTTOMは下へ）
		if (damageDirection_ == DamageDirection::TOP) {
			minV.y += damageColliderYOffset_;
			maxV.y += damageColliderYOffset_;
		} else {
			minV.y -= damageColliderYOffset_;
			maxV.y -= damageColliderYOffset_;
		}
	}
	aabb_.Set(minV, maxV);

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