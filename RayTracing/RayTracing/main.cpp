#include <DxLib.h>
#include <math.h>
#include "Geometry.h"
#include "main.h"
#include <memory>
#include <string>
#include <vector>

//primitiveに対して名前を付け一緒に保存する
using PrimPair = std::pair<Primitive, std::string>;

//上のvector
using PrimPairVec = std::vector<PrimPair>;

using namespace std;

const int screen_width = 640;
const int screen_height = 480;

void DrawPixelWithFloat(int x, int y, float r, float g, float b) 
{
	DrawPixel(x, y, GetColor(r * 0xff, g * 0xff, b * 0xff));
}

// inVector 入射ベクトル
// nVector　当たった面の法線ベクトル
// return 反射ベクトル
Vector3 ReflectVector(const Vector3& inVector, const Vector3& nVector)
{
	return inVector - nVector * 2 * (inVector * nVector);
}

//表面材質と法線と光から単純にそのオブジェクトにおけるその点の色を返す 
//@param object オブジェクト(マテリアルも入っているよ) 
//@param ray 入射ベクトル 
//@param hitpos 衝突点 
//@param normal 衝突点におけるその物体の法線ベクトル(正規化済み) 
//@param toLight ライト方向ベクトル 
Color GetBasicColor(const Primitive* prim, const Vector3& ray, const Position3&hitpos, const Vector3& normal, const Vector3& light)
{
	// 光線と球の法線ベクトルとの反射ベクトル
	auto refLight = ReflectVector(light, normal);
	//照り返しをなくす為のClamp
	float spec = pow(Clamp(Dot(refLight, -ray)), 15.0f);

	auto deff = Dot(normal, -light);
	deff = max(deff + spec, 0.25f);

	auto difCol = prim->material.color;
	auto speCol = Color::Color(255, 255, 255);
	auto ambCol = Color::Color(32, 32, 32);
	auto col = Color::Color();

	difCol *= deff;
	speCol *= spec;

	col = difCol + speCol;
	col = col.Max(ambCol);

	return col;
}

//Color RecursiveTrace(Vector3 & light, const Position3& eye, const Vector3 & ray, const vector<Primitive *> & prims, Primitive * self, unsigned int limit)
//{
//	Color retCol;
//
//	Position3 H;
//
//	Vector3 N;
//
//	float RecCnt;
//
//	for (Primitive* prim : prims) {
//		if (prim->IshitRay(eye, ray,normal, hitpos)) {
//			if (prim->material.reflectance > 0.1f) {
//
//				RecCnt++;
//
//				if (RecCnt < limit)		//反射するなら反射ベクトルと衝突点を元に 
//				{
//					//再帰する(再帰限界を考慮して) 
//					RecursiveTrace(light, eye, ray, prims, self, limit);
//				}
//				else
//				{
//				}
//			}
//			else {
//				//反射しない 
//				//物体の材質をそのまま採用(色を得る)する 
//				return GetBasicColor(prim, ray, hitpos, normal, light);
//				//光源方向を調べて物体のどれかと当たっていたら暗くする 
//			}
//		}
//	}
//	return retCol;
//}

Color RecursiveTrace(Vector3 & light, const Position3& eye, const Vector3 & ray, const vector<Primitive *> & prims, Primitive * self, unsigned int limit)
{
	Color retCol;

	Position3 H;

	Vector3 N;

	float RecCnt;

	for (Primitive* prim : prims) {
		if (prim->IshitRay(eye, ray, N, H))
		{
			auto col = GetBasicColor(prims[0], ray, H, N, light);

			auto tmpRay = ReflectVector(ray, N);	//反射ベクトル

			if (prims[1]->IshitRay(H, tmpRay, N, H))
			{
				//交点座標を求める
				auto color = GetCheckerColorPosition(H, prims[1]->material.color);

				return Color(col.r * color.Normalization().r, col.g * color.Normalization().g, col.b * color.Normalization().b);
				continue;
			}
			return col;
		}
		else
		{
			if (prims[1]->IshitRay(eye, ray, N, H))
			{
				auto col = GetCheckerColorPosition(H, prims[1]->material.color);
				return col;

				//影の実装
				if (prims[0]->IshitRay(H, -light, N, N))
				{
					col *= 0.5;
					return col;
				}
			}
		}
	}
	return retCol;
}

void RayTracing(Vector3 light, const Position3& eye, std::vector<Primitive*>& prims)
{
	for (int y = 0; y < screen_height; ++y) {//スクリーン縦方向
		for (int x = 0; x < screen_width; ++x) {//スクリーン横方向

			//①視点とスクリーン座標から視線ベクトルを作る
			//ここでrayを作る
			Vector3 pos = { static_cast<float>(x - screen_width / 2),static_cast<float>(screen_height / 2 - y),0 };
			Vector3 ray = pos - eye;

			//②正規化をする
			ray.Normalize();

			auto distance = 0.0f;

			light.Normalize();

			auto c = RecursiveTrace(light, eye, ray, prims, nullptr, 5);
			DrawPixel(x, y, GetColor(c.r, c.g, c.b));
		}
	}
}

int main() {
	ChangeWindowMode(true);
	SetGraphMode(screen_width, screen_height, 32);
	SetMainWindowText(_T("1816007_尾崎　隼"));
	DxLib_Init();

	auto Light = Vector3(1, -1, -1);

	char keystate[256];
	vector <Primitive *> primitives;
	auto sp = new Sphere(100, Position3(0, 0, -100), Material(Color(255, 255, 0), 15.0f, 1.0f, TEXTURE_TYPE::none));
	primitives.push_back(sp);

	auto pl = new Plane(Position3(0, 1, 0), -100, Material(Color(192, 255, 255), 0.0f, 0.0f, TEXTURE_TYPE::checkered));
	primitives.push_back(pl);

	while (ProcessMessage() == 0) {
		GetHitKeyStateAll(keystate);
		ClearDrawScreen();

		if (keystate[KEY_INPUT_UP]) {
			primitives[0]->SetPosition(Position3(primitives[0]->GetPosition().x, primitives[0]->GetPosition().y + 2, primitives[0]->GetPosition().z));
		}
		if (keystate[KEY_INPUT_DOWN]) {
			primitives[0]->SetPosition(Position3(primitives[0]->GetPosition().x, primitives[0]->GetPosition().y - 2, primitives[0]->GetPosition().z));
		}
		if (keystate[KEY_INPUT_LEFT]) {
			primitives[0]->SetPosition(Position3(primitives[0]->GetPosition().x - 2, primitives[0]->GetPosition().y, primitives[0]->GetPosition().z));
		}
		if (keystate[KEY_INPUT_RIGHT]) {
			primitives[0]->SetPosition(Position3(primitives[0]->GetPosition().x + 2, primitives[0]->GetPosition().y, primitives[0]->GetPosition().z));
		}

		DrawBox(0, 0, screen_width, screen_height, GetColor(200, 200, 255), true);
		RayTracing(Light,Vector3(0, 0, 300), primitives);
		ScreenFlip();
	}
	WaitKey();
	DxLib_End();
}
