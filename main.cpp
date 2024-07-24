#include <Novice.h>
#include <Vector3.h>
#include <Matrix4x4.h>
#include <cstdint>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>
#include <imgui.h>
#include <numbers>

const char kWindowTitle[] = "LE2B_13_サトウ_リュウセイ_MT3_03_00";

struct Sphere {
	Vector3 centers;
	float radius;
	unsigned int color;
};
const float kWindowWidth = 1280.0f;
const float kWindowHeight = 720.0f;

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3 translate);

// x軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

// y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

// z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// lerp関数
Vector3 lerp(Vector3 easeStart, Vector3 easeEnd, float t);

Vector3 Bezier(const Vector3 p0, const Vector3 p1, const Vector3 p2, float t);

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color);

Vector3 GetWorldPos(const Matrix4x4& m) {
	Vector3 worldPos{};
	worldPos.x = m.m[3][0];
	worldPos.y = m.m[3][1];
	worldPos.z = m.m[3][2];
	return worldPos;
}

void DrawLine(Vector3 v1, Vector3 v2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix);
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Sphere sphere[3]{};
	Vector3 rotate = { 0.26f,0.0f,0.0f };

	Vector3 cameraTranslate{ 0.0f,0.0f,-10.0f };
	Vector3 cameraRotate{ 0.0f,0.0f,0.0f };

	Vector3 translates[3] = {
		{ 0.2f,1.0f,0.0f },
		{ 0.4f,0.0f,0.0f },
		{ 0.3f,0.0f,0.0f }
	};

	Vector3 rotates[3] = {
		{ 0.0f,0.0f,-6.8f },
		{ 0.0f,0.0f,-1.4f },
		{ 0.0f,0.0f,0.0f }
	};

	Vector3 scales[3] = {
		{ 1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f }
	};

	sphere[0].radius = 0.05f;
	sphere[1].radius = 0.05f;
	sphere[2].radius = 0.05f;

	sphere[0].color = RED;
	sphere[1].color = GREEN;
	sphere[2].color = BLUE;

	Matrix4x4 worldShoulderMatrix = {};
	Matrix4x4 worldElbowMatrix = {};
	Matrix4x4 worldHandMatrix = {};

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &rotate.x, 0.01f);
		ImGui::DragFloat3("sphere0.translate", &translates[0].x, 0.01f);
		ImGui::DragFloat3("sphere0.rotate", &rotates[0].x, 0.01f);
		ImGui::DragFloat3("sphere0.scales", &scales[0].x, 0.01f);
		ImGui::DragFloat3("sphere1.translate", &translates[1].x, 0.01f);
		ImGui::DragFloat3("sphere1.rotate", &rotates[1].x, 0.01f);
		ImGui::DragFloat3("sphere1.scales", &scales[1].x, 0.01f);
		ImGui::DragFloat3("sphere2.translate", &translates[2].x, 0.01f);
		ImGui::DragFloat3("sphere2.rotate", &rotates[2].x, 0.01f);
		ImGui::DragFloat3("sphere2.scales", &scales[2].x, 0.01f);
		ImGui::End();
		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, { 0,0,0 });
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 worldViewMatrix = Inverse(worldMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldViewMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewPortMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		worldShoulderMatrix = MakeAffineMatrix(scales[0], rotates[0], translates[0]);
		worldElbowMatrix = MakeAffineMatrix(scales[1], rotates[1], translates[1]);
		worldHandMatrix = MakeAffineMatrix(scales[2], rotates[2], translates[2]);

		worldElbowMatrix = Multiply(worldElbowMatrix, worldShoulderMatrix);
		worldHandMatrix = Multiply(worldHandMatrix, worldElbowMatrix);

		sphere[0].centers =GetWorldPos(worldShoulderMatrix);
		sphere[1].centers =GetWorldPos(worldElbowMatrix);
		sphere[2].centers =GetWorldPos(worldHandMatrix);
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		DrawGrid(worldViewProjectionMatrix, viewPortMatrix);
		DrawSphere(sphere[0], worldViewProjectionMatrix, viewPortMatrix);
		DrawSphere(sphere[1], worldViewProjectionMatrix, viewPortMatrix);
		DrawSphere(sphere[2], worldViewProjectionMatrix, viewPortMatrix);
		DrawLine(sphere[0].centers, sphere[1].centers, worldViewProjectionMatrix, viewPortMatrix);
		DrawLine(sphere[1].centers, sphere[2].centers, worldViewProjectionMatrix, viewPortMatrix);
		//DrawBezier(controlPoint[0].center, controlPoint[1].center, controlPoint[2].center, worldViewProjectionMatrix, viewPortMatrix, 0xFFFFFFFF);
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
// グリッドの描画
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	const float kGridHalfWidth = 2.0f;                                      // Gridの半分の幅
	const uint32_t kSubdivision = 10;                                       // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 1つ分の長さ

	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {

		float posX = -kGridHalfWidth + kGridEvery * xIndex;

		Vector3 start = { posX, 0.0f, -kGridHalfWidth };
		Vector3 end = { posX, 0.0f, kGridHalfWidth };

		start = Transform(start, Multiply(viewProjectionMatrix, viewportMatrix));
		end = Transform(end, Multiply(viewProjectionMatrix, viewportMatrix));

		// 左から右も同じように順々に引いていく
		for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
			float posZ = -kGridHalfWidth + kGridEvery * zIndex;

			Vector3 startZ = { -kGridHalfWidth, 0.0f, posZ };
			Vector3 endZ = { kGridHalfWidth, 0.0f, posZ };

			startZ = Transform(startZ, Multiply(viewProjectionMatrix, viewportMatrix));
			endZ = Transform(endZ, Multiply(viewProjectionMatrix, viewportMatrix));

			Novice::DrawLine((int)start.x, (int)start.y, (int)end.x, (int)end.y, 0xFFFFFFFF);
			Novice::DrawLine((int)startZ.x, (int)startZ.y, (int)endZ.x, (int)endZ.y, 0xFFFFFFFF);
		}
	}
}
// 球の描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	const uint32_t kSubdivision = 10; // 分割数
	const float kLatEvery = (float)M_PI / kSubdivision; // 経度分割1つ分の角度
	const float kLonEvery = (2 * (float)M_PI) / kSubdivision; // 緯度分割1つ分の角度

	// 緯度の方向に分割 -Π/2 ~ Π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {

		float lat = -(float)M_PI / 2.0f + latIndex * kLatEvery; // 現在の緯度

		// 経度の方向に分割 0 ~ 2Π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {

			float lon = lonIndex * kLonEvery; // 現在の経度

			// World座標系でのa、b、cを求める
			Vector3 a, b, c;

			a = {
				sphere.centers.x + sphere.radius * cosf(lat) * cosf(lon),
				sphere.centers.y + sphere.radius * sinf(lat),
				sphere.centers.z + sphere.radius * cosf(lat) * sinf(lon)
			};

			b = {
				sphere.centers.x + sphere.radius * cosf(lat + kLatEvery) * cosf(lon),
				sphere.centers.y + sphere.radius * sinf(lat + kLatEvery),
				sphere.centers.z + sphere.radius * cosf(lat + kLatEvery) * sinf(lon)
			};

			c = {
				sphere.centers.x + sphere.radius * cosf(lat) * cosf(lon + kLonEvery),
				sphere.centers.y + sphere.radius * sinf(lat),
				sphere.centers.z + sphere.radius * cosf(lat) * sinf(lon + kLonEvery)
			};

			// a、b、cをScreen座標系まで変換
			a = Transform(a, Multiply(viewProjectionMatrix, viewportMatrix));
			b = Transform(b, Multiply(viewProjectionMatrix, viewportMatrix));
			c = Transform(c, Multiply(viewProjectionMatrix, viewportMatrix));

			// ab、bcで線を引く
			Novice::DrawLine(int(a.x), int(a.y), int(b.x), int(b.y), sphere.color);
			Novice::DrawLine(int(a.x), int(a.y), int(c.x), int(c.y), sphere.color);
		}
	}
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 resultMultiply = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			resultMultiply.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}
	return resultMultiply;
}
// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 resultInverse = {};
	float A = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];
	resultInverse.m[0][0] = (
		m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) / A;
	resultInverse.m[0][1] = (
		-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) / A;
	resultInverse.m[0][2] = (
		m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) / A;
	resultInverse.m[0][3] = (
		-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) / A;
	resultInverse.m[1][0] = (
		-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2]
		+ m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) / A;
	resultInverse.m[1][1] = (
		m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) / A;
	resultInverse.m[1][2] = (
		-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2]
		+ m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) / A;
	resultInverse.m[1][3] = (
		m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) / A;
	resultInverse.m[2][0] = (
		m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1]
		- m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) / A;
	resultInverse.m[2][1] = (
		-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1]
		+ m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) / A;
	resultInverse.m[2][2] = (
		m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1]
		- m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) / A;
	resultInverse.m[2][3] = (
		-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1]
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) / A;
	resultInverse.m[3][0] = (
		-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1]
		+ m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) / A;
	resultInverse.m[3][1] = (
		m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) / A;
	resultInverse.m[3][2] = (
		-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1]
		+ m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) / A;
	resultInverse.m[3][3] = (
		m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) / A;
	return resultInverse;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 resultTransform = {};
	resultTransform.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	resultTransform.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	resultTransform.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];

	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	resultTransform.x /= w;
	resultTransform.y /= w;
	resultTransform.z /= w;

	return resultTransform;
}

