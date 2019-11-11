#include<dxlib.h>
#include<math.h>
#include"Geometry.h"
const int screen_width = 640;	//����
const int screen_height = 480;	//�c��

//�q���g�ɂȂ�Ǝv���āA�F�X�Ɗ֐���p�ӂ��Ă���܂���
//�ʂɂ��̊֐����g��Ȃ���΂����Ȃ��킯�ł��A����ɉ���Ȃ���΂����Ȃ��킯�ł�
//����܂���B���C�g���[�V���O���ł��Ă���΍\���܂���B

float Clamp(float value, const float minvalue = 0.0f, const float maxvalue = 1.0f)
{
	return min(max(value, minvalue), maxvalue);
}

//0�`255����0�`1�ň����ق��������B
void DrawPixelWithFloat(int x, int y, float r, float g, float b) 
{
	DrawPixel(x, y, GetColor(r * 0xff, g * 0xff, b * 0xff));
}

///���C(����)�Ƌ��̂̓����蔻��
///@param ray (���_����X�N���[���s�N�Z���ւ̃x�N�g��)
///@param sphere ��
///@hint ���C�͐��K�����Ƃ����ق����g���₷�����낤
bool IsHitRayAndObject(const Position3& eye,const Vector3& ray,const Sphere& sp,float &distance,Vector3&normal) {
	//���C�����K���ς݂ł���O��Łc
	//���_���狅�̒��S�ւ̃x�N�g��(����)�����܂�
	Vector3 Rray = sp.pos - eye;
	//
	auto PLen = Dot(Rray,ray);			//�ˉe

	//���S���王���ւ̓��ς��Ƃ�܂������x�N�g����
	//�����x�N�g���ƃx�N�g�����������āA���S����̐����������_�����߂܂�

	auto d = ray * PLen;				//����

	auto aaa = Rray - d;				//�����̓_

	float len = aaa.Magnitude();		//������

	auto W = sqrt(pow(sp.radius, 2) - pow(len, 2));

	normal = (ray * (PLen - W) - Rray);

	//�O�����̒藝���g���Ăǂꂭ�炢�߂��̂����ׂ�B(W)

	//�ˉe����W�������B(distance)
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

///���C�g���[�V���O
///@param eye ���_���W
///@param sphere ���I�u�W�F�N�g(���̂��������ɂ���)
void RayTracing(const Position3& eye,const Sphere& sphere) {

	float distance = 0.0f;
	Vector3 normal;
	float ray;
	Vector3 light = {1,1,-1};

	float brightness;

	for (float y = 0; y < screen_height; ++y) {//�X�N���[���c����
		for (float x = 0; x < screen_width; ++x) {//�X�N���[��������
			//�@���_�ƃX�N���[�����W���王���x�N�g�������
			Vector3 P = {x - screen_width / 2, y - screen_height / 2,0 };
			Vector3 Vray = P - eye;

			//�A���K�����Ƃ�
			Vray.Normalize();
			//�BIsHitRay�֐���True�������甒���h��Ԃ�
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
	SetMainWindowText(_T("1816007_���蔹"));
	DxLib_Init();
	//DxLib::DrawBox(0, 0, screen_width, screen_height,0xff00ff,true);
	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)));
	WaitKey();
	DxLib_End();
}