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

	void SetDamage(bool flag) { isDamage_ = flag; }

	// ダメージ処理を開始する関数
	void OnDamage();
	// 無敵状態かどうかを返す関数
	bool IsInvincible() const;

private:
	// 足場のスクロール速度
	float platformScrollSpeed = 0.2f;

	// 横移動速度
	const float speed = 0.42f;

	// 落下速度の上限
	const float maxFallSpeed = 1.0f;

	// 重力
	float gravity = -0.07f;//これいじっても変わらないよん

	// ジャンプ初速度
	const float jumpPower = 0.76f;

	// 足場のX座標制限（必要に応じてGameSceneから渡す設計に変更可）
	const float minPlatformX = -20.0f; // 左端
	const float maxPlatformX = 20.0f; 

	bool isOnGround_ = false;

	// 速度
	float velocityY_ = 0.0f;
	float velocityX_ = 0.0f;

	float targetGravity = -0.04f; // 目標重力
	float gravityLerpSpeed = 0.07f;
	
	// 重力切り替え後の一時的な低重力システム
	bool isGravityTransitioning_ = false; // 重力切り替え中フラグ
	float transitionGravity_ = -0.02f; // 切り替え後の低重力
	float normalGravity_ = -0.07f; // 通常重力

	// ジャンプ回数管理
	int jumpCount_ = 0;
	int maxJumpCount_ = 1; // ← ここを2にすれば二段ジャンプ

	bool isJumping_ = false;
	bool inversion = false;

	// ダメージ表示用
	bool isDamage_ = false;
	bool IsDamage() const { return isDamage_; }

	// 無敵状態かどうかを示すフラグ
	bool isInvincible_ = false;
	// 無敵時間タイマー
	float invincibilityTimer_ = 0.0f;

	Model* model_ = nullptr;
	Input* input_ = nullptr;
	Camera* camera_ = nullptr;

	WorldTransform worldTransform_;
	AABB aabb_;
	Vector3 halfSize_{0.5f, 0.5f, 0.5f};
};