// アフィン変換
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3 translate) {
	Matrix4x4 resultAffineMatrix = {};
	Matrix4x4 resultRotateXYZMatrix = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));
	resultAffineMatrix.m[0][0] = scale.x * resultRotateXYZMatrix.m[0][0];
	resultAffineMatrix.m[0][1] = scale.x * resultRotateXYZMatrix.m[0][1];
	resultAffineMatrix.m[0][2] = scale.x * resultRotateXYZMatrix.m[0][2];
	resultAffineMatrix.m[1][0] = scale.y * resultRotateXYZMatrix.m[1][0];
	resultAffineMatrix.m[1][1] = scale.y * resultRotateXYZMatrix.m[1][1];
	resultAffineMatrix.m[1][2] = scale.y * resultRotateXYZMatrix.m[1][2];
	resultAffineMatrix.m[2][0] = scale.z * resultRotateXYZMatrix.m[2][0];
	resultAffineMatrix.m[2][1] = scale.z * resultRotateXYZMatrix.m[2][1];
	resultAffineMatrix.m[2][2] = scale.z * resultRotateXYZMatrix.m[2][2];
	resultAffineMatrix.m[3][0] = translate.x;
	resultAffineMatrix.m[3][1] = translate.y;
	resultAffineMatrix.m[3][2] = translate.z;
	resultAffineMatrix.m[3][3] = 1;
	return resultAffineMatrix;
}

