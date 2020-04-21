#pragma once

class Camera
{
private:
	struct VectorType 
	{ 
		float x, y, z;
	};

public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void setPosition(float, float, float);
	void setRotation(float, float, float);

	void render();
	void getViewMatrix(float*);

private:
	static void matrixRotationYawPitchRoll(float*, float, float, float);
	static void transformCoord(VectorType&, float*);
	void buildViewMatrix(VectorType, VectorType, VectorType);

	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_viewMatrix[16];
};
