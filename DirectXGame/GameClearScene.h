#pragma once
#include "KamataEngine.h"

/// <summary>
/// ゲームクリアシーン
/// </summary>
class GameClearScene {
public:
	~GameClearScene();

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
	/// タイトルに戻る準備ができたかを取得
	/// </summary>
	bool IsReturnToTitle() const { return isReturnToTitle_; }

private:
	bool isReturnToTitle_ = false;
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Sprite* gameClearSprite_ = nullptr;
	uint32_t textureHandle_ = 0;
};