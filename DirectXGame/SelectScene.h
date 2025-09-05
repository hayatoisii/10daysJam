#pragma once
#include "KamataEngine.h"

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
	bool isGameStart_ = false;
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Sprite* selectSprite_ = nullptr;
	uint32_t textureHandle_ = 0;
};