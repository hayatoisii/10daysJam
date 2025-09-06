#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "WorldTransformEx.h"

using namespace KamataEngine;

// どの面が危険かを示す列挙型を定義
enum class DamageDirection {
	NONE,   // 無害
	TOP,    // 上面が危険
	BOTTOM, // 下面が危険
};

class Platform {
public:
	void Initialize(const Vector3& pos, const Vector3& scale, Model* normalModel, Model* damageTopModel, Model* damageBottomModel, Camera* camera);
	void Update();
	void Draw();

	const AABB& GetAABB() const { return aabb_; }

	void SetScrollSpeed(float speed);

	Vector3 GetWorldPosition() const;

	bool IsDamage() const { return isDamage_; }
	void SetDamage(bool flag) { isDamage_ = flag; }

	// SetDamageをSetDamageDirectionに変更
	void SetDamageDirection(DamageDirection direction);
	// IsDamageをGetDamageDirectionに変更
	DamageDirection GetDamageDirection() const;

	// ダメージ足場の当たり判定を上方向にずらす量を設定
	void SetDamageColliderYOffset(float offset) { damageColliderYOffset_ = offset; }
	// ダメージ足場の当たり判定の高さスケールを設定（1.0が等倍）
	void SetDamageColliderScaleY(float scale) { damageColliderScaleY_ = scale; }
	// 安全側（ダメージではない側）の高さスケール（1.0が等倍、<1.0で小さく）
	void SetSafeSideScaleY(float scale) { safeSideScaleY_ = scale; }

private:
	float platformScrollSpeed = 0.2f; // 足場の上方向移動速度

	bool isDamage_ = false;

	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;

	Vector3 baseSize_ = {5.0f, 2.5f, 0.15f};
	Vector3 scale_ = {1.0f, 1.0f, 1.0f};
	AABB aabb_;

	Model* normalModel_ = nullptr;
	// ★変更：ダメージモデルを2つに分ける
	Model* damageTopModel_ = nullptr;
	Model* damageBottomModel_ = nullptr;

	DamageDirection damageDirection_ = DamageDirection::NONE;

	// ダメージ足場用AABBのYオフセット（+で上にずらす）
	float damageColliderYOffset_ = 0.1f;
	// ダメージ足場用AABBの高さスケール（1.0が等倍。小さいほど薄くなる）
	float damageColliderScaleY_ = 1.0f;
	// 安全側（反対側）高さスケール（1.0が等倍。小さくして安全側を少し削る）
	float safeSideScaleY_ = 1.0f;
};