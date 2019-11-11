#include<dxlib.h>
#include<math.h>
#include"Geometry.h"
const int screen_width = 640;	//横幅
const int screen_height = 480;	//縦幅

//ヒントになると思って、色々と関数を用意しておりますが
//別にこの関数を使わなければいけないわけでも、これに沿わなければいけないわけでも
//ありません。レイトレーシングができていれば構いません。

float Clamp(float value, const float minvalue = 0.0f, const float maxvalue = 1.0f)
{
	return min(max(value, minvalue), maxvalue);
}

//0～255よりも0～1で扱うほうがいい。
void DrawPixelWithFloat(int x, int y, float r, float g, float b) 
{
	DrawPixel(x, y, GetColor(r * 0xff, g * 0xff, b * 0xff));
}

///レイ(光線)と球体の当たり判定
///@param ray (視点からスクリーンピクセルへのベクトル)
///@param sphere 球
///@hint レイは正規化しといたほうが使いやすいだろう
bool IsHitRayAndObject(const Position3& eye,const Vector3& ray,const Sphere& sp,float &distance,Vector3&normal) {
	//レイが正規化済みである前提で…
	//視点から球体中心へのベクトル(視線)を作ります
	Vector3 Rray = sp.pos - eye;
	//
	auto PLen = Dot(Rray,ray);			//射影

	//中心から視線への内積をとります＝＞ベクトル長
	//視線ベクトルとベクトル長をかけて、中心からの垂線下した点を求めます

	auto d = ray * PLen;				//垂線

	auto aaa = Rray - d;				//垂線の点

	float len = aaa.Magnitude();		//垂線長

	auto W = sqrt(pow(sp.radius, 2) - pow(len, 2));

	normal = (ray * (PLen - W) - Rray);

	//三平方の定理を使ってどれくらい戻すのか調べる。(W)

	//射影からWを引く。(distance)
	if (sp.radius >= len)
	{
		distance = PLen - W;
		return true;
	}
	else
	{
		return false;
	}
}

///レイトレーシング
///@param eye 視点座標
///@param sphere 球オブジェクト(そのうち複数にする)
void RayTracing(const Position3& eye,const Sphere& sphere) {

	float distance = 0.0f;
	Vector3 normal;
	float ray;
	Vector3 light = {1,1,-1};

	float brightness;

	for (float y = 0; y < screen_height; ++y) {//スクリーン縦方向
		for (float x = 0; x < screen_width; ++x) {//スクリーン横方向
			//①視点とスクリーン座標から視線ベクトルを作る
			Vector3 P = {x - screen_width / 2, y - screen_height / 2,0 };
			Vector3 Vray = P - eye;

			//②正規化しとく
			Vray.Normalize();
			//③IsHitRay関数がTrueだったら白く塗りつぶす
			if (IsHitRayAndObject(eye,Vray,sphere, distance, normal))
			{
				brightness = Dot(-light.Normalized(),normal.Normalized());

				unsigned char b = 255;

				auto rate = Clamp(brightness);

				b *= rate;

				DrawPixel(x, y, GetColor(b, b, b));
			}
		}
	}
}

int main() {
	ChangeWindowMode(true);
	SetGraphMode(screen_width, screen_height, 32);
	SetMainWindowText(_T("1816007_尾崎隼"));
	DxLib_Init();
	//DxLib::DrawBox(0, 0, screen_width, screen_height,0xff00ff,true);
	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)));
	WaitKey();
	DxLib_End();
}