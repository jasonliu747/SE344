#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in vec3 Pos;

uniform sampler2D texture_diffuse1;
uniform float intensity;

void main()
{
	// 漫反射 + 0.2环境光
	vec3 light = intensity * max( dot(normalize(Normal), normalize(vec3(0)-Pos)), 0.0 ) + vec3(0.2); 
	// 光源 * 纹理的颜色
    FragColor = vec4(vec3(texture(texture_diffuse1, TexCoords) * light), 1.0);
}