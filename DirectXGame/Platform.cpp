#include "Platform.h"

void Platform::Initialize(const Vector3& pos, const Vector3& scale, Model* normalModel, Model* damageTopModel, Model* damageBottomModel, Camera* camera) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = scale;
	scale_ = scale;
	worldTransform_.UpdateMatarix();

	{
		Vector3 initPos = pos;
		Vector3 baseHalf = (baseSize_ * scale_) * 0.5f;
		Vector3 minV = initPos - baseHalf;
		Vector3 maxV = initPos + baseHalf;
		if (damageDirection_ == DamageDirection::TOP || damageDirection_ == DamageDirection::BOTTOM) {
			float delta = baseHalf.y * (damageColliderScaleY_ - 1.0f);
			if (delta > 0.0f) {
				if (damageDirection_ == DamageDirection::TOP) {
					maxV.y += delta;
				} else {
					minV.y -= delta;
				}
			} else if (delta < 0.0f) {
				float shrink = -delta;
				if (damageDirection_ == DamageDirection::TOP) {
					maxV.y -= shrink;
				} else {
					minV.y += shrink;
				}
			}
			float safeDelta = baseHalf.y * (safeSideScaleY_ - 1.0f);
			if (safeDelta < 0.0f) {
				float safeShrink = -safeDelta;
				if (damageDirection_ == DamageDirection::TOP) {
					minV.y += safeShrink;
				} else {
					maxV.y -= safeShrink;
				}
			} else if (safeDelta > 0.0f) {
				if (damageDirection_ == DamageDirection::TOP) {
					minV.y -= safeDelta;
				} else {
					maxV.y += safeDelta;
				}
			}
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
	this->normalModel_ = normalModel;
	this->damageTopModel_ = damageTopModel;
	this->damageBottomModel_ = damageBottomModel;
}

void Platform::SetScrollSpeed(float speed) { platformScrollSpeed = speed; }

Vector3 Platform::GetWorldPosition() const { return worldTransform_.translation_; }

void Platform::Update() {
	worldTransform_.translation_.y += platformScrollSpeed;

	Vector3 pos = worldTransform_.translation_;
	Vector3 baseHalf = (baseSize_ * scale_) * 0.5f;
	Vector3 minV = pos - baseHalf;
	Vector3 maxV = pos + baseHalf;
	if (damageDirection_ == DamageDirection::TOP || damageDirection_ == DamageDirection::BOTTOM) {
		float delta = baseHalf.y * (damageColliderScaleY_ - 1.0f);
		if (delta > 0.0f) {
			if (damageDirection_ == DamageDirection::TOP) {
				maxV.y += delta;
			} else {
				minV.y -= delta;
			}
		} else if (delta < 0.0f) {
			float shrink = -delta;
			if (damageDirection_ == DamageDirection::TOP) {
				maxV.y -= shrink;
			} else {
				minV.y += shrink;
			}
		}
		float safeDelta = baseHalf.y * (safeSideScaleY_ - 1.0f);
		if (safeDelta < 0.0f) {
			float safeShrink = -safeDelta;
			if (damageDirection_ == DamageDirection::TOP) {
				minV.y += safeShrink;
			} else {
				maxV.y -= safeShrink;
			}
		} else if (safeDelta > 0.0f) {
			if (damageDirection_ == DamageDirection::TOP) {
				minV.y -= safeDelta;
			} else {
				maxV.y += safeDelta;
			}
		}
		if (damageDirection_ == DamageDirection::TOP) {
			minV.y += damageColliderYOffset_;
			maxV.y += damageColliderYOffset_;
		} else {
			minV.y -= damageColliderYOffset_;
			maxV.y -= damageColliderYOffset_;
		}
	}
	aabb_.Set(minV, maxV);
	worldTransform_.UpdateMatarix();
}

void Platform::Draw() {
	switch (damageDirection_) {
	case DamageDirection::TOP:
		if (damageTopModel_) {
			damageTopModel_->Draw(worldTransform_, *camera_);
		}
		break;
	case DamageDirection::BOTTOM:
		if (damageBottomModel_) {
			damageBottomModel_->Draw(worldTransform_, *camera_);
		}
		break;
	case DamageDirection::NONE:
	default:
		if (normalModel_) {
			normalModel_->Draw(worldTransform_, *camera_);
		}
		break;
	}
}

void Platform::SetDamageDirection(DamageDirection direction) { damageDirection_ = direction; }

DamageDirection Platform::GetDamageDirection() const { return damageDirection_; }