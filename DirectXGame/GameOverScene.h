#pragma once
#include "KamataEngine.h"
#include "BIt_Map_Font.h"

/// <summary>
/// ゲームオーバーシーン
/// </summary>
class GameOverScene {
public:
	~GameOverScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int currentScore, int bestScore);

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
	KamataEngine::Sprite* gameOverSprite_ = nullptr;
	uint32_t textureHandle_ = 0;

	// ▼▼▼ 以下を追加 ▼▼▼
	BIt_Map_Font* font_ = nullptr; // スコア表示用フォント
	int finalScore_ = 0;           // 最終スコア

	BIt_Map_Font* bestScoreFont_ = nullptr;          // ベストスコア表示用フォント
	int bestScore_ = 0;                              // ベストスコア
	KamataEngine::Sprite* bestTextSprite_ = nullptr; // 「BEST」の文字スプライト
	uint32_t bestTextTextureHandle_ = 0;
	uint32_t sfxConfirmHandle_ = 0;
};