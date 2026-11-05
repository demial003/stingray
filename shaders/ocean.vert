#version 400 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

const float pi = 3.14159265359;
const int MAX_WAVES = 8;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float k;
uniform float uTime;

struct Wave {
	float freq;
	float phaseOffset;
	float amp;
	vec2 dir;
};

struct waveData {
	vec3 offset;
	vec3 tilt;
};

uniform float steepness;
uniform int numWaves;
uniform int numTextureWaves;

uniform Wave[MAX_WAVES] geometricWaves;
uniform Wave[MAX_WAVES] textureWaves;

// void sumOfSines(Wave wave){
// 	vec3 bPos = vec3(model * vec4(aPos, 1.0));
// 
// 	float k = 2.0;
// 	float s = sin(dot(dir, bPos.xz) * freq + uTime * phase);
// 	float c = cos(dot(dir, bPos.xz) * freq + uTime * phase);
// 
// 	float waveHeight = 2.0 * amp * pow((s + 1.0)/2.0, k);
// 
// 	float dx = k * dir.x * freq * amp * pow((s + 1.0) / 2.0, k - 1.0) * c;
// 	float dz = k * dir.y * freq * amp * pow((s + 1.0) / 2.0, k - 1.0) * c;
// 
// 	vec3 sineSumPos = vec3(bPos.x, bPos.y + waveHeight, bPos.z);
// 
// 	vec3 binormal = vec3(1.0, dx, 0.0);
// 	vec3 tangent = vec3(0.0, dz, 1.0);
// 	vec3 norm = normalize(cross(tangent, binormal));
// 
// 	 FragPos = sineSumPos;
// 	 Normal = norm;
// 	TexCoords = aTexCoord;
// 	gl_Position = projection * view * vec4(FragPos, 1.0);
// }

vec3 textureWave(Wave wave, vec3 bPos){
	bPos = bPos * 0.5 + 0.5;

	float u = dot(wave.dir, bPos.xz ) + wave.phaseOffset;

	float c1 = wave.dir.x * wave.freq * wave.amp * k;
	float c2 = wave.dir.y * wave.freq * wave.amp * k;
	
	float s = pow((sin(u) + 1.0) / 2.0, k - 1.0);
	float c = k * cos(u);
	
	float du = c1 * s * c;
	float dv = c2 * s * c;

	vec3 binormal = vec3(1.0, du, 0.0);
	vec3 tangent = vec3(0.0, dv, 1.0);

	return normalize(cross(tangent, binormal) * 2.0 - 1.0);
}

waveData geometricWave(Wave wave, vec3 bPos){
 	float s = sin(dot(wave.dir, bPos.xz) * wave.freq + uTime * wave.phaseOffset);
 	float c = cos(dot(wave.dir, bPos.xz) * wave.freq + uTime * wave.phaseOffset);
 
 	float q = steepness / (wave.freq * wave.amp * numWaves);
 
 	float x = q * wave.amp * wave.dir.x * c;
 	float y = wave.amp * s;
 	float z = q * wave.amp * wave.dir.y * c;

	float wa = wave.freq * wave.amp;
	vec3 offset = vec3(x, y, z);
	vec3 tilt = vec3(wave.dir.x * wa * c, q * wa * s ,wave.dir.y * wa * c);
 
	return waveData(offset, tilt);
 }

void main(){
 	vec3 bPos = vec3(model * vec4(aPos, 1.0));
	vec3 posSum = vec3(0);
	vec3 normalSum = vec3(0);

	FragPos = vec3(0);
	Normal = vec3(0);

	for(int i = 0; i < numWaves; i++){
		waveData data = geometricWave(geometricWaves[i], bPos);
		posSum += data.offset;
		normalSum += data.tilt;
	}
	for(int i = 0; i <  numTextureWaves; i++){
		normalSum += textureWave(textureWaves[i], bPos);
	}

	FragPos = bPos + posSum;
	Normal = normalize(vec3(-normalSum.x, 1.0 - normalSum.y, -normalSum.z));
	TexCoords = aTexCoord;
	gl_Position = projection * view * vec4(FragPos, 1.0);
}
