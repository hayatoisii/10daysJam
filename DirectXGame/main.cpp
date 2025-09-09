#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScnce.h"
#include "SelectScene.h"
#include "PauseMenu.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include "HowToPlayScene.h"
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
enum class Scene { Title, Select, HowToPlay, Game, GameOver, Pause, GameClear };

Scene scene = Scene::Title;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// 初期化処理
	//  // エンジンの初期化

	KamataEngine::Initialize(L"LE3C_26_ムラタ_トモキ");

	int bestScore = LoadBestScore();

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ▼▼▼ オーディオ関連の処理をここにまとめます ▼▼▼
	Audio::GetInstance()->Initialize();
	// BGMの読み込み
	uint32_t titleBgmHandle = Audio::GetInstance()->LoadWave("audio/TitleBGM.wav"); // タイトル用BGM
	uint32_t gameBgmHandle = Audio::GetInstance()->LoadWave("audio/BGM.wav");       // ゲーム用BGM
	uint32_t sfxGameOverHandle = Audio::GetInstance()->LoadWave("audio/gameover.wav");
	// 再生中BGMのハンドル
	uint32_t titleBgmVoiceHandle = 0;
	uint32_t gameBgmVoiceHandle = 0;

	// main関数の前に
	TitleScnce* titleScnce = nullptr;
	SelectScene* selectScene = nullptr;
	GameScene* gameScnce = nullptr;
	GameOverScene* gameOverScene = nullptr;
	HowToPlayScene* howToPlayScene = nullptr;
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

	howToPlayScene = new HowToPlayScene();
	howToPlayScene->Initialize();

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
			// ▼▼▼ タイトルBGMの再生管理 ▼▼▼
			if (!Audio::GetInstance()->IsPlaying(titleBgmVoiceHandle)) {
				// 停止していたら再生を開始
				titleBgmVoiceHandle = Audio::GetInstance()->PlayWave(titleBgmHandle, true);
			}

			titleScnce->Update();
			titleScnce->Draw();
			if (titleScnce->IsSelectFinished()) {
				scene = Scene::Select;
			}
			break;

		case Scene::Select:
			// ▼▼▼ タイトルBGMが継続して再生されていることを確認 ▼▼▼
			if (!Audio::GetInstance()->IsPlaying(titleBgmVoiceHandle)) {
				titleBgmVoiceHandle = Audio::GetInstance()->PlayWave(titleBgmHandle, true);
			}

			selectScene->Update();
			selectScene->Draw();
			if (selectScene->IsGameStart()) {
				// ▼▼▼ ゲームシーンへの移行を、HowToPlayシーンへの移行に変更 ▼▼▼
				// BGMの切り替えは次のシーンで行うため、ここでは何もしない
				scene = Scene::HowToPlay;
				// gameScnce->Initialize() もここでは呼ばない
			}
			break;

		// ▼▼▼ HowToPlayシーンのケースを丸ごと追加 ▼▼▼
		case Scene::HowToPlay:
			// タイトルBGMを継続
			if (!Audio::GetInstance()->IsPlaying(titleBgmVoiceHandle)) {
				titleBgmVoiceHandle = Audio::GetInstance()->PlayWave(titleBgmHandle, true);
			}

			howToPlayScene->Update();
			howToPlayScene->Draw();

			// 操作説明シーンが終わったらゲームシーンへ
			if (howToPlayScene->IsFinished()) {
				// BGMの切り替え
				Audio::GetInstance()->StopWave(titleBgmVoiceHandle);
				gameBgmVoiceHandle = Audio::GetInstance()->PlayWave(gameBgmHandle, true);

				scene = Scene::Game;
				gameScnce->Initialize();      // ゲームシーンを初期化して開始
				howToPlayScene->Initialize(); // 次回のためにHowToPlayシーンもリセット
			}
			break;

		case Scene::Game:
			gameScnce->Update();
			gameScnce->Draw();
			if (gameScnce->IsGameOver()) {
				// ▼▼▼ ゲームオーバー時の処理 ▼▼▼
				Audio::GetInstance()->StopWave(gameBgmVoiceHandle); // ゲームBGMを停止
				Audio::GetInstance()->PlayWave(sfxGameOverHandle);  // ★ゲームオーバー効果音を再生

				int finalScore = gameScnce->GetScore();
				if (finalScore > bestScore) {
					bestScore = finalScore;
					SaveBestScore(bestScore);
				}
				gameOverScene->Initialize(finalScore, bestScore);
				scene = Scene::GameOver;

			} else if (gameScnce->IsGameClear()) {
				Audio::GetInstance()->StopWave(gameBgmVoiceHandle);
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
	delete howToPlayScene;    // 追加
	howToPlayScene = nullptr; // 追加
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
