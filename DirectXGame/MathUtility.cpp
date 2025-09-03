#include "MathUtility.h"
#include "KamataEngine.h"

namespace KamataEngine {
namespace MathUtility {

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 mat = {};
	mat.m[0][0] = scale.x;
	mat.m[1][1] = scale.y;
	mat.m[2][2] = scale.z;
	mat.m[3][3] = 1.0f;
	return mat;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& trans) {
	Matrix4x4 mat = {};
	mat.m[0][0] = 1.0f;
	mat.m[1][1] = 1.0f;
	mat.m[2][2] = 1.0f;
	mat.m[3][3] = 1.0f;
	mat.m[3][0] = trans.x;
	mat.m[3][1] = trans.y;
	mat.m[3][2] = trans.z;
	return mat;
}

}
}