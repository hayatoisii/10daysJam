#pragma once
#include "KamataEngine.h"
#include <cmath>

using namespace KamataEngine;

// イージング関数（EaseOutCubic）
float EaseOutCubic(float t);

/// <summary>
/// 選択シーン
/// </summary>
class SelectScene {
public:
	~SelectScene();

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
	/// ゲーム開始が決定したかを取得
	/// </summary>
	bool IsGameStart() const { return isGameStart_; }

private:
	/// <summary>
	/// 矢印の移動を開始する
	/// </summary>
	void StartArrowMovement();

	/// <summary>
	/// 矢印のアニメーション更新
	/// </summary>
	void UpdateArrowAnimation();

private:
	bool isGameStart_ = false;
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Sprite* selectSprite_ = nullptr;
	Sprite* selectSprite_1 = nullptr;
	Sprite* selectSprite_2 = nullptr;
	Sprite* selectSprite_3 = nullptr;

	Sprite* KeyselectSprite_ = nullptr;
	Sprite* arrowSprite_ = nullptr; // 矢印スプライト

	uint32_t textureHandle_ = 0;
	uint32_t textureHandle_1 = 0;
	uint32_t textureHandle_2 = 0;
	uint32_t textureHandle_3 = 0;
	uint32_t KeytextureHandle_ = 0;
	uint32_t arrowTextureHandle_ = 0; // 矢印テクスチャハンドル

	// 矢印の移動
	int currentSelectIndex_ = 0;
	float currentArrowX_ = 0.0f;
	float currentArrowY_ = 0.0f;
	float targetArrowX_ = 0.0f;
	float targetArrowY_ = 0.0f;
	bool isArrowMoving_ = false;

	// 矢印の浮遊アニメーション用
	float arrowVerticalOffset_ = 0.0f; // 垂直方向のオフセット
	float animationTimer_ = 0.0f;      // アニメーションタイマー
};