// x軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 resultRotateXMatrix = {};
	resultRotateXMatrix.m[0][0] = 1;
	resultRotateXMatrix.m[1][1] = std::cos(radian);
	resultRotateXMatrix.m[1][2] = std::sin(radian);
	resultRotateXMatrix.m[2][1] = -std::sin(radian);
	resultRotateXMatrix.m[2][2] = std::cos(radian);
	resultRotateXMatrix.m[3][3] = 1;
	return resultRotateXMatrix;
}

// y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 resultRotateYMatrix = {};
	resultRotateYMatrix.m[0][0] = std::cos(radian);
	resultRotateYMatrix.m[0][2] = -std::sin(radian);
	resultRotateYMatrix.m[1][1] = 1;
	resultRotateYMatrix.m[2][0] = std::sin(radian);
	resultRotateYMatrix.m[2][2] = std::cos(radian);
	resultRotateYMatrix.m[3][3] = 1;
	return resultRotateYMatrix;
}

// z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 resultRotateZMatrix = {};
	resultRotateZMatrix.m[0][0] = std::cos(radian);
	resultRotateZMatrix.m[0][1] = std::sin(radian);
	resultRotateZMatrix.m[1][0] = -std::sin(radian);
	resultRotateZMatrix.m[1][1] = std::cos(radian);
	resultRotateZMatrix.m[2][2] = 1;
	resultRotateZMatrix.m[3][3] = 1;
	return resultRotateZMatrix;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 resultPerspectiveFovMatrix = {};
	resultPerspectiveFovMatrix.m[0][0] = (1 / aspectRatio) * (1 / std::tan(fovY / 2));
	resultPerspectiveFovMatrix.m[1][1] = 1 / std::tan(fovY / 2);
	resultPerspectiveFovMatrix.m[2][2] = farClip / (farClip - nearClip);
	resultPerspectiveFovMatrix.m[2][3] = 1;
	resultPerspectiveFovMatrix.m[3][2] = -nearClip * farClip / (farClip - nearClip);
	return resultPerspectiveFovMatrix;
}

