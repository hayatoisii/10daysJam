#pragma once
#include "KamataEngine.h"
#include "Platform.h"
#include "Player.h"
#include "BIt_Map_Font.h"
#include <random>
#include <vector>

class GameScene {
public:
	// デストラクタ
	~GameScene();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// ▼▼▼ 以下の2行を追加 ▼▼▼
	bool IsGameOver() const { return isGameOver_; }
	bool IsGameClear() const { return isGameClear_; }
	int GetScore() const { return score_; }

private:

	// プレイヤーオブジェクトへのポインタ
	Player* player_ = nullptr;

	// プレイヤーモデル
	KamataEngine::Model* modelPlayer_ = nullptr;

	// プラットフォームモデル
	KamataEngine::Model* modelPlatform_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// ワールド変換（汎用）
	WorldTransform worldTransform;

	// 動的に生成されるプラットフォームのコンテナ
	std::vector<Platform*> platforms_;

	// 乱数生成エンジン（メルセンヌ・ツイスタ）
	std::mt19937 randomEngine_;
	std::vector<WorldTransform*> hpWorldTransforms_;

	// ゲーム状態フラグ
	bool isGameOver_ = false;
	bool isGameClear_ = false;

	// プレイヤーのX軸移動範囲を可視化するモデル
	KamataEngine::Model* modelEnd_ = nullptr;
	WorldTransform endTransformLeft_;
	WorldTransform endTransformRight_;

	// ▼▼▼ 重力反転ライン用スプライトのメンバ変数を追加 ▼▼▼
	uint32_t spriteGravityLineTopHandle_ = 0;
	uint32_t spriteGravityLineBottomHandle_ = 0;
	Sprite* spriteGravityLineTop_ = nullptr;
	Sprite* spriteGravityLineBottom_ = nullptr;

	uint32_t skyTextureHandle_ = 0;
	Sprite* skySprite1_ = nullptr;
	Sprite* skySprite2_ = nullptr;

	// HP用モデル
	Model* hpModel_ = nullptr;

	Model* modelDamageTop_ = nullptr;    // 上面が危険な足場モデル
	Model* modelDamageBottom_ = nullptr; // 下面が危険な足場モデル

	Model* modelPlatformItemSpeedReset_ = nullptr;
	Model* modelItemHpRecovery_ = nullptr; // ★追加：HP回復アイテムモデル

	KamataEngine::Model* modelBackground_ = nullptr;
	WorldTransform transformBackground_;

	// プラットフォーム生成タイマー（経過時間）
	float platformSpawnTimer = 1.5f;

	// 最後に生成したプラットフォームのX座標
	float lastPlatformX = 0.0f;

	// プラットフォームの左右生成フラグ（true:右、false:左）
	bool platformSideFlag = false;

	// プラットフォーム生成間隔（秒）
	const float platformSpawnInterval = 0.8f;

	// ゲーム時間と速度倍率
	float gameTime_ = 0.0f;
	float speedMultiplier_ = 1.0f;

	const float minPlatformDistance = 4.0f;

	int playerHP_ = 3;

	const int platformCount = 1; // 足場の数

	const float spawnRateModifier = 1.1f; // ★この値を大きくすると、生成が遅くなります 1.2

	// スコア表示用フォント
	BIt_Map_Font* font_ = nullptr;
	int score_ = 0;
	int prevScore_ = 0; // 追加
	Vector3 prevPlayerPos_; // 前フレームのプレイヤー位置
	bool prevOnGround_ = false; // 前フレームの地面判定

	// アイテム取得時の効果音ハンドル
	uint32_t sfxHealHandle_ = 0;
	uint32_t sfxClockHandle_ = 0;
	uint32_t sfxJumpHandle_ = 0;
	uint32_t sfxDamageHandle_ = 0;
	uint32_t sfxWarningHandle_ = 0;
	uint32_t warningVoiceHandle_ = 0; // 警告音の再生ハンドル
	bool isWarningPlaying_ = false;   // 警告音が再生中かどうかのフラグ

	// ▼▼▼ ゲームオーバーになるまでの猶予時間を 2.0f から 4.0f に変更 ▼▼▼
	const float kGameOverDelay_ = 4.0f; // ゲームオーバーになるまでの猶予時間（4秒）
	float gameOverTimer_ = 0.0f;        // ゲームオーバータイマー
};