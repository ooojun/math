#include <DxLib.h>
#include<math.h>
#include"Geometry.h"
const int screen_width = 640;
const int screen_height = 480;

//ヒントになると思って、色々と関数を用意しておりますが
//別にこの関数を使わなければいけないわけでも、これに沿わなければいけないわけでも
//ありません。レイトレーシングができていれば構いません。

///レイ(光線)と球体の当たり判定
///@param ray (視点からスクリーンピクセルへのベクトル)
///@param sphere 球
///@param distance 交点までの距離
///@hint レイは正規化しといたほうが使いやすいだろう
bool IsHitRayAndObject(const Position3& eye, const Vector3& ray, const Sphere& sp, float &distance) {
	//レイが正規化済みである前提で…
	//
	//視点から球体中心へのベクトル(視線)を作ります
	//
	auto C = sp.pos - eye;
	//中心ベクトルと視線の内積をとります＝＞ベクトル長　=　射影
	auto R = Dot(C, ray);

	auto vec = ray * R;

	auto Vec = C - vec;

	if (Vec.Magnitude() < sp.radius)
	{
		// 当たっていたらdistanceを返す
		auto Magni = sqrt(sp.radius * sp.radius - Vec.Magnitude() * Vec.Magnitude());

		auto power = vec.Magnitude() - Magni;

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

/// inVector 入社ベクトル
/// nVector　当たった面の法専ベクトル（正規化済み）
/// return 反射ベクトル
Vector3 ReflectVector(const Vector3& inVector, const Vector3& nVector)
{
	return inVector - nVector * 2 * (inVector * nVector);
}

void RayTracing(const Position3& eye, const Sphere& sphere) {
	struct Col
	{
		unsigned char r, g, b;
		Col() :r(0), g(0), b(0) {};
		Col(unsigned char inr, unsigned char ing, unsigned char inb) :r(inr), g(ing), b(inb) {}
		void operator*=(float scale)
		{
			r = Clamp(r * scale, 0, 255);
			g = Clamp(g * scale, 0, 255);
			b = Clamp(b * scale, 0, 255);
		}

		void operator+=(const Col & incol)
		{
			r = Clamp(r + incol.r, 0, 255);
			g = Clamp(g + incol.g, 0, 255);
			b = Clamp(b + incol.b, 0, 255);
		}

		Col operator+(const Col & incol)
		{
			Col ret(r, g, b);
			ret.r = Clamp(ret.r + incol.r, 0, 255);
			ret.g = Clamp(ret.g + incol.g, 0, 255);
			ret.b = Clamp(ret.b + incol.b, 0, 255);
			return ret;
		}

		Col Max(const Col& col) {
			Col ret(r, g, b);
			ret.r = max(col.r, ret.r);
			ret.g = max(col.g, ret.g);
			ret.b = max(col.b, ret.b);
			return ret;
		}
	};
	for (int y = 0; y < screen_height; ++y) {//スクリーン縦方向
		for (int x = 0; x < screen_width; ++x) {//スクリーン横方向
			//①視点とスクリーン座標から視線ベクトルを作る
			//ここでrayを作る
			Vector3 pos = { static_cast<float>(x - screen_width / 2), static_cast<float>(y - screen_height / 2) , 0 };
			Vector3 ray = pos - eye;
			Vector3 PlaneVec(0, 1, 0);

			//②正規化をする
			ray.Normalize();
			//③IsHitRay関数がTrueだったら塗りつぶす
			//※塗りつぶしはDrawPixel関数を使う。

		/*	float brightness = min(max(normal*toLight,0) + ambient, 1.0f);
*/

			float distance = 0.0f;

			if (IsHitRayAndObject(eye, ray, sphere, distance))
			{
				/*	auto W = (sphere.radius * sphere.radius);

					auto N = vec.Magnitude() - W;*/

				int Color = 255;
				auto N = (ray * distance - (sphere.pos - eye)).Normalized();
				auto Light = Vector3(1, 1, -1);
				Light.Normalize();

				// 反射ベクトル
				auto rLight = ReflectVector(Light, N);

				// pow関数 pow(元の数,乗数)
				float spec = pow(Clamp(Dot(rLight, -ray.Normalized())), 15.0f);

				auto dot = Dot(N, -Light);
				dot = max(dot + spec, 0.25f);

				Col difCol(255, 0, 0);
				Col speCol(255, 255, 255);
				Col ambCol(32, 32, 32);

				difCol *= dot;
				speCol *= spec;

				Col col = difCol + speCol;
				col = col.Max(ambCol);

				//Color *= Clamp(dot);
				DrawPixel(x, y, GetColor(col.r, col.g, col.b));
			}
			else
			{
				if (Dot(ray, PlaneVec) > 0)
				{
					//視点座標と法線ベクトルの内積をとる
					//これが平面から視点の距離、ここからオフセット値を引く。
					//逆視線ベクトルと法線ベクトルの内積をとる　これが1回あたりどれくらい平面に近づくかを表す
					//t = 1 / 2
					//tをもとに座標を求めるeye+ray*t
					//交点を元に色分けを行う。
					Plane pla(Position3(0, 1, 0), 50);
					auto a = Dot(ray, pla.normal) - pla.offset;
					auto b = Dot(-ray, pla.normal);
					auto t = a / b;
					auto as = eye + ray * t;


					if (((((int)(as.x) / 10) + (int)(as.z) / 10) % 2) == 0)
					{
						DrawPixel(x, y, GetColor(255, 255, 255));
					}
				}
			}
		}
	}
}

void DrawPixelWithFloat(int x, int y, float r, float g, float b)
{
	DrawPixel(x, y, GetColor(r * 0xff, g * 0xff, b * 0xff));
}

int main() {
	ChangeWindowMode(true);
	SetGraphMode(screen_width, screen_height, 32);
	SetMainWindowText(_T("1816007_尾崎　隼"));
	DxLib_Init();
	//DrawBox(0, 0, screen_width, screen_height, GetColor(255,255,255), true);
	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)));

	WaitKey();
	DxLib_End();
}