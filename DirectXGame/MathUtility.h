#pragma once
#include "KamataEngine.h"

namespace KamataEngine {
namespace MathUtility {

Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeTranslateMatrix(const Vector3& trans);

}
}
