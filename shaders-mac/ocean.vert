#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec3 Binormal;
out vec3 Tangent;
out vec3 VertPos;
out vec3 FragPos;
out vec2 TexCoords;
out vec3 debugColor;

const float pi = 3.14159265359;
const int MAX_WAVES = 32;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float k;
uniform float uTime;
uniform float edgeLength;

struct Wave {
	float freq;
	float phaseOffset;
	float amp;
	vec2 dir;
};

struct waveData {
	vec3 offset;
	vec3 tilt;
	vec3 binormal;
	vec3 tangent;
};

uniform float steepness;
uniform int numWaves;

uniform Wave[MAX_WAVES] geometricWaves;


waveData eulerWave(Wave wave, vec3 bPos){
 	float s = exp(sin(dot(wave.dir, bPos.xz) * wave.freq + uTime * wave.phaseOffset) - 1.0);
 	float c = cos(dot(wave.dir, bPos.xz) * wave.freq + uTime * wave.phaseOffset);
 	float s2 = sin(dot(wave.dir, bPos.xz) * wave.freq + uTime * wave.phaseOffset);

 	float wavelength = 2.0 * pi / wave.freq;
 	float ratio = wavelength / edgeLength;
 
    float weight = smoothstep(2.0, 4.0, ratio);
	wave.amp *= weight;
 
 	float q = steepness / (wave.freq * numWaves);
 
 	float x = q * wave.dir.x * c * weight;
 	float y = wave.amp * s;
 	float z = q * wave.dir.y * c * weight;

	float wa = wave.freq * wave.amp;
	float wa2 = wave.freq * weight;
	vec3 offset = vec3(x, y, z);
	vec3 binormal = vec3(1 - q * wa2 * pow(wave.dir.x, 2) * s2, wa * wave.dir.x * s * c, -q * wa2 * wave.dir.x * wave.dir.y * s2);
	vec3 tangent = vec3(- q * wa2 * wave.dir.y * wave.dir.x * s2, wa * wave.dir.y * s * c, 1 - q * wa2 * pow(wave.dir.y , 2) * s2);
 
	return waveData(offset, vec3(0), binormal, tangent);
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
 
	return waveData(offset, tilt, vec3(0), vec3(0));
 }

void main(){
 	vec3 bPos = vec3(model * vec4(aPos, 1.0));
	vec3 posSum = vec3(0);
	vec3 normalSum = vec3(0);
	vec3 binormalSum = vec3(0);
	vec3 tangentSum = vec3(0);

	FragPos = vec3(0);
	Normal = vec3(0);

	for(int i = 0; i < numWaves; i++){
		waveData data = eulerWave(geometricWaves[i], bPos);
		data.tangent.z -= 1;
		data.binormal.x -= 1;
		posSum += data.offset;
		tangentSum += data.tangent ;
		binormalSum += data.binormal ;
	 }

	tangentSum.z += 1;
	binormalSum.x += 1;
	vec3 normal = normalize(cross(tangentSum, binormalSum));
	
	VertPos = bPos;
	FragPos = bPos + posSum;
	Normal = normal;
	Binormal = normalize(binormalSum);
	Tangent = normalize(tangentSum);
	TexCoords = aTexCoord;
	gl_Position = projection * view * vec4(FragPos, 1.0);
}
