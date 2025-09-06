#pragma once
#include "KamataEngine.h"
#include "algorithm"

using namespace KamataEngine;

class Particle 
{
public:

	// 初期化
	void Initialize(Model* model, Vector3 position, Vector3 velocity);
	// 更新
	void Update();
	// 描画
	void Draw(Camera* camera);

	bool isFinished_ = false; // 終了フラグ
	float counter_ = 0.0f;    // カウンター
	const float kDuration = 1.0f; // 寿命

	//デスフラグのゲッター
	bool IsFinished() const { return isFinished_; }

private:

	WorldTransform worldTransform_; // ワールド変形
	Model* model_ = nullptr;        // モデル

	ObjectColor objectcolor_;
	Vector4 color_;
	Vector3 velocity_; // 速度

};
