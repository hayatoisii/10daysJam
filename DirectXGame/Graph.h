#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Graph {
public:
	
	~Graph();

	void Initialize();

	void Update();

	void Draw();

private:
	uint32_t textureHandle_ = 0;
	uint32_t textureHandle2_ = 0;

	Sprite* sprite_ = nullptr;
	Sprite* sprite2_ = nullptr;

	float hp_ = 200.0f;
	const float hpMax_ = 200.0f;
};
