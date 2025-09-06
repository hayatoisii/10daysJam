#include "Skydome.h"

Skydome::~Skydome()
{ 

}

void Skydome::Initialize(Model* model, Camera* viewProjection) {
	// assert(model);

	model_ = model;

	worldTransform_.Initialize();
	viewProjection_ = viewProjection;
}

void Skydome::Update() {}

void Skydome::Draw() { model_->Draw(worldTransform_, *viewProjection_); }