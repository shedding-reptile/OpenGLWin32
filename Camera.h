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
	~Camera();

	void setPosition(float, float, float);
	void setRotation(float, float, float);
	void render();
	void getViewMatrix(float*) const;

private:
	static void matrixRotationYawPitchRoll(float*, float, float, float);
	static void transformCoord(VectorType&, float*);
	void buildViewMatrix(VectorType, VectorType, VectorType);

	float posX;
	float posY;
	float posZ;
	float rotX;
	float rotY;
	float rotZ;
	float viewMatrix[16];
};
