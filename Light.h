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
	float m_diffuseColor[4];
	float m_direction[3];
	float m_ambientLight[4];
};
