#include"Geometry.h"
#include<DxLib.h>
#include<cmath>


void
Rect::Draw() {
	DxLib::DrawBox(Left()*2, Top()*2, Right()*2, Bottom()*2, 0xffffffff, false);
}

void
Rect::Draw(Vector2& offset) {
	DxLib::DrawBox((Left()+offset.x)*2, (Top()+offset.y)*2, (Right()+offset.x)*2, (Bottom()+offset.y)*2, 0xffffffff, false);
}

void
Vector2::operator*=(float scale) {
	x *= scale;
	y *= scale;
}

Vector2
Vector2::operator*(float scale) {
	return Vector2(x*scale, y*scale);
}

Vector2 operator+(const Vector2& va, const Vector2 vb) {
	return Vector2(va.x + vb.x, va.y + vb.y);
}

Vector2 operator-(const Vector2& va, const Vector2 vb){
	return Vector2(va.x - vb.x, va.y - vb.y);
}

float
Vector2::Magnitude()const {
	return hypot(x, y);
}


void 
Vector2::Normalize() {
	float mag = Magnitude();
	x /= mag;
	y /= mag;
}


Vector2
Vector2::Normalized() {
	float mag = Magnitude();
	return Vector2(x / mag,	y /mag);
}


///内積を返す
float
Dot(const Vector2& va, const Vector2& vb) {
	return va.x*vb.x + va.y*vb.y;
}

///外積を返す
float
Cross(const Vector2& va, const Vector2& vb) {
	return va.x*vb.y - vb.x*va.y;
}

///内積演算子
float 
operator*(const Vector2& va, const Vector2& vb) {
	return Dot(va, vb);
}

///外積演算子
float 
operator%(const Vector2& va, const Vector2& vb) {
	return Cross(va, vb);
}


void 
Vector2::operator+=(const Vector2& v) {
	x += v.x;
	y += v.y;
}
void 
Vector2::operator-=(const Vector2& v) {
	x -= v.x;
	y -= v.y;
}


//-------ここから3Dのターン------
void
Vector3::operator*=(float scale) {
	x *= scale;
	y *= scale;
	z *= scale;
}

Vector3
Vector3::operator*(float scale)const {
	return Vector3(x*scale, y*scale,z*scale);
}

Vector3 operator+(const Vector3& va, const Vector3 vb) {
	return Vector3(va.x + vb.x, va.y + vb.y,va.z+vb.z);
}

Vector3 operator-(const Vector3& va, const Vector3 vb) {
	return Vector3(va.x - vb.x, va.y - vb.y,va.z-vb.z);
}

float
Vector3::Magnitude()const {
	return sqrt(x*x+y*y+z*z);
}

void
Vector3::Normalize() {
	float mag = Magnitude();
	x /= mag;
	y /= mag;
	z /= mag;
}

Vector3
Vector3::Normalized() {
	float mag = Magnitude();
	return Vector3(x / mag, y / mag,z/mag);
}

///内積を返す
float
Dot(const Vector3& va, const Vector3& vb) {
	return va.x*vb.x + va.y*vb.y+va.z*vb.z;
}

///外積を返す
Vector3
Cross(const Vector3& va, const Vector3& vb) {
	return Vector3(va.z*vb.y - va.y*vb.z, va.z*vb.x - va.x*vb.z, va.x*vb.y - vb.x*va.y);
}

///内積演算子
float
operator*(const Vector3& va, const Vector3& vb) {
	return Dot(va, vb);
}

///外積演算子
Vector3
operator%(const Vector3& va, const Vector3& vb) {
	return Cross(va, vb);
}

float Clamp(float val, float minVal, float maxVal)
{
	return max(min(val, maxVal), minVal);
}


void
Vector3::operator+=(const Vector3& v) {
	x += v.x;
	y += v.y;
	z += v.z;
}
void
Vector3::operator-=(const Vector3& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

void Color::operator*=(const float scale)
{
	r = Clamp(r * scale, 0, 255);
	g = Clamp(g * scale, 0, 255);
	b = Clamp(b * scale, 0, 255);
}

void Color::operator+=(const Color & incol)
{
	r = Clamp(r + incol.r, 0, 255);
	g = Clamp(g + incol.g, 0, 255);
	b = Clamp(b + incol.b, 0, 255);
}

Color Color::operator+(const Color & incol)
{
	Color ret(r, g, b);
	ret.r = Clamp(ret.r + incol.r, 0, 255);
	ret.g = Clamp(ret.g + incol.g, 0, 255);
	ret.b = Clamp(ret.b + incol.b, 0, 255);
	return ret;
}

Color Color::operator*(float scale) const
{
	return Color(r * scale, g * scale, b * scale);
}

Color Color::Max(const Color & col)
{
	Color ret(r, g, b);
	ret.r = max(col.r, ret.r);
	ret.g = max(col.g, ret.g);
	ret.b = max(col.b, ret.b);
	return ret;
}

Color Color::Min(const Color & col)
{
	Color ret(r, g, b);
	ret.r = min(col.r, ret.r);
	ret.g = min(col.g, ret.g);
	ret.b = min(col.b, ret.b);
	return ret;
}

Color Color::Normalization() const
{
	Color ret;

	ret.r = r / 255.0f;
	ret.g = g / 255.0f;
	ret.b = b / 255.0f;

	return ret;
}

unsigned int Color::GetCol() const
{
	return DxLib::GetColor(r, g, b);
}

Color GetCheckerColorPosition(Vector3 &as, Color baseColor)
{
	if (((((int)(as.x) / 30) + (int)(as.z) / 30) % 2) == 0)
	{
		if ((int)(as.x) > 0)
		{
			return baseColor;
		}
		else
		{
			return baseColor * 0.5;
		}
	}
	else
	{
		if ((int)(as.x) > 0)
		{
			return baseColor * 0.5;
		}
		else
		{
			return baseColor;
		}
	}
}

Color operator*(const Color rcolor, const Color lcolor)
{
	return Color(rcolor.r * (lcolor.r / 255.0f), rcolor.g * (lcolor.g / 255.0f), rcolor.b * (lcolor.b / 255.0f));
}


bool Plane::IshitRay(const Position3 & eye, const Vector3 & ray, Vector3 & normalVec,Vector3 & hitPos) const
{
	auto distancepl = Dot(eye, normal) - offset;

	auto Ray = ray;

	auto rayplane = Dot(-Ray, normal);

	if (Dot(-Ray, normal) > 0)
	{
		auto distance = distancepl / rayplane;

		hitPos = ray * distance + eye;

		return true;
	}
	else
	{
		return false;
	}
	return false;
}


bool Sphere::IshitRay(const Position3 & eye, const Vector3 & ray, Vector3 & normalVec, Vector3 & hitPos) const
{
	auto sphereToRay = pos - eye;

	auto perLen = Dot(sphereToRay, ray);

	auto perRay = ray * perLen;

	auto perVec = sphereToRay - perRay;

	if (perVec.Magnitude() < radius)
	{
		auto distanceToHitPos = sqrt(pow(radius, 2) - pow(perVec.Magnitude(), 2));
		auto distance = perRay.Magnitude() - distanceToHitPos;
		normalVec = (ray * distance - sphereToRay).Normalized();
		hitPos = ray * distance + eye;
		return true;
	}
	else
	{
		return false;
	}
	return false;
}