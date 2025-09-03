#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "Platform.h"
#include "WorldTransformEx.h"

using namespace KamataEngine;

class Player {
public:
	~Player();

	void Initialize(Model* model, Camera* camera, const Vector3& pos);
	void Update();
	void Draw();

	// ==== Getter / Setter ====
	float GetGravity() const;

	Vector3 GetPosition() const { return worldTransform_.translation_; }
	void SetPosition(const Vector3& pos);

	float GetVelocityX() const { return velocityX_; }
	float GetVelocityY() const { return velocityY_; }
	void SetVelocityX(float vx) { velocityX_ = vx; }
	void SetVelocityY(float vy) { velocityY_ = vy; }

	const AABB& GetAABB() const { return aabb_; }
	float GetHalfSizeY() const { return halfSize_.y; }

	bool IsInversion() const { return inversion; }

	void SetOnGround(bool flag);

private:
	// 足場のスクロール速度
	float platformScrollSpeed = 0.2f;

	// 横移動速度
	const float speed = 0.6f;

	// 落下速度の上限
	const float maxFallSpeed = 1.0f;

	// 重力
	float gravity = -0.07f;

	// ジャンプ初速度
	const float jumpPower = 1.3f;


	bool isOnGround_ = false;

	// 速度
	float velocityY_ = 0.0f;
	float velocityX_ = 0.0f;

	float targetGravity = -0.06f; // 目標重力
	float gravityLerpSpeed = 0.07f;

	// ジャンプ回数管理
	int jumpCount_ = 0;
	int maxJumpCount_ = 1; // ← ここを2にすれば二段ジャンプ

	bool isJumping_ = false;
	bool inversion = false;

	Model* model_ = nullptr;
	Input* input_ = nullptr;
	Camera* camera_ = nullptr;

	WorldTransform worldTransform_;
	AABB aabb_;
	Vector3 halfSize_{0.5f, 0.5f, 0.5f};
};