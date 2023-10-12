﻿
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DD;

// 격자를 그리는 함수
void SoftRenderer::DrawGizmo2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// 좌측 하단에서부터 격자 그리기
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// 그리드가 시작되는 좌하단 좌표 값 계산
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// 게임 오브젝트 목록


// 최초 씬 로딩을 담당하는 함수
void SoftRenderer::LoadScene2D()
{
	// 최초 씬 로딩에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();

}

// 게임 로직과 렌더링 로직이 공유하는 변수
Vector2 CurrentPosition;
float CurrentScale = 10.0f;
float CurrentDegree = 0.0f;

// 게임 로직을 담당하는 함수
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직의 로컬 변수
	static float MoveSpeed = 100.0f;
	static float ScaleMin = 5.0f;
	static float ScaleMax = 20.0f;
	static float ScaleSpeed = 20.0f;
	static float RotateSpeed = 180.0f;

	Vector2 InputVector = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)).GetNormalize();
	Vector2 DeltaPosition = InputVector * MoveSpeed * InDeltaSeconds;
	float DeltaScale = input.GetAxis(InputAxis::ZAxis) * ScaleSpeed * InDeltaSeconds;
	float DeltaDegree = input.GetAxis(InputAxis::WAxis) * RotateSpeed * InDeltaSeconds;

	// 물체의 최종 상태 설정
	CurrentPosition += DeltaPosition;
	CurrentScale = Math::Clamp(CurrentScale + DeltaScale, ScaleMin, ScaleMax);
	CurrentDegree += DeltaDegree;
}

// 렌더링 로직을 담당하는 함수
void SoftRenderer::Render2D()
{
	// 렌더링 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 배경에 격자 그리기
	DrawGizmo2D();

	// 렌더링 로직의 로컬 변수
	static float Increment = 0.001f;
	static float Radius = 50.0f;
	static std::vector<Vector2> Hearts;
	HSVColor HSV(0.0f, 1.0f, 0.85f);

	// 하트를 구성하는 점 생성
	if (Hearts.empty())
	{
		for (float rad = 0.0f; rad < Math::TwoPI; rad += Increment)
		{
			float sin = sinf(rad);
			float cos = cosf(rad);
			float cos2 = cosf(2 * rad);
			float cos3 = cosf(3 * rad);
			float cos4 = cosf(4 * rad);
			float x = 16.0f * sin * sin * sin;
			float y = 13 * cos - 5 * cos2 - 2 * cos3 - cos4;
			Hearts.push_back(Vector2(x, y));
		}
	}

	// 각도에 해당하는 사인과 코사인 값 얻기
	float Sin = 0.0f, Cos = 0.0f;
	Math::GetSinCos(Sin, Cos, CurrentDegree);

	// 각 값을 초기화한 후 색상을 증가시키면서 점에 대응
	float Rad = 0.0f;
	for (const auto& i : Hearts)
	{
		// 1. 점에 크기를 적용한다.
		Vector2 ScaledV = i * CurrentScale;
		// 2. 크기가 변한 점을 회전시킨다.
		Vector2 RotatedV = Vector2(ScaledV.X * Cos - ScaledV.Y * Sin, ScaledV.X * Sin + ScaledV.Y * Cos);
		// 3. 회전시킨 점을 이동한다.
		Vector2 TranslatedV = RotatedV + CurrentPosition;

		HSV.H = Rad / Math::TwoPI;
		r.DrawPoint(TranslatedV, HSV.ToLinearColor());
		Rad += Increment;
	}

	// 현재 위치, 크기, 각도를 화면에 출력
	r.PushStatisticText(std::string("Position : ") + CurrentPosition.ToString());
	r.PushStatisticText(std::string("Scale : ") + std::to_string(CurrentScale));
	r.PushStatisticText(std::string("Degree : ") + std::to_string(CurrentDegree));
}

// 메시를 그리는 함수
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// 삼각형을 그리는 함수
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}