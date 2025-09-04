#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "WorldTransformEx.h"

using namespace KamataEngine;

class Spike {
public:
	// コンストラクタ
	Spike();

	// 初期化
	void Initialize(const Vector3& pos, const Vector3& scale, Model* model, Camera* camera);

	// 更新
	void Update();

	// 描画
	void Draw();

	// AABBを取得
	const AABB& GetAABB() const { return aabb_; }

	// ワールド座標を取得
	Vector3 GetWorldPosition() const;

	// スクロール速度を設定
	void SetScrollSpeed(float speed);

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	Vector3 scale_ = {1.0f, 1.0f, 1.0f};
	Vector3 baseSize_ = {1.0f, 1.0f, 1.0f}; // とげのモデルの基本サイズ
	AABB aabb_;
	float spikeScrollSpeed_ = 0.2f; // とげの上方向移動速度
};