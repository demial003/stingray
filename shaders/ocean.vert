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
uniform float steepness;
uniform vec2 dir;

void main(){
	FragPos = vec3(model * vec4(aPos, 1.0));

	float k = 2.0;
	float freq = 2.0 * pi / wavelength;
	float phase = speed * freq;
	float s = sin(dot(dir, (FragPos.xz)) * freq + uTime * phase) + 1;
	float c = cos(dot(dir, (FragPos.xz)) * freq + uTime * phase);

	float q = steepness / (freq * amp);

	float x = q * amp * dir.x * c;
	float y = amp * s;
	float z = q * amp * dir.y * c;

	float waveHeight = 2.0 * amp * pow(s/2.0, k);

	float dx = k * dir.x * freq * amp * pow(s / 2.0, k - 1.0) * c;
	float dz = k * dir.y * freq * amp * pow(s / 2.0, k - 1.0) * c;


	vec3 binormal = vec3(1.0, dz, 0.0);
	vec3 tangent = vec3(0.0, dx, 1.0);
	vec3 norm = cross(binormal, tangent);
		

	// FragPos = vec3(FragPos.x, FragPos.y += waveHeight, FragPos.z);
	FragPos = vec3(FragPos.x += x, FragPos.y += y, FragPos.z += z);
	Normal = norm;
	TexCoords = aTexCoord;

	 





	gl_Position = projection * view * vec4(FragPos, 1.0);
}
