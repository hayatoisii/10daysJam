#include "Graph.h"

Graph::~Graph() 
{
	delete sprite_;
	sprite_ = nullptr;
	delete sprite2_;
	sprite2_ = nullptr;
}

void Graph::Initialize() {
	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("HPR.png");
	textureHandle2_ = TextureManager::Load("HP.png");
	// スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, {10, 10});
	sprite2_ = Sprite::Create(textureHandle2_, {10, 10});
}

void Graph::Update() {

	hp_ -= 1.0f;
	if (hp_ <= 0.0f) {
		hp_ = hpMax_;
	}

	Vector2 size = sprite_->GetSize();
	size.x = hpMax_;
	size.y = 30;
	sprite_->SetSize(size);

	Vector2 size2 = sprite2_->GetSize();
	size2.x = hp_;
	size2.y = 30;
	sprite2_->SetSize(size2);

	Vector4 color = sprite_->GetColor();
	color.w = 0.5f;
	sprite_->SetColor(color);
}

void Graph::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());

	sprite_->Draw();
	sprite2_->Draw();

	Sprite::PostDraw();
}