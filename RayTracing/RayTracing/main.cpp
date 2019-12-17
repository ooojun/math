#include <DxLib.h>
#include <math.h>
#include "Geometry.h"
#include "main.h"
#include <memory>
#include <string>
#include <vector>

//primitive�ɑ΂��Ė��O��t���ꏏ�ɕۑ�����
using PrimPair = std::pair<Primitive, std::string>;

//���vector
using PrimPairVec = std::vector<PrimPair>;

using namespace std;

const int screen_width = 640;
const int screen_height = 480;

float RecCnt = 0;

void DrawPixelWithFloat(int x, int y, float r, float g, float b) 
{
	DrawPixel(x, y, GetColor(r * 0xff, g * 0xff, b * 0xff));
}

// inVector ���˃x�N�g��
// nVector�@���������ʂ̖@���x�N�g��
// return ���˃x�N�g��
Vector3 ReflectVector(const Vector3& inVector, const Vector3& nVector)
{
	return inVector - nVector * 2 * (inVector * nVector);
}

//�\�ʍގ��Ɩ@���ƌ�����P���ɂ��̃I�u�W�F�N�g�ɂ����邻�̓_�̐F��Ԃ� 
//@param object �I�u�W�F�N�g(�}�e���A���������Ă����) 
//@param ray ���˃x�N�g�� 
//@param hitpos �Փ˓_ 
//@param normal �Փ˓_�ɂ����邻�̕��̖̂@���x�N�g��(���K���ς�) 
//@param toLight ���C�g�����x�N�g�� 
Color GetBasicColor(const Primitive* prim, const Vector3& ray, const Position3&hitpos, const Vector3& normal, const Vector3& light)
{
	// �����Ƌ��̖@���x�N�g���Ƃ̔��˃x�N�g��
	auto refLight = ReflectVector(light, normal);
	//�Ƃ�Ԃ����Ȃ����ׂ�Clamp
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

Color RecursiveTrace(Vector3 & light,const Position3& eye,Vector3 & ray, const vector<Primitive *> & prims, Primitive * self, unsigned int limit)
{
	Color retCol = Color(200, 200, 255);

	Position3 H;

	Vector3 N;

	for (Primitive* prim : prims) {
		if (prim != self)
		{
			if (prim->IshitRay(eye, ray, N, H)) {
				if (prim->material.reflectance > 0.1f) {

					auto col = GetBasicColor(prim, ray, H, N, light);

					auto tmpRay = ReflectVector(ray, N);	//���˃x�N�g��

					RecCnt++;

					if (RecCnt < limit)		//���˂���Ȃ甽�˃x�N�g���ƏՓ˓_�����ɍċA����(�ċA���E���l������) 
					{
						self = prim;
						RecursiveTrace(light, eye, ray, prims, self, limit);
					}
					if (prims[1]->IshitRay(H, tmpRay, N, H))
					{
						auto color = GetCheckerColorPosition(H, prim->material.color);

						return Color(col.r * color.Normalization().r, col.g * color.Normalization().g, col.b * color.Normalization().b);
					}
					else
					{
						return col;
					}
					RecCnt = 0;
				}
				else//���˂��Ȃ� 
				{
					//���̂̍ގ������̂܂܍̗p(�F�𓾂�)���� 			 
					if (prim->IshitRay(eye, ray, N, H))
					{
						auto col = GetCheckerColorPosition(H, prims[1]->material.color);
						return col;

						//�e�̎���
						if (prim->IshitRay(H, -light, N, N))//���������𒲂ׂĕ��̂̂ǂꂩ�Ɠ������Ă�����Â�����					
						{
							col *= 0.5;
							return col;
						}
					}
				}

			}
		}
	}
	return retCol;
}

void RayTracing(Vector3 light, const Position3& eye, std::vector<Primitive*>& prims)
{
	for (int y = 0; y < screen_height; ++y) {//�X�N���[���c����
		for (int x = 0; x < screen_width; ++x) {//�X�N���[��������

			//�@���_�ƃX�N���[�����W���王���x�N�g�������
			//������ray�����
			Vector3 pos = { static_cast<float>(x - screen_width / 2),static_cast<float>(screen_height / 2 - y),0 };
			Vector3 ray = pos - eye;

			//�A���K��������
			ray.Normalize();

			auto distance = 0.0f;

			light.Normalize();

			auto c = RecursiveTrace(light,eye, ray, prims, nullptr, 5);
			DrawPixel(x, y, GetColor(c.r, c.g, c.b));
		}
	}
}

int main() {
	ChangeWindowMode(true);
	SetGraphMode(screen_width, screen_height, 32);
	SetMainWindowText(_T("1816007_����@��"));
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
		RayTracing(Light,Vector3(0, 0, 300), primitives);
		ScreenFlip();
	}
	WaitKey();
	DxLib_End();
}
