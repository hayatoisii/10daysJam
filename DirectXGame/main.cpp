#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScnce.h"
#include "SelectScene.h"
#include "PauseMenu.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include <Windows.h>

using namespace KamataEngine;

// main.cppの修正
enum class Scene { Title, Select, Game, GameOver, Pause, GameClear };

Scene scene = Scene::Title;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// 初期化処理
	//  // エンジンの初期化

	KamataEngine::Initialize(L"LE3C_26_ムラタ_トモキ");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// main関数の前に
	TitleScnce* titleScnce = nullptr;
	SelectScene* selectScene = nullptr;
	GameScene* gameScnce = nullptr;
	GameOverScene* gameOverScene = nullptr;
	PauseMenu* pauseMenu = nullptr;
	GameClearScene* gameClearScene = nullptr;

	// タイトルシーンの初期化
	titleScnce = new TitleScnce();
	titleScnce->Initialize();

	// 新しいシーンをインスタンス化
	selectScene = new SelectScene();
	selectScene->Initialize();

	gameScnce = new GameScene();
	gameScnce->Initialize();

	gameOverScene = new GameOverScene();
	gameOverScene->Initialize();

	pauseMenu = new PauseMenu();
	pauseMenu->Initialize();

	gameClearScene = new GameClearScene();
	gameClearScene->Initialize();

	// メインループ
	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		dxCommon->PreDraw();

		// シーンごとに処理を分岐
		switch (scene) {
		case Scene::Title:
			titleScnce->Update();
			titleScnce->Draw();
			if (titleScnce->IsSelectFinished()) {
				scene = Scene::Select;
			}
			break;

		case Scene::Select:
			selectScene->Update();
			selectScene->Draw();
			if (selectScene->IsGameStart()) {
				scene = Scene::Game;
				// ゲームシーンを再初期化して、新しいゲームを開始
				gameScnce->Initialize();
			}
			break;

		case Scene::Game:
			gameScnce->Update();
			gameScnce->Draw();
			// プレイヤーのHPが0になったらゲームオーバーシーンへ
			// 注: GameSceneにIsGameOver()メソッドを実装する必要があります
			if (gameScnce->IsGameOver()) {
				scene = Scene::GameOver;
			}
			// ゲームクリアの条件が満たされたらゲームクリアシーンへ
			// 注: GameSceneにIsGameClear()メソッドを実装する必要があります
			else if (gameScnce->IsGameClear()) {
				scene = Scene::GameClear;
			}
			// ポーズキー（例: Escape）が押されたらポーズメニューへ
			// 注: InputクラスのTriggerKeyメソッドを使用
			else if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
				scene = Scene::Pause;
			}
			break;

		case Scene::GameOver:
			gameOverScene->Update();
			gameOverScene->Draw();
			// ゲームオーバーシーンが終了したらタイトルシーンへ戻る
			if (gameOverScene->IsReturnToTitle()) {
				scene = Scene::Title;
				// タイトルシーンも再初期化
				titleScnce->Initialize();
			}
			break;

		case Scene::Pause:
			pauseMenu->Update();
			pauseMenu->Draw();
			// ポーズメニューからゲームに戻る
			if (pauseMenu->IsResume()) {
				scene = Scene::Game;
			}
			// ポーズメニューからタイトルへ戻る
			else if (pauseMenu->IsQuit()) {
				scene = Scene::Title;
				titleScnce->Initialize();
			}
			break;

		case Scene::GameClear:
			gameClearScene->Update();
			gameClearScene->Draw();
			// ゲームクリアシーンが終了したらタイトルシーンへ戻る
			if (gameClearScene->IsReturnToTitle()) {
				scene = Scene::Title;
				titleScnce->Initialize();
			}
			break;
		}

		dxCommon->PostDraw();
	}

	delete gameScnce;
	gameScnce = nullptr;
	delete titleScnce;
	titleScnce = nullptr;
	delete selectScene;
	selectScene = nullptr;
	delete gameOverScene;
	gameOverScene = nullptr;
	delete pauseMenu;
	pauseMenu = nullptr;
	delete gameClearScene;
	gameClearScene = nullptr;


	// ここでゲームシーンの終了処理を行う

	// 終了処理
	KamataEngine::Finalize();
	return 0;
}
