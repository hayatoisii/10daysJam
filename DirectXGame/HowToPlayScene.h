#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

/// <summary>
/// 操作説明シーン
/// </summary>
class HowToPlayScene {
public:
	~HowToPlayScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// シーンが終了したかを取得
	/// </summary>
	bool IsFinished() const { return isFinished_; }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	// 操作説明用のスプライト
	Sprite* howToPlaySprite_ = nullptr;
	uint32_t textureHandle_ = 0;

	// 決定音
	uint32_t sfxConfirmHandle_ = 0;

	bool isFinished_ = false;
};