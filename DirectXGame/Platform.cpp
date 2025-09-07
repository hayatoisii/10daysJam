#include "Platform.h"

// プラットフォームの初期化
void Platform::Initialize(
    const Vector3& pos, const Vector3& scale, DamageDirection direction, Model* normalModel, Model* damageTopModel, Model* damageBottomModel, Model* itemSpeedResetModel, Model* itemHpRecoveryModel,
    Camera* camera) {
	worldTransform_.Initialize();
	damageDirection_ = direction;
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = scale;
	scale_ = scale;

	itemWorldTransform_.Initialize();
	itemWorldTransform_.parent_ = &worldTransform_;
	itemWorldTransform_.translation_ = {0.0f, 2.0f, 0.0f};

	worldTransform_.UpdateMatarix();    // ★タイポ修正
	itemWorldTransform_.UpdateMatarix(); // ★タイポ修正

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
	this->itemSpeedResetModel_ = itemSpeedResetModel;
	this->itemHpRecoveryModel_ = itemHpRecoveryModel;
}

// アイテムの種類と、それが足場の下側にあるべきかを設定する
void Platform::SetItemType(ItemType type, bool isPlayerInverted) {
	this->itemType_ = type;
	this->itemIsOnBottom_ = isPlayerInverted;

	if (this->itemIsOnBottom_) {
		itemWorldTransform_.translation_.y = -2.0f;
	} else {
		itemWorldTransform_.translation_.y = 2.0f;
	}
}

// スクロール速度の設定
void Platform::SetScrollSpeed(float speed) { platformScrollSpeed = speed; }

// プラットフォームの現在位置を取得
Vector3 Platform::GetWorldPosition() const { return worldTransform_.translation_; }

// プラットフォームの更新処理
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

	worldTransform_.UpdateMatarix();     // ★タイポ修正
	itemWorldTransform_.UpdateMatarix(); // ★タイポ修正

	if (itemType_ != ItemType::NONE) {
		Vector3 itemPos = itemWorldTransform_.GetWorldPosition();
		Vector3 itemHalfSize = {1.5f, 1.5f, 1.5f};
		itemAABB_.Set(itemPos - itemHalfSize, itemPos + itemHalfSize);
	}
}

// 描画処理
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

	switch (itemType_) {
	case ItemType::SPEED_RESET:
		itemSpeedResetModel_->Draw(itemWorldTransform_, *camera_);
		break;
	case ItemType::HP_RECOVERY:
		itemHpRecoveryModel_->Draw(itemWorldTransform_, *camera_);
		break;
	case ItemType::NONE:
	default:
		break;
	}
}