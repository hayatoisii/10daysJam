#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "WorldTransformEx.h"

using namespace KamataEngine;

class Platform {
public:
	void Initialize(const Vector3& pos, const Vector3& size, Model* model, Camera* camera);
	void Update();
	void Draw();

	const AABB& GetAABB() const { return aabb_; }

	void SetScrollSpeed(float speed);

	 Vector3 GetWorldPosition() const;

private:

	float platformScrollSpeed = 0.2f; // 足場の上方向移動速度

	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;

	Vector3 baseSize_ = {5.0f, 2.5f, 0.15f};
	Vector3 scale_ = {1.0f, 1.0f, 1.0f};
	AABB aabb_;
};