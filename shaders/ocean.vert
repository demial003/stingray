#version 400 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

const float pi = 3.14159265359;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

uniform float wavelength;
uniform float amp;
uniform float speed;
uniform vec2 dir;

void main(){
	FragPos = vec3(model * vec4(aPos, 1.0));

	float freq = 2.0 * pi / wavelength;
	float phase = speed * freq;
	float waveHeight = amp * sin(dot(dir, (vec2(FragPos.xz))) * freq + uTime * phase);

	FragPos = vec3(FragPos.x, waveHeight, FragPos.z);
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoord;

	 





	gl_Position = projection * view * vec4(FragPos, 1.0);
}
