#include "Spike.h"
#include <cassert>

Spike::Spike() {}

void Spike::Initialize(const Vector3& pos, const Vector3& scale, Model* model, Camera* camera) {
	assert(model);
	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = scale;
	model_ = model;
	camera_ = camera;
	scale_ = scale;

	Vector3 halfSize = (baseSize_ * scale_) * 0.5f;
	aabb_.Set(pos - halfSize, pos + halfSize);
}

void Spike::SetScrollSpeed(float speed) { spikeScrollSpeed_ = speed; }

Vector3 Spike::GetWorldPosition() const { return worldTransform_.translation_; }

void Spike::Update() {
	worldTransform_.translation_.y += spikeScrollSpeed_;
	Vector3 pos = worldTransform_.translation_;
	Vector3 halfSize = (baseSize_ * scale_) * 0.5f;
	aabb_.Set(pos - halfSize, pos + halfSize);
	worldTransform_.UpdateMatarix();
}

void Spike::Draw() {
	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}