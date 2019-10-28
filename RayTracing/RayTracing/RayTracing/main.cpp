#include<dxlib.h>
#include<math.h>
#include"Geometry.h"
const int screen_width = 640;	//����
const int screen_height = 480;	//�c��

//�q���g�ɂȂ�Ǝv���āA�F�X�Ɗ֐���p�ӂ��Ă���܂���
//�ʂɂ��̊֐����g��Ȃ���΂����Ȃ��킯�ł��A����ɉ���Ȃ���΂����Ȃ��킯�ł�
//����܂���B���C�g���[�V���O���ł��Ă���΍\���܂���B

float Clamp(float in, const float min = 0.0f, const float max = 1.0f)
{
	return max(min(in, max), min);
}

///���C(����)�Ƌ��̂̓����蔻��
///@param ray (���_����X�N���[���s�N�Z���ւ̃x�N�g��)
///@param sphere ��
///@hint ���C�͐��K�����Ƃ����ق����g���₷�����낤
bool IsHitRayAndObject(const Position3& eye,const Vector3& ray,const Sphere& sp,float &distance) {
	//���C�����K���ς݂ł���O��Łc
	//���_���狅�̒��S�ւ̃x�N�g��(����)�����܂�
	Vector3 Vray = sp.pos - eye;
	//
	auto PLen = Dot(Vray,ray);

	//���S���王���ւ̓��ς��Ƃ�܂������x�N�g����
	//�����x�N�g���ƃx�N�g�����������āA���S����̐����������_�����߂܂�

	auto d = ray * PLen;				//����

	auto aaa = Vray - d;				//�����̓_

	float len = aaa.Magnitude();		//������

	//�O�����̒藝���g���Ăǂꂭ�炢�߂��̂����ׂ�B(W)

	//�ˉe����W�������B(distance)

	if (sp.radius >= len)
	{
		auto W = PLen - sqrt(pow(sp.radius, 2) - pow(len, 2));
		distance = W;
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

	for (float y = 0; y < screen_height; ++y) {//�X�N���[���c����
		for (float x = 0; x < screen_width; ++x) {//�X�N���[��������
			//�@���_�ƃX�N���[�����W���王���x�N�g�������
			Vector3 P = {x - screen_width / 2, y - screen_height / 2,0 };
			Vector3 Vray = P - eye;

			//�A���K�����Ƃ�
			Vray.Normalize();
			//�BIsHitRay�֐���True�������甒���h��Ԃ�
			if (IsHitRayAndObject(eye,Vray,sphere, distance))
			{
				float b = 255;
				auto rate = ((400.0f - (float)distance) / 100.0f);
				b *= rate;
				DrawPixel(x, y, GetColor(b, b, b));
			}
			//���h��Ԃ���DrawPixel�Ƃ����֐����g���B

		}
	}
}

int main() {
	ChangeWindowMode(true);
	SetGraphMode(screen_width, screen_height, 32);
	SetMainWindowText(_T("1816007_���蔹"));
	DxLib_Init();

	RayTracing(Vector3(0, 0, 300), Sphere(100, Position3(0, 0, -100)));

	WaitKey();
	DxLib_End();
}