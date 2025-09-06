#pragma once
#include "KamataEngine.h"
#include "Platform.h"
#include "Player.h"
#include <random>

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

private:
	// プレイヤーのX軸移動範囲を可視化するモデル
	KamataEngine::Model* modelEnd_ = nullptr;
	WorldTransform endTransformLeft_;
	WorldTransform endTransformRight_;

	// 重力反転ライン用
	KamataEngine::Model* modelGravityLineTop_ = nullptr;
	KamataEngine::Model* modelGravityLineBottom_ = nullptr;
	WorldTransform gravityLineTop_;
	WorldTransform gravityLineBottom_;

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

    // HP用モデル
    Model* hpModel_ = nullptr;

	Model* modelDamageTop_ = nullptr;    // 上面が危険な足場モデル
	Model* modelDamageBottom_ = nullptr; // 下面が危険な足場モデル

	const float spawnRateModifier = 1.2f; // ★この値を大きくすると、生成が遅くなります
	
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
};