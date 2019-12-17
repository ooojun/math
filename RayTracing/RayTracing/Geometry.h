#pragma once
#include <math.h>
#include <DxLib.h>

//サイズを表す構造体
struct Size {
	float w;//幅
	float h;//高さ
};

//2D座標・ベクトルを表す構造体
struct Vector2 {
	Vector2() :x(0), y(0) {}
	Vector2(float inx, float iny) :x(inx), y(iny) {}
	float x, y;
	///ベクトルの大きさを返します
	float Magnitude()const;

	///正規化(大きさを１に)します
	void Normalize();

	///正規化ベクトルを返します
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

///内積を返す
float Dot(const Vector2& va, const Vector2& vb);

///外積を返す
float Cross(const Vector2& va, const Vector2& vb);

///内積演算子
float operator*(const Vector2& va, const Vector2& vb);

///外積演算子
float operator%(const Vector2& va, const Vector2& vb);

//とりあえず「座標」って意味だとベクタより
//Positionのほうがよくね？って理由でこの名前
typedef Vector2 Position2;

//3D座標・ベクトルを表す構造体
struct Vector3 {
	Vector3() :x(0), y(0), z(0) {}
	Vector3(float inx, float iny, float inz) :x(inx), y(iny), z(inz) {}
	float x, y, z;
	///ベクトルの大きさを返します
	float Magnitude()const;

	///正規化(大きさを１に)します
	void Normalize();

	///正規化ベクトルを返します
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

///内積を返す
float Dot(const Vector3& va, const Vector3& vb);

///外積を返す
Vector3 Cross(const Vector3& va, const Vector3& vb);

///内積演算子
float operator*(const Vector3& va, const Vector3& vb);

///外積演算子
Vector3 operator%(const Vector3& va, const Vector3& vb);
typedef Vector3 Position3;

struct Primitive;

enum class TEXTURE_TYPE {
	none,					//模様なし 
	hstripe,				//縞模様(横) 
	vstripe,				//縞模様(縦) 
	checkered,				//市松模様(チェッカーフラッグ) 
	textured				//テクスチャマッピング 
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
	Color color;					//ﾃﾞﾌｭｰｽﾞｶﾗｰ
	float specularity;				//ｽﾍﾟｷｭﾗの強さ(0)だったらｽﾍﾟｷｭﾗなし
	float reflectance;				//反射率 (0) : 無反射　(1)　:　全反射

	TEXTURE_TYPE texType;			//ﾃｸｽﾁｬｰﾊﾟﾀｰﾝ

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

///平面を表す構造体
struct Plane : Primitive {
	Vector3 normal;	//法線ベクトル(正規化済み)
	float offset;	//原点からのオフセット
	Plane() : normal(0, 0, 0), offset(0) {}
	Plane(const Vector3& innormal, float inoffset, Material & mat) :
		Primitive(mat),
		normal(innormal), offset(inoffset) {
		normal.Normalize();
	}
	bool IshitRay(const Position3 & eye, const Vector3 & ray, Vector3 & normalVec, Vector3 & hitPos) const final;
};

///円を表す構造体
struct Circle {
	float radius;//半径
	Position2 pos; //中心座標
	Circle() :radius(0), pos(0, 0) {}
	Circle(float r, Position2& p) :radius(r), pos(p) {}
};


///球を表す構造体
struct Sphere : Primitive {
	float radius;//半径
	Position3 pos; //中心座標
	Sphere() :radius(0), pos(0, 0, 0) {}
	Sphere(float r, Position3& p,Material & mat) :radius(r), pos(p), Primitive(mat){}
	bool IshitRay(const Position3 & eye, const Vector3 & ray, Vector3 & normalVec, Vector3 & hitPos) const final;
	Position3 GetPosition() const final { return pos; }
	void SetPosition(const Position3 & position) {  pos = position; }
};

///矩形を表す構造体
struct Rect {
	Position2 pos; //中心座標
	int w, h;//幅,高さ
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
	void Draw();					//自分の矩形を描画する
	void Draw(Vector2& offset);		//自分の矩形を描画する(オフセット付き)
};

float Clamp(float val, float minVal = 0.0f, float maxVal = 1.0f);