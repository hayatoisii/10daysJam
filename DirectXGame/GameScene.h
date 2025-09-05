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
	KamataEngine::Model* modelEnd_ = nullptr;
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
    KamataEngine::Model* hpModel_ = nullptr;

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