#pragma once

class Light
{
public:
	Light();
	Light(const Light&);
	~Light();
	void setDiffuseColor(float, float, float, float);
	void setDirection(float, float, float);
	void setAmbientLight(float, float, float, float);
	void getDiffuseColor(float*);
	void getDirection(float*);
	void getAmbientLight(float*);

private:
	float diffuseColor[4] = {};
	float direction[3] = {};
	float ambientLight[4] = {};
};
