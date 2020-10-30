#version 400

in vec3 vert;
in vec3 vertNormal;

out vec4 fragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 objColour;

void main()
{
   vec3 lightColor = vec3(1.0, 1.0, 1.0);
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * lightColor;
   vec3 lightDir  = normalize(lightPos - vert);
   vec3 norm = normalize(vertNormal);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;
   float specularStrength = 0.5;
   vec3 viewDir = normalize(viewPos - vert);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
   vec3 specular = specularStrength * spec * lightColor;
   
   fragColor = vec4(ambient + diffuse + specular, 1.0) * objColour;
}
