#version 430 core
out vec4 FragColor;

const int MAX_WAVES = 64;
const float pi = 3.14159265359;

struct Material {
  vec3 diffuseColor;
  float shininess;
};

struct Color {
	vec3 body;
	vec3 specular;
};

struct Wave {
	float freq;
	float phaseOffset;
	float amp;
	vec2 dir;
};

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in VS_OUT {
	vec3 Normal;
	vec3 Binormal;
	vec3 Tangent;
	vec3 VertPos;
	vec3 FragPos;
	vec2 TexCoords;
	vec3 debugColor;
} fs_in;


uniform DirLight dirLight;
uniform float k;
uniform float uTime;
uniform float drag;
uniform samplerCube skybox;
uniform Material material;
uniform vec3 viewPos;
uniform float steepness;
uniform float edgeLength;
uniform int numWaves;

uniform Wave[MAX_WAVES] geometricWaves;

struct waveData {
	vec3 offset;
	vec3 tilt;
	vec3 binormal;
	vec3 tangent;
};

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
	vec3 binormal = vec3( -q * wa2 * pow(wave.dir.x, 2) * s2, wa * wave.dir.x * s * c, -q * wa2 * wave.dir.x * wave.dir.y * s2);
	vec3 tangent = vec3(- q * wa2 * wave.dir.y * wave.dir.x * s2, wa * wave.dir.y * s * c,  -q * wa2 * pow(wave.dir.y , 2) * s2);
 
	vec2 shift = wave.dir *(wave.amp * s * c) * drag;
 
	return waveData(offset, vec3(shift, 0), binormal, tangent);
}

vec3 calcEnvironmentMap(vec3 normal, vec3 R){
    return vec3(texture(skybox, R).rgb);
}

float calcFresnel(vec3 normal, vec3 viewDir){
	float n1 = 1.0;
	float n2 = 1.33;
	float F0 = pow((n1 - n2) / (n1 + n2), 2.0);
	float base = 1 - max(dot(viewDir, normal), 0.0);
	float exponential = pow(base, 5.0);
	return exponential + F0 * (1.0 - exponential);
}

Color CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);

  float diff = max(dot(normal, lightDir), 0.0);

  vec3 halfDir = normalize(lightDir + viewDir);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(normal, halfDir), 0.0), material.shininess);

  vec3 ambient = light.ambient * material.diffuseColor;
  vec3 diffuse = light.diffuse * diff * material.diffuseColor;
  vec3 specular = light.specular * spec;

  Color res;
  res.body = ambient + diffuse;
  res.specular = specular;

  return res;
}

void main() {

	vec2 tiltSum = vec2(0);
	vec3 normal = vec3(0);
	vec3 binormalSum = vec3(0);
	vec3 tangentSum = vec3(0);
	
	vec2 p = fs_in.VertPos.xz;
	for(int i = 0; i < numWaves; i++){
		waveData data = eulerWave(geometricWaves[i], vec3(p.x, fs_in.VertPos.y, p.y));
		tangentSum += data.tangent;
		binormalSum += data.binormal;
		p += data.tilt.xy;
	}
	tangentSum.z += 1;
	binormalSum.x += 1;
	normal = normalize(cross(tangentSum, binormalSum));
	float ratio = 1.00 / 1.33;
	vec3 I = normalize(fs_in.FragPos - viewPos);
    vec3 R = reflect(I, normalize(normal));
	vec3 envMap = calcEnvironmentMap(normal, R);

	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

	float fresnel = calcFresnel(normal, viewDir);

	float fade = smoothstep(-0.3, 0.0, R.y);

	Color res = CalcDirLight(dirLight, normal, viewDir);
	vec3 result = mix(res.body, envMap, fresnel * fade) + res.specular;
	// vec3 result = res.body + res.specular;

	FragColor = vec4(result, 1.0);
	// FragColor.rgb = vec3(normal);

}
