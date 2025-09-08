#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScnce.h"
#include "SelectScene.h"
#include "PauseMenu.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include <fstream> // ▼▼▼ 追加 ▼▼▼
#include <string>  // ▼▼▼ 追加 ▼▼▼
#include <Windows.h>

using namespace KamataEngine;

int LoadBestScore() {
	std::ifstream file("best_score.txt");
	int score = 0;
	if (file.is_open()) {
		std::string line;
		if (std::getline(file, line)) {
			try {
				score = std::stoi(line);
			} catch (...) {
				// ファイル内容が不正な場合は0にする
				score = 0;
			}
		}
		file.close();
	}
	return score;
}

// ベストスコアをファイルへ書き込む関数
void SaveBestScore(int score) {
	std::ofstream file("best_score.txt");
	if (file.is_open()) {
		file << score;
		file.close();
	}
}

// main.cppの修正
enum class Scene { Title, Select, Game, GameOver, Pause, GameClear };

Scene scene = Scene::Title;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// 初期化処理
	//  // エンジンの初期化

	KamataEngine::Initialize(L"LE3C_26_ムラタ_トモキ");

	int bestScore = LoadBestScore();

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Audio::GetInstance()->Initialize();
	// "BGM.wav"を読み込み、サウンドデータハンドルとして保持
	uint32_t bgmSoundHandle = Audio::GetInstance()->LoadWave("audio/BGM.wav");
	// 再生中のBGMを識別するためのボイスハンドル（最初は0で初期化）
	uint32_t bgmVoiceHandle = 0;

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
	//gameOverScene->Initialize();

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
				gameScnce->Initialize();
				// ▼▼▼ ゲームシーン開始時にBGMの再生を開始します（trueでループ再生）▼▼▼
				bgmVoiceHandle = Audio::GetInstance()->PlayWave(bgmSoundHandle, true);
			}
			break;

		case Scene::Game:
			gameScnce->Update();
			gameScnce->Draw();
			if (gameScnce->IsGameOver()) {
				// ▼▼▼ ゲームオーバー時にBGMを停止します ▼▼▼
				Audio::GetInstance()->StopWave(bgmVoiceHandle);

				int finalScore = gameScnce->GetScore();
				if (finalScore > bestScore) {
					bestScore = finalScore;
					SaveBestScore(bestScore);
				}
				gameOverScene->Initialize(finalScore, bestScore);
				scene = Scene::GameOver;
			} else if (gameScnce->IsGameClear()) {
				// ▼▼▼ ゲームクリア時もBGMを停止させます ▼▼▼
				Audio::GetInstance()->StopWave(bgmVoiceHandle);
				scene = Scene::GameClear;
			}
			break;

			// GameClearシーンからタイトルに戻る際
		case Scene::GameClear:
			gameClearScene->Update();
			gameClearScene->Draw();
			if (gameClearScene->IsReturnToTitle()) {
				scene = Scene::Title;
				titleScnce->Initialize();
				selectScene->Initialize(); // ★★★この行を追加★★★
			}
			break;

		// GameOverシーンからタイトルに戻る際
		case Scene::GameOver:
			gameOverScene->Update();
			gameOverScene->Draw();
			if (gameOverScene->IsReturnToTitle()) {
				scene = Scene::Title;
				titleScnce->Initialize();
				selectScene->Initialize(); // ★★★この行を追加★★★
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

	Audio::GetInstance()->Finalize();
	// 終了処理
	KamataEngine::Finalize();
	return 0;
}
