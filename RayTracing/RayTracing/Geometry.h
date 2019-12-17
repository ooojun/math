#pragma once
#include <math.h>
#include <DxLib.h>

//�T�C�Y��\���\����
struct Size {
	float w;//��
	float h;//����
};

//2D���W�E�x�N�g����\���\����
struct Vector2 {
	Vector2() :x(0), y(0) {}
	Vector2(float inx, float iny) :x(inx), y(iny) {}
	float x, y;
	///�x�N�g���̑傫����Ԃ��܂�
	float Magnitude()const;

	///���K��(�傫�����P��)���܂�
	void Normalize();

	///���K���x�N�g����Ԃ��܂�
	Vector2 Normalized();

	void operator+=(const Vector2& v);
	void operator-=(const Vector2& v);
	void operator*=(float scale);
	Vector2 operator*(float scale);
	Vector2 operator-() {
		return Vector2(-x, -y);
	}
};

Vector2 operator+(const Vector2& va, const Vector2 vb);
Vector2 operator-(const Vector2& va, const Vector2 vb);

///���ς�Ԃ�
float Dot(const Vector2& va, const Vector2& vb);

///�O�ς�Ԃ�
float Cross(const Vector2& va, const Vector2& vb);

///���ω��Z�q
float operator*(const Vector2& va, const Vector2& vb);

///�O�ω��Z�q
float operator%(const Vector2& va, const Vector2& vb);

//�Ƃ肠�����u���W�v���ĈӖ����ƃx�N�^���
//Position�̂ق����悭�ˁH���ė��R�ł��̖��O
typedef Vector2 Position2;

//3D���W�E�x�N�g����\���\����
struct Vector3 {
	Vector3() :x(0), y(0), z(0) {}
	Vector3(float inx, float iny, float inz) :x(inx), y(iny), z(inz) {}
	float x, y, z;
	///�x�N�g���̑傫����Ԃ��܂�
	float Magnitude()const;

	///���K��(�傫�����P��)���܂�
	void Normalize();

	///���K���x�N�g����Ԃ��܂�
	Vector3 Normalized();

	void operator+=(const Vector3& v);
	void operator-=(const Vector3& v);
	void operator*=(float scale);
	Vector3 operator*(float scale)const;
	Vector3 operator-()const {
		return Vector3(-x, -y, -z);
	}
};
Vector3 operator+(const Vector3& va, const Vector3 vb);
Vector3 operator-(const Vector3& va, const Vector3 vb);

///���ς�Ԃ�
float Dot(const Vector3& va, const Vector3& vb);

///�O�ς�Ԃ�
Vector3 Cross(const Vector3& va, const Vector3& vb);

///���ω��Z�q
float operator*(const Vector3& va, const Vector3& vb);

///�O�ω��Z�q
Vector3 operator%(const Vector3& va, const Vector3& vb);
typedef Vector3 Position3;

struct Primitive;

enum class TEXTURE_TYPE {
	none,					//�͗l�Ȃ� 
	hstripe,				//�Ȗ͗l(��) 
	vstripe,				//�Ȗ͗l(�c) 
	checkered,				//�s���͗l(�`�F�b�J�[�t���b�O) 
	textured				//�e�N�X�`���}�b�s���O 
};

struct Color
{
	unsigned char r, g, b;
	Color() :r(0), g(0), b(0) {};
	Color(unsigned char inr, unsigned char ing, unsigned char inb) :r(inr), g(ing), b(inb) {}
	void operator*=(const float scale);
	void operator+=(const Color & incol);
	Color operator+(const Color & incol);
	Color operator*(float scale)const;
	Color Max(const Color& col);
	Color Min(const Color& col);
	Color Normalization()const;

	unsigned int GetCol()const;
};

Color operator*(Color rcolor, Color lcolor);
Color GetCheckerColorPosition(Vector3 &as, Color baseColor);

struct Material
{
	Color color;					//��̭��޶װ
	float specularity;				//��߷�ׂ̋���(0)���������߷�ׂȂ�
	float reflectance;				//���˗� (0) : �����ˁ@(1)�@:�@�S����

	TEXTURE_TYPE texType;			//ø���������

	Material() :color(0, 0, 0), specularity(0), reflectance(0), texType(TEXTURE_TYPE::none) {}
	Material(Color & inCol, float inSpe, float inRef, TEXTURE_TYPE type) :color(inCol), specularity(inSpe), reflectance(inRef), texType(type) {}
};

struct Primitive {

	Material material;

	Primitive() : material(Color(0,0,0),0.0f,0.0f,TEXTURE_TYPE::none){}

	Primitive(Material & mat) :material(mat){}

	virtual bool IshitRay(const Position3 & eye, const Vector3 & ray,Vector3 & normalVec, Vector3 & hitPos) const = 0;
	virtual Position3 GetPosition() const { return Position3(); }
	virtual void SetPosition(const Position3 & pos) {}
	virtual const Material& GetMaterial()const { return material; }
};

///���ʂ�\���\����
struct Plane : Primitive {
	Vector3 normal;	//�@���x�N�g��(���K���ς�)
	float offset;	//���_����̃I�t�Z�b�g
	Plane() : normal(0, 0, 0), offset(0) {}
	Plane(const Vector3& innormal, float inoffset, Material & mat) :
		Primitive(mat),
		normal(innormal), offset(inoffset) {
		normal.Normalize();
	}
	bool IshitRay(const Position3 & eye, const Vector3 & ray, Vector3 & normalVec, Vector3 & hitPos) const final;
};

///�~��\���\����
struct Circle {
	float radius;//���a
	Position2 pos; //���S���W
	Circle() :radius(0), pos(0, 0) {}
	Circle(float r, Position2& p) :radius(r), pos(p) {}
};


///����\���\����
struct Sphere : Primitive {
	float radius;//���a
	Position3 pos; //���S���W
	Sphere() :radius(0), pos(0, 0, 0) {}
	Sphere(float r, Position3& p,Material & mat) :radius(r), pos(p), Primitive(mat){}
	bool IshitRay(const Position3 & eye, const Vector3 & ray, Vector3 & normalVec, Vector3 & hitPos) const final;
	Position3 GetPosition() const final { return pos; }
	void SetPosition(const Position3 & position) {  pos = position; }
};

///��`��\���\����
struct Rect {
	Position2 pos; //���S���W
	int w, h;//��,����
	Rect() : pos(0, 0), w(0), h(0) {}
	Rect(float x, float y, int inw, int inh) :
		pos(x, y), w(inw), h(inh) {}
	Rect(Position2& inpos, int inw, int inh) :
		pos(inpos), w(inw), h(inh)
	{}
	void SetCenter(float x, float y) {
		pos.x = x;
		pos.y = y;
	}
	void SetCenter(const Position2& inpos) {
		pos.x = inpos.x;
		pos.y = inpos.y;
	}
	Vector2 Center() {
		return pos;
	}
	float Left() { return pos.x - w / 2; }
	float Top() { return pos.y - h / 2; }
	float Right() { return pos.x + w / 2; }
	float Bottom() { return pos.y + h / 2; }
	void Draw();					//�����̋�`��`�悷��
	void Draw(Vector2& offset);		//�����̋�`��`�悷��(�I�t�Z�b�g�t��)
};

float Clamp(float val, float minVal = 0.0f, float maxVal = 1.0f);