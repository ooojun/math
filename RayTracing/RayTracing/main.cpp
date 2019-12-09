#include <DxLib.h>
#include<math.h>
#include"Geometry.h"
#include "main.h"
const int screen_width = 640;
const int screen_height = 480;

//�q���g�ɂȂ�Ǝv���āA�F�X�Ɗ֐���p�ӂ��Ă���܂���
//�ʂɂ��̊֐����g��Ȃ���΂����Ȃ��킯�ł��A����ɉ���Ȃ���΂����Ȃ��킯�ł�
//����܂���B���C�g���[�V���O���ł��Ă���΍\���܂���B

///���C(����)�Ƌ��̂̓����蔻��
///@param ray (���_����X�N���[���s�N�Z���ւ̃x�N�g��)
///@param sphere ��
///@param distance ��_�܂ł̋���
///@hint ���C�͐��K�����Ƃ����ق����g���₷�����낤
bool IsHitRayAndObject(const Position3& eye, const Vector3& ray, const Sphere& sp, Vector3& Normal,Vector3 &hitPos) {
	//���C�����K���ς݂ł���O��Łc
	//
	//���_���狅�̒��S�ւ̃x�N�g��(����)�����܂�
	//
	auto C = sp.pos - eye;

	//���S�x�N�g���Ǝ����̓��ς��Ƃ�܂������x�N�g�����@=�@�ˉe
	auto Plen = Dot(C, ray);			//������

	auto Pray = ray * Plen;			

	auto Pvec = C - Pray;

	if (Pvec.Magnitude() < sp.radius)
	{
		auto W = sqrt(pow(sp.radius, 2) - pow(Pvec.Magnitude(), 2));

		auto distance = Pray.Magnitude() - W;

		Normal = (ray * distance - (sp.pos - eye)).Normalized();

		hitPos = (ray * distance + eye);

		return true;
	}
	else
	{
		return false;
	}

	return false;
}

///���C�g���[�V���O
///@param eye ���_���W
///@param sphere ���I�u�W�F�N�g(���̂��������ɂ���)
float Clamp(float val, float minVal = 0.0f, float maxVal = 1.0f)
{
	return max(min(val, maxVal), minVal);
}

/// inVector ���˃x�N�g��
/// nVector�@���������ʂ̖@���x�N�g��
/// return ���˃x�N�g��
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
	unsigned int GetColor()const {
		return DxLib::GetColor(r, g, b);
	}
};

Color GetCheckerColorPosition(Vector3 &as)
{
	if (((((int)(as.x) / 30) + (int)(as.z) / 30) % 2) == 0)
	{
		return Color(255, 255, 255);
	}
	else
	{
		return Color(128, 128, 128);
	}
}

void RayTracing(const Position3& eye, const Sphere& sphere, const Plane& plane) {

	auto distancepl = Dot(eye, plane.normal) - plane.offset;

	for (int y = 0; y < screen_height; ++y) {//�X�N���[���c����
		for (int x = 0; x < screen_width; ++x) {//�X�N���[��������
			//�@���_�ƃX�N���[�����W���王���x�N�g�������
			//������ray�����
			Vector3 pos = { static_cast<float>(x - screen_width / 2), -static_cast<float>(y - screen_height / 2) , 0 };
			Vector3 ray = pos - eye;
			
			Vector3 normal;
			Vector3 hitPos;

			//�A���K��������
			ray.Normalize();
			//�BIsHitRay�֐���True��������h��Ԃ�
			//���h��Ԃ���DrawPixel�֐����g���B

			if (IsHitRayAndObject(eye, ray, sphere, normal, hitPos))
			{

				int Color = 255;
				auto Light = Vector3(1, -1, -1);
				Light.Normalize();

				// ���˃x�N�g��
				auto rLight = ReflectVector(Light, normal);

				// pow�֐� pow(���̐�,�搔)
				float spec = pow(Clamp(Dot(rLight, -ray.Normalized())), 15.0f);

				auto deff = Dot(normal, rLight);
				deff = max(deff + spec, 0.25f);

				auto difCol = Color::Color(100, 255, 100);
				auto speCol = Color::Color(255, 255, 255);
				auto ambCol = Color::Color(32, 32, 32);
				auto col = Color::Color();

				difCol *= deff;
				speCol *= spec;

				auto tmpRay = ReflectVector(ray, normal);	//���˃x�N�g��

				auto t = (Dot(hitPos, plane.normal) - plane.offset) / Dot(-tmpRay, plane.normal);

				auto Tpos = (hitPos + tmpRay * t);

				auto h = (hitPos.y - plane.offset) / 100;

				col = difCol + speCol;
				col = col.Max(ambCol);

				if (Dot(ray, plane.normal) > 0)
				{
						//��_���W�����߂�
						auto color = GetCheckerColorPosition(Tpos);

						//DrawPixel(x, y, GetColor(h * 255, h * 255, h * 255));

						DrawPixel(x, y, color.GetColor());
						continue;
				}
	
				DrawPixel(x, y, GetColor(col.r, col.g, col.b));
			}
			else
			{
				if (Dot(ray, plane.normal) > 0)
				{
					auto rayplane = Dot(-ray, plane.normal);

					auto t = distancepl / rayplane;

					auto intercepl = eye + ray * t;

					DrawPixel(x, y, GetCheckerColorPosition(intercepl).GetColor());

					if (/*���̂Ƃ̓����蔻�肪��������e�̕`��ɓ���*/true)
					{

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
	SetMainWindowText(_T("1816007_����@��"));
	DxLib_Init();
	DrawBox(0, 0, screen_width, screen_height, GetColor(200,200,255), true);
	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)),Plane(Position3(0, -1, 0), -100));
	WaitKey();
	DxLib_End();
}