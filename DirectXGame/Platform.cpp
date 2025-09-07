#include "Platform.h"

// プラットフォームの初期化
void Platform::Initialize(const Vector3& pos, const Vector3& scale, DamageDirection direction, Model* normalModel, Model* damageTopModel, Model* damageBottomModel, Model* itemSpeedResetModel, Camera* camera) {
	worldTransform_.Initialize();
	damageDirection_ = direction;
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = scale;
	// Keep an internal copy of scale for AABB updates
	scale_ = scale;

	// ★ アイテム用のWorldTransformを初期化
	itemWorldTransform_.Initialize();
	// ★ 親子関係を設定。これで足場が動くとアイテムも一緒に動く。
	itemWorldTransform_.parent_ = &worldTransform_;
	// ★ 足場の中心から、Y軸方向に少しずらした位置をアイテムの座標とする
	itemWorldTransform_.translation_ = {0.0f, 2.0f, 0.0f}; // このY値で高さを調整

	worldTransform_.UpdateMatarix();
	// ★ 子のこちらも更新しておく
	itemWorldTransform_.UpdateMatarix();

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
			// 安全側（反対側）をスケール
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

	// モデルへのポインタを保持
	this->normalModel_ = normalModel;
	this->damageTopModel_ = damageTopModel;
	this->damageBottomModel_ = damageBottomModel;
	this->itemSpeedResetModel_ = itemSpeedResetModel;
}

// スクロール速度の設定
void Platform::SetScrollSpeed(float speed) { platformScrollSpeed = speed; }

// プラットフォームの現在位置を取得
Vector3 Platform::GetWorldPosition() const { return worldTransform_.translation_; }

// プラットフォームの更新処理
void Platform::Update(bool isPlayerInverted) { // ★ 変更：引数を追加
	// Y軸方向にスクロール速度分移動
	worldTransform_.translation_.y += platformScrollSpeed;

	// プレイヤーの重力状態に応じてアイテムのY座標を切り替える
	if (isPlayerInverted) {
		// 重力反転時：足場の下側にアイテムを配置
		itemWorldTransform_.translation_.y = -2.0f;

	} else {
		// 通常時：足場の上側にアイテムを配置
		itemWorldTransform_.translation_.y = 2.0f;
	}

	// AABBを現在の位置・スケールに合わせて更新
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
		// 安全側（反対側）をスケール
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
	// ★★★ 追加：子のアイテムTransformも更新する ★★★
	itemWorldTransform_.UpdateMatarix();
}

// 描画処理
void Platform::Draw() {
	// --- 1. 足場本体を描画 ---
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

	// --- 2. もしアイテムを持っているなら、追加でアイテムを描画 ---
	if (itemType_ == ItemType::SPEED_RESET && itemSpeedResetModel_) {
		// アイテム専用のTransformを使って描画する
		itemSpeedResetModel_->Draw(itemWorldTransform_, *camera_);
	}
}