// ビューポート変換
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 resultViewPortMatrix = {};
	resultViewPortMatrix.m[0][0] = width / 2.0f;
	resultViewPortMatrix.m[1][1] = -height / 2.0f;
	resultViewPortMatrix.m[2][2] = maxDepth - minDepth;
	resultViewPortMatrix.m[3][0] = left + (width / 2);
	resultViewPortMatrix.m[3][1] = top + (height / 2);
	resultViewPortMatrix.m[3][2] = minDepth;
	resultViewPortMatrix.m[3][3] = 1;
	return resultViewPortMatrix;
}

Vector3 lerp(Vector3 easeStart, Vector3 easeEnd, float t) {
	easeStart.x = (1.0f - t) * easeStart.x + t * easeEnd.x;
	easeStart.y = (1.0f - t) * easeStart.y + t * easeEnd.y;
	easeStart.z = (1.0f - t) * easeStart.z + t * easeEnd.z;
	return easeStart;
}

Vector3 Bezier(Vector3 p0, Vector3 p1, Vector3 p2, float t) {
	Vector3 p0p1 = lerp(p0, p1, t);
	Vector3 p1p2 = lerp(p1, p2, t);
	Vector3 p = lerp(p0p1, p1p2, t);
	return p;
}

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color) {
	int index = 0;
	int split = 32;
	float t0 = 0;
	float t1 = 0;
	Vector3 bezier0 = {};
	Vector3 bezier1 = {};
	for (index = 0; index < split; index++) {
		t0 = index / float(split);
		t1 = (index + 1) / float(split);
		// t0とt1と制御点をつかってベジェ曲線上の点を求める
		bezier0 = Bezier(controlPoint0, controlPoint1, controlPoint2, t0);
		bezier1 = Bezier(controlPoint0, controlPoint1, controlPoint2, t1);
		// それぞれの点をスクリーン座標に変換
		//
		bezier0 = Transform(bezier0, Multiply(viewProjectionMatrix, viewPortMatrix));
		bezier1 = Transform(bezier1, Multiply(viewProjectionMatrix, viewPortMatrix));

		// スクリーン座標に変換した点を利用して線を描画
		Novice::DrawLine(
			static_cast<int>(bezier0.x), static_cast<int>(bezier0.y),
			static_cast<int>(bezier1.x), static_cast<int>(bezier1.y), color);
	}
}

void DrawLine(Vector3 v1, Vector3 v2,const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix){
	v1 = Transform(v1, Multiply(viewProjectionMatrix, viewPortMatrix));
	v2 = Transform(v2, Multiply(viewProjectionMatrix, viewPortMatrix));
	Novice::DrawLine(int(v1.x), int(v1.y), int(v2.x), int(v2.y), 0xFFFFFFFF);
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 resultCross = {};
	resultCross.x = v1.y * v2.z - v1.z * v2.y;
	resultCross.y = v1.z * v2.x - v1.x * v2.z;
	resultCross.z = v1.x * v2.y - v1.y * v2.x;
	return resultCross;
}