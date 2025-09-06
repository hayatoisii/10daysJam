#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

/// <summary>
/// 天球
/// </summary>
class Skydome {
public:

	~Skydome();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, Camera* viewProjection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// ビュー変換データ
	Camera* viewProjection_ = nullptr;
	// モデル
	Model* model_ = nullptr;
};
