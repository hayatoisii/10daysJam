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

	void SetDamageDirection(DamageDirection direction);
	DamageDirection GetDamageDirection() const;

	void SetDamageColliderYOffset(float offset) { damageColliderYOffset_ = offset; }
	void SetDamageColliderScaleY(float scale) { damageColliderScaleY_ = scale; }
	void SetSafeSideScaleY(float scale) { safeSideScaleY_ = scale; }

private:
	float platformScrollSpeed = 0.2f;

	bool isDamage_ = false;

	DamageDirection damageDirection_ = DamageDirection::NONE;

	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;

	Vector3 baseSize_ = {5.0f, 2.5f, 0.15f};
	Vector3 scale_ = {1.0f, 1.0f, 1.0f};
	AABB aabb_;

	Model* normalModel_ = nullptr;
	Model* damageTopModel_ = nullptr;
	Model* damageBottomModel_ = nullptr;

	float damageColliderYOffset_ = 0.1f;
	float damageColliderScaleY_ = 1.0f;
	float safeSideScaleY_ = 1.0f;
};