#version 400

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

out vec3 vert;
out vec3 vertNormal;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main()
{
   vert = vec3(modelMatrix * vec4(vertex, 1.0));
   vertNormal = mat3(transpose(inverse(modelMatrix))) * normal;
   gl_Position = projectionMatrix * viewMatrix * vec4(vert, 1.0);
}
