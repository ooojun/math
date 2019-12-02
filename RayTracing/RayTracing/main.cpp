#include <DxLib.h>
#include<math.h>
#include"Geometry.h"
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
bool IsHitRayAndObject(const Position3& eye, const Vector3& ray, const Sphere& sp, float &distance) {
	//���C�����K���ς݂ł���O��Łc
	//
	//���_���狅�̒��S�ւ̃x�N�g��(����)�����܂�
	//
	auto C = sp.pos - eye;
	//���S�x�N�g���Ǝ����̓��ς��Ƃ�܂������x�N�g�����@=�@�ˉe
	auto R = Dot(C, ray);

	auto vec = ray * R;

	auto Vec = C - vec;

	if (Vec.Magnitude() < sp.radius)
	{
		// �������Ă�����distance��Ԃ�
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

///���C�g���[�V���O
///@param eye ���_���W
///@param sphere ���I�u�W�F�N�g(���̂��������ɂ���)
float Clamp(float val, float minVal = 0.0f, float maxVal = 1.0f)
{
	return max(min(val, maxVal), minVal);
}

/// inVector ���Ѓx�N�g��
/// nVector�@���������ʂ̖@��x�N�g���i���K���ς݁j
/// return ���˃x�N�g��
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
	for (int y = 0; y < screen_height; ++y) {//�X�N���[���c����
		for (int x = 0; x < screen_width; ++x) {//�X�N���[��������
			//�@���_�ƃX�N���[�����W���王���x�N�g�������
			//������ray�����
			Vector3 pos = { static_cast<float>(x - screen_width / 2), static_cast<float>(y - screen_height / 2) , 0 };
			Vector3 ray = pos - eye;
			Vector3 PlaneVec(0, 1, 0);

			//�A���K��������
			ray.Normalize();
			//�BIsHitRay�֐���True��������h��Ԃ�
			//���h��Ԃ���DrawPixel�֐����g���B

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

				// ���˃x�N�g��
				auto rLight = ReflectVector(Light, N);

				// pow�֐� pow(���̐�,�搔)
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
					//���_���W�Ɩ@���x�N�g���̓��ς��Ƃ�
					//���ꂪ���ʂ��王�_�̋����A��������I�t�Z�b�g�l�������B
					//�t�����x�N�g���Ɩ@���x�N�g���̓��ς��Ƃ�@���ꂪ1�񂠂���ǂꂭ�炢���ʂɋ߂Â�����\��
					//t = 1 / 2
					//t�����Ƃɍ��W�����߂�eye+ray*t
					//��_�����ɐF�������s���B
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
	SetMainWindowText(_T("1816007_����@��"));
	DxLib_Init();
	//DrawBox(0, 0, screen_width, screen_height, GetColor(255,255,255), true);
	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)));

	WaitKey();
	DxLib_End();
}