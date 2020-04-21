#include <cmath>
#include "Camera.h"

Camera::Camera(): m_viewMatrix{}
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

Camera::Camera(const Camera& other): m_positionX(0), m_positionY(0), m_positionZ(0), m_rotationX(0),
                                                    m_rotationY(0),
                                                    m_rotationZ(0),
                                                    m_viewMatrix{}
{
}

Camera::~Camera()
{
}

void Camera::setPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void Camera::setRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
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
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	float pitch = m_rotationX * 0.0174532925f;
	float yaw = m_rotationY * 0.0174532925f;
	float roll = m_rotationZ * 0.0174532925f;

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
	m_viewMatrix[0]  = xAxis.x;
	m_viewMatrix[1]  = yAxis.x;
	m_viewMatrix[2]  = zAxis.x;
	m_viewMatrix[3]  = 0.0f;

	m_viewMatrix[4]  = xAxis.y;
	m_viewMatrix[5]  = yAxis.y;
	m_viewMatrix[6]  = zAxis.y;
	m_viewMatrix[7]  = 0.0f;

	m_viewMatrix[8]  = xAxis.z;
	m_viewMatrix[9]  = yAxis.z;
	m_viewMatrix[10] = zAxis.z;
	m_viewMatrix[11] = 0.0f;

	m_viewMatrix[12] = result1;
	m_viewMatrix[13] = result2;
	m_viewMatrix[14] = result3;
	m_viewMatrix[15] = 1.0f;
}

void Camera::getViewMatrix(float* matrix)
{
	matrix[0]  = m_viewMatrix[0];
	matrix[1]  = m_viewMatrix[1];
	matrix[2]  = m_viewMatrix[2];
	matrix[3]  = m_viewMatrix[3];

	matrix[4]  = m_viewMatrix[4];
	matrix[5]  = m_viewMatrix[5];
	matrix[6]  = m_viewMatrix[6];
	matrix[7]  = m_viewMatrix[7];

	matrix[8]  = m_viewMatrix[8];
	matrix[9]  = m_viewMatrix[9];
	matrix[10] = m_viewMatrix[10];
	matrix[11] = m_viewMatrix[11];

	matrix[12] = m_viewMatrix[12];
	matrix[13] = m_viewMatrix[13];
	matrix[14] = m_viewMatrix[14];
	matrix[15] = m_viewMatrix[15];
}