#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in vec3 Pos;

uniform sampler2D texture_diffuse1;
uniform float intensity;

void main()
{
	// ������ + 0.2������
	vec3 light = intensity * max( dot(normalize(Normal), normalize(vec3(0)-Pos)), 0.0 ) + vec3(0.2); 
	// ��Դ * �������ɫ
    FragColor = vec4(vec3(texture(texture_diffuse1, TexCoords) * light), 1.0);
}