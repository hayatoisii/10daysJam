#pragma once
#include "KamataEngine.h"

/// <summary>
/// ポーズメニュー
/// </summary>
class PauseMenu {
public:
	~PauseMenu();

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
	/// ゲームに戻るかを取得
	/// </summary>
	bool IsResume() const { return isResume_; }

	/// <summary>
	/// タイトルに戻るかを取得
	/// </summary>
	bool IsQuit() const { return isQuit_; }

private:
	bool isResume_ = false;
	bool isQuit_ = false;
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Sprite* pauseSprite_ = nullptr;
	uint32_t textureHandle_ = 0;
};