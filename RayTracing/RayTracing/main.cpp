#include <DxLib.h>
#include<math.h>
#include"Geometry.h"
#include "main.h"

const int screen_width = 640;
const int screen_height = 480;

//ヒントになると思って、色々と関数を用意しておりますが
//別にこの関数を使わなければいけないわけでも、これに沿わなければいけないわけでも
//ありません。レイトレーシングができていれば構いません。

///レイ(光線)と球体の当たり判定
///@param ray (視点からスクリーンピクセルへのベクトル)
///@param sphere 球
///@hint レイは正規化しといたほうが使いやすいだろう
bool IsHitRayAndObject(const Position3& eye, const Vector3& ray, const Sphere& sp,float& distance) {

	auto sphereToRay = sp.pos - eye;													//球体の中心までのベクトル

	//中心ベクトルと視線の内積をとります＝＞ベクトル長　=　射影
	auto perLen = Dot(sphereToRay, ray);												//射影長

	auto perRay = ray * perLen;															//

	auto perVec = sphereToRay - perRay;													//

	if (perVec.Magnitude() < sp.radius)
	{
		auto distanceToHitPos = sqrt(pow(sp.radius, 2) - pow(perVec.Magnitude(), 2));	//W

		auto power = perRay.Magnitude() - distanceToHitPos;

		distance = power;
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

///レイトレーシング
///@param eye 視点座標
///@param sphere 球オブジェクト(そのうち複数にする)
float Clamp(float val, float minVal = 0.0f, float maxVal = 1.0f)
{
	return max(min(val, maxVal), minVal);
}

/// inVector 入射ベクトル
/// nVector　当たった面の法線ベクトル
/// return 反射ベクトル
Vector3 ReflectVector(const Vector3& inVector, const Vector3& nVector)
{
	return inVector - nVector * 2 * (inVector * nVector);
}

struct Color
{
	unsigned char r, g, b;
	Color() :r(0), g(0), b(0) {};
	Color(unsigned char inr, unsigned char ing, unsigned char inb) :r(inr), g(ing), b(inb) {}
	void operator*=(float scale)
	{
		r = Clamp(r * scale, 0, 255);
		g = Clamp(g * scale, 0, 255);
		b = Clamp(b * scale, 0, 255);
	}
	void operator+=(const Color & incol)
	{
		r = Clamp(r + incol.r, 0, 255);
		g = Clamp(g + incol.g, 0, 255);
		b = Clamp(b + incol.b, 0, 255);
	}
	Color operator+(const Color & incol)
	{
		Color ret(r, g, b);
		ret.r = Clamp(ret.r + incol.r, 0, 255);
		ret.g = Clamp(ret.g + incol.g, 0, 255);
		ret.b = Clamp(ret.b + incol.b, 0, 255);
		return ret;
	}
	Color Max(const Color& col) {
		Color ret(r, g, b);
		ret.r = max(col.r, ret.r);
		ret.g = max(col.g, ret.g);
		ret.b = max(col.b, ret.b);
		return ret;
	}
	unsigned int GetCol()const {
		return DxLib::GetColor(r, g, b);
	}
};

Color GetCheckerColorPosition(Vector3 &as)
{
	if (((((int)(as.x) / 30) + (int)(as.z) / 30) % 2) == 0)
	{
		if ((int)(as.x) > 0)
		{
			return Color(255, 255, 255);
		}
		else
		{
			return Color(128, 128, 128);
		}
	}
	else
	{
		if ((int)(as.x) > 0)
		{
			return Color(128, 128, 128);
		}
		else
		{
			return Color(255, 255, 255);
		}
	}
}

void RayTracing(const Position3& eye, const Sphere& sphere, const Plane& plane) 
{
	auto distancepl = Dot(eye, plane.normal) - plane.offset;

	for (int y = 0; y < screen_height; ++y) {//スクリーン縦方向
		for (int x = 0; x < screen_width; ++x) {//スクリーン横方向

			//①視点とスクリーン座標から視線ベクトルを作る
			//ここでrayを作る
			Vector3 pos = { static_cast<float>(x - screen_width / 2),static_cast<float>(screen_height / 2 - y),0 };
			Vector3 ray = pos - eye;

			//②正規化をする
			ray.Normalize();

			auto distance = 0.0f;

			auto Light = Vector3(1, -1, -1);
			Light.Normalize();

			//③IsHitRay関数がTrueだったら塗りつぶす
			//※塗りつぶしはDrawPixel関数を使う。
			if (IsHitRayAndObject(eye, ray, sphere,distance))
			{
				auto normal = (ray * distance - (sphere.pos - eye)).Normalized();
				// 光線と球の法線ベクトルとの反射ベクトル
				auto refLight = ReflectVector(Light, normal);
				//照り返しをなくす為のClamp
				float spec = pow(Clamp(Dot(refLight, -ray)), 15.0f);

				auto deff = Dot(normal, -Light);
				deff = max(deff + spec, 0.25f);

				auto difCol = Color::Color(255, 0, 0);
				auto speCol = Color::Color(255, 255, 255);
				auto ambCol = Color::Color(32, 32, 32);
				auto col = Color::Color();

				difCol *= deff;
				speCol *= spec;

				col = difCol + speCol;
				col = col.Max(ambCol);

				auto tmpPos = ray * distance + eye;

				auto tmpRay = ReflectVector(ray, normal);	//反射ベクトル

				auto t = (Dot(tmpPos, plane.normal) - plane.offset) / Dot(-tmpRay, plane.normal);

				if (Dot(-tmpRay, plane.normal) > 0)
				{
					auto Tpos = tmpPos + tmpRay * t;
					//交点座標を求める
					auto color = GetCheckerColorPosition(Tpos);

					color.r /= 255.0f;
					color.g /= 255.0f;
					color.b /= 255.0f;

					DrawPixel(x, y, GetColor(col.r * color.r, col.g * color.g, col.b * color.b));
					continue;
				}

				DrawPixel(x, y, GetColor(col.r, col.g, col.b));
			}
			else
			{
				if (Dot(-ray, plane.normal) > 0)
				{
					auto rayplane = Dot(-ray, plane.normal);

					auto t = distancepl / rayplane;

					auto intercepl = eye + ray * t;

					DrawPixel(x, y, GetCheckerColorPosition(intercepl).GetCol());

					//影の実装
					if (IsHitRayAndObject(intercepl, -Light, sphere,distance))
					{
						auto col = GetCheckerColorPosition(intercepl);
						col.r *= 0.5;
						col.g *= 0.5;
						col.b *= 0.5;
						DrawPixel(x, y, col.GetCol());
					}
				}
			}
		}
	}
}

int main() {
	ChangeWindowMode(true);
	SetGraphMode(screen_width, screen_height, 32);
	SetMainWindowText(_T("1816007_尾崎　隼"));
	DxLib_Init();
	DrawBox(0, 0, screen_width, screen_height, GetColor(200,200,255), true);
	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)),Plane(Position3(0, 1, 0), -100));
	WaitKey();
	DxLib_End();
}