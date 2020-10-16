#include <cmath>
#include <cstring>
#include "Camera.h"

Camera::Camera(): viewMatrix{}
{
	posX = 0.0f;
	posY = 0.0f;
	posZ = 0.0f;

	rotX = 0.0f;
	rotY = 0.0f;
	rotZ = 0.0f;
}

Camera::Camera(const Camera& other): posX(0), posY(0), posZ(0), rotX(0),
                                                    rotY(0),
                                                    rotZ(0),
                                                    viewMatrix{}
{
}

Camera::~Camera()
{
}

void Camera::setPosition(float x, float y, float z)
{
	posX = x;
	posY = y;
	posZ = z;
}

void Camera::setRotation(float x, float y, float z)
{
	rotX = x;
	rotY = y;
	rotZ = z;
}

void Camera::render()
{
	VectorType up, position, lookAt;
	float rotationMatrix[9];

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = posX;
	position.y = posY;
	position.z = posZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	float pitch = rotX * 0.0174532925f;
	float yaw = rotY * 0.0174532925f;
	float roll = rotZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	matrixRotationYawPitchRoll(rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	transformCoord(lookAt, rotationMatrix);
	transformCoord(up, rotationMatrix);
	
	// Translate the rotated camera position to the location of the viewer.
	lookAt.x = position.x + lookAt.x;
	lookAt.y = position.y + lookAt.y;
	lookAt.z = position.z + lookAt.z;

	// Finally create the view matrix from the three updated vectors.
	buildViewMatrix(position, lookAt, up);

}

void Camera::matrixRotationYawPitchRoll(float* matrix, float yaw, float pitch, float roll)
{
	// Get the cosine and sin of the yaw, pitch, and roll.
	float cYaw = cosf(yaw);
	float cPitch = cosf(pitch);
	float cRoll = cosf(roll);

	float sYaw = sinf(yaw);
	float sPitch = sinf(pitch);
	float sRoll = sinf(roll);

	// Calculate the yaw, pitch, roll rotation matrix.
	matrix[0] = (cRoll * cYaw) + (sRoll * sPitch * sYaw);
	matrix[1] = (sRoll * cPitch);
	matrix[2] = (cRoll * -sYaw) + (sRoll * sPitch * cYaw);
	
	matrix[3] = (-sRoll * cYaw) + (cRoll * sPitch * sYaw);
	matrix[4] = (cRoll * cPitch);
	matrix[5] = (sRoll * sYaw) + (cRoll * sPitch * cYaw);
	
	matrix[6] = (cPitch * sYaw);
	matrix[7] = -sPitch;
	matrix[8] = (cPitch * cYaw);

}

void Camera::transformCoord(VectorType& vector, float* matrix)
{
	// Transform the vector by the 3x3 matrix.
	float x = (vector.x * matrix[0]) + (vector.y * matrix[3]) + (vector.z * matrix[6]);
	float y = (vector.x * matrix[1]) + (vector.y * matrix[4]) + (vector.z * matrix[7]);
	float z = (vector.x * matrix[2]) + (vector.y * matrix[5]) + (vector.z * matrix[8]);

	// Store the result in the reference.
	vector.x = x;
	vector.y = y;
	vector.z = z;
}

void Camera::buildViewMatrix(VectorType position, VectorType lookAt, VectorType up)
{
	VectorType zAxis, xAxis, yAxis;


	// zAxis = normal(lookAt - position)
	zAxis.x = lookAt.x - position.x;
	zAxis.y = lookAt.y - position.y;
	zAxis.z = lookAt.z - position.z;
	float length = sqrt((zAxis.x * zAxis.x) + (zAxis.y * zAxis.y) + (zAxis.z * zAxis.z));
	zAxis.x = zAxis.x / length;
	zAxis.y = zAxis.y / length;
	zAxis.z = zAxis.z / length;

	// xAxis = normal(cross(up, zAxis))
	xAxis.x = (up.y * zAxis.z) - (up.z * zAxis.y);
	xAxis.y = (up.z * zAxis.x) - (up.x * zAxis.z);
	xAxis.z = (up.x * zAxis.y) - (up.y * zAxis.x);
	length = sqrt((xAxis.x * xAxis.x) + (xAxis.y * xAxis.y) + (xAxis.z * xAxis.z));
	xAxis.x = xAxis.x / length;
	xAxis.y = xAxis.y / length;
	xAxis.z = xAxis.z / length;

	// yAxis = cross(zAxis, xAxis)
	yAxis.x = (zAxis.y * xAxis.z) - (zAxis.z * xAxis.y);
	yAxis.y = (zAxis.z * xAxis.x) - (zAxis.x * xAxis.z);
	yAxis.z = (zAxis.x * xAxis.y) - (zAxis.y * xAxis.x);

	// -dot(xAxis, position)
	float result1 = ((xAxis.x * position.x) + (xAxis.y * position.y) + (xAxis.z * position.z)) * -1.0f;

	// -dot(yaxis, eye)
	float result2 = ((yAxis.x * position.x) + (yAxis.y * position.y) + (yAxis.z * position.z)) * -1.0f;

	// -dot(zaxis, eye)
	float result3 = ((zAxis.x * position.x) + (zAxis.y * position.y) + (zAxis.z * position.z)) * -1.0f;

	// Set the computed values in the view matrix.
	viewMatrix[0]  = xAxis.x;
	viewMatrix[1]  = yAxis.x;
	viewMatrix[2]  = zAxis.x;
	viewMatrix[3]  = 0.0f;

	viewMatrix[4]  = xAxis.y;
	viewMatrix[5]  = yAxis.y;
	viewMatrix[6]  = zAxis.y;
	viewMatrix[7]  = 0.0f;

	viewMatrix[8]  = xAxis.z;
	viewMatrix[9]  = yAxis.z;
	viewMatrix[10] = zAxis.z;
	viewMatrix[11] = 0.0f;

	viewMatrix[12] = result1;
	viewMatrix[13] = result2;
	viewMatrix[14] = result3;
	viewMatrix[15] = 1.0f;
}

void Camera::getViewMatrix(float* matrix)
{
	std::memcpy(matrix, viewMatrix, sizeof(viewMatrix));
}
