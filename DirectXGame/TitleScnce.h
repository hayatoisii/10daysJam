#pragma once

#include "Skydome.h" // 天球のヘッダーファイルをインクルード
#include "player.h"
#include <KamataEngine.h>
#include <math/Vector2.h>
#include <random>

using namespace KamataEngine;

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScnce {

public:
	~TitleScnce();

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

	bool IsSelectFinished() const { return isFinished_; }

	// 現在表示されているスプライトを管理する変数
	size_t currentSpriteIndex = 0;
	std::vector<KamataEngine::Sprite*> sprites;

	// スプライトの初期化メソッド
	void InitializeSprites();

private:
	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 5.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = -5.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalklMotionTime = 1.0f;
	// タイマー
	float Timer_ = 0.0f;

	bool isFinished_ = false;
	bool isGameFinished_ = false;
	DirectXCommon* dxCommon_ = nullptr;
	WorldTransform titleWorldTransform_;
	WorldTransform titleWorldTransformFont_;
	Model* titlemodel_ = nullptr;
	Model* titlemodelFont_ = nullptr;
	uint32_t textureHandle_ = 0;
	Sprite* sprite_ = nullptr;
	uint32_t textureHandle2_ = 0;
	Sprite* sprite2_ = nullptr;
	uint32_t textureHandle3_ = 0;
	Sprite* sprite3_ = nullptr;
	uint32_t textureHandle4_ = 0;
	Sprite* sprite4_ = nullptr;

	// 天球
	Model* modelSkydome_ = nullptr;
	Skydome* skydome_ = nullptr;

	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	uint32_t TitleSEHandle_ = 0;
	uint32_t TitleSEHandle2_ = 0;
	uint32_t TitleSEHandle3_ = 0;
	uint32_t voiceHandle_ = 0;
	uint32_t voiceHandle2_ = 0;

	float sprite2MoveSpeed = 2.0f; // 移動速度
	bool isMovingDown = false;     // 下に移動中か否か
	bool isFlipped = false;        // スプライトが反転しているか否か
	float animationTimer = 0.0f;   // アニメーション用のタイマー

	// 画面中央付近での上下移動に調整（1280x720想定）
	const float FLIP_THRESHOLD = 150.0f;  // 上端での反転位置
	const float SCREEN_EDGE_Y = -400.0f;  // 画面上端付近
	const float SCREEN_BOTTOM_Y = 400.0f; // 画面下端付近

	// スプライト2用のワールドトランスフォーム
	WorldTransform sprite2WorldTransform_;

	// イージング用の変数
	bool isFlipping = false;   // 反転アニメーション中か
	float flipProgress = 0.0f; // 反転の進行度 (0.0f～1.0f)
	float flipDuration = 0.3f; // 反転にかける時間（秒）
	float currentScale = 1.0f; // 現在のスケール値

	// イージング関数
	float EaseInOutQuad(float t);
	float EaseOutBounce(float t);

	Model* modelEnd_ = nullptr;
	WorldTransform endTransformLeft_;
	WorldTransform endTransformRight_;

	// プラットフォーム生成タイマー（経過時間）
	float platformSpawnTimer = 0.0f;

	// 最後に生成したプラットフォームのX座標
	float lastPlatformX = 0.0f;

	// プラットフォームの左右生成フラグ（true:右、false:左）
	bool platformSideFlag = false;

	// プラットフォーム生成間隔（秒）
	const float platformSpawnInterval = 1.2f;

	const float minPlatformDistance = 4.0f;

	int playerHP_ = 3;

	// HP用モデル
	Model* hpModel_ = nullptr;
	// HP用ワールドトランスフォーム
	std::vector<WorldTransform*> hpWorldTransforms_;
	// プレイヤーオブジェクトへのポインタ
	Player* player_ = nullptr;
	// プレイヤーモデル
	Model* modelPlayer_ = nullptr;
	// プラットフォームモデル
	Model* modelPlatform_ = nullptr;
	// カメラ
	Camera camera_;
	// ワールド変換（汎用）
	WorldTransform worldTransform;
	// 動的に生成されるプラットフォームのコンテナ
	std::vector<Platform*> platforms_;
	// 乱数生成エンジン（メルセンヌ・ツイスタ）
	std::mt19937 randomEngine_;
	// ゲームオーバーフラグ
	bool isGameOver_ = false;
	// ゲームクリアフラグ
	bool isGameClear_ = false;
};
