#include "Graph.h"

Graph::~Graph() {
	// ハートスプライトを解放
	for (int i = 0; i < 3; ++i) {
		delete hpSprites_[i];
		hpSprites_[i] = nullptr;
	}
}

void Graph::Initialize() {
	// ハートテクスチャの読み込み
	hpTextureHandle_ = TextureManager::Load("HP.png");

	// 3つのハートスプライトを生成
	for (int i = 0; i < 3; ++i) {
		hpSprites_[i] = Sprite::Create(hpTextureHandle_, {50.0f + i * 60.0f, 50.0f});
	}
}

void Graph::Update(int hp) {
	// 現在のHPを更新
	currentHP_ = hp;
}

void Graph::Draw() {
	// 現在のHPの数だけハートを描画
	for (int i = 0; i < currentHP_; ++i) {
		if (hpSprites_[i]) {
			hpSprites_[i]->Draw();
		}
	}
}