#include "Particle.h"
#include "math/MathUtility.h"

using namespace MathUtility;

void Particle::Initialize(Model* model,Vector3 position, Vector3 velocity) 
{ 
	assert(model);
	model_ = model;
	velocity_ = velocity; // 速度を初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position; // ワールド変形の位置を初期化

	//大きさ
	worldTransform_.scale_ = {0.2f, 0.2f, 0.2f}; // ワールド変形の大きさを初期化

	objectcolor_.Initialize();
	color_ = {1, 1, 0, 1}; // RGBA形式で色を指定
}

void Particle::Update() 
{
	//フェード処理
	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f); // アルファ値を更新

	if (isFinished_) {
		return;
	}

	counter_ += 1.0f / 60.0f; // カウンターを更新

	if (counter_ >= kDuration) {

		counter_ = kDuration; // カウンターを最大値にする

		isFinished_ = true; // 寿命を超えたら終了
	}

	// 修正箇所: +=演算子から、明示的な加算と代入に書き換える
	worldTransform_.translation_ = worldTransform_.translation_.operator+(velocity_);

	worldTransform_.TransferMatrix(); // ワールド変形の転送
	worldTransform_.UpdateMatarix();  // ワールド変形の更新

	objectcolor_.SetColor(color_); // 色を設定
}

void Particle::Draw(Camera* camera) {

	model_->Draw(worldTransform_, *camera, &objectcolor_); // 必要な引数を渡す
}
