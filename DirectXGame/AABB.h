#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class AABB {
public:
	AABB() = default;
	AABB(const Vector3& min, const Vector3& max) : min_(min), max_(max) {}

	void Set(const Vector3& min, const Vector3& max) {
		min_ = min;
		max_ = max;
	}

	const Vector3& GetMin() const { return min_; }
	const Vector3& GetMax() const { return max_; }

	bool IsColliding(const AABB& other) const {
		return (min_.x <= other.max_.x && max_.x >= other.min_.x) && (min_.y <= other.max_.y && max_.y >= other.min_.y) && (min_.z <= other.max_.z && max_.z >= other.min_.z);
	}


	Vector3 min_;
	Vector3 max_;
};