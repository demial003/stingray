#version 430 core
out vec4 FragColor;

const int MAX_WAVES = 32;
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

in vec3 VertPos;
in vec3 FragPos;
in vec3 Binormal;
in vec3 Tangent;
in vec3 Normal;
in vec2 TexCoords;
in vec3 debugColor;


uniform DirLight dirLight;
uniform float k;
uniform float uTime;
uniform samplerCube skybox;
uniform Material material;
uniform vec3 viewPos;
uniform float steepness;
uniform float edgeLength;
uniform int numWaves;

uniform Wave[MAX_WAVES] textureWaves;
uniform Wave[MAX_WAVES] geometricWaves;
uniform int numTextureWaves;

struct waveData {
	vec3 offset;
	vec3 tilt;
	vec3 binormal;
	vec3 tangent;
};

vec2 ripple(Wave wave, vec3 bPos){
	float u = dot(wave.dir, bPos.xz ) * wave.freq + uTime * wave.phaseOffset;

	float c1 = wave.dir.x * wave.freq * wave.amp * k;
	float c2 = wave.dir.y * wave.freq * wave.amp * k;
	
	float s = exp(sin(u) - 1);
	float c = cos(u);
	
	float du = c1 * s * c;
	float dv = c2 * s * c;

	return vec2(du, dv);

}

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

vec3 calcEnvironmentMap(vec3 normal){
	float ratio = 1.00 / 1.33;
	vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(normal));
    // vec3 R = refract(I, normalize(Normal), ratio);
    return vec3(texture(skybox, R).rgb);
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
	
	for(int i = 0; i <  numTextureWaves; i++){
		tiltSum += ripple(textureWaves[i], VertPos);
	}
	for(int i = 0; i < numWaves; i++){
		waveData data = eulerWave(geometricWaves[i], VertPos);
		data.tangent.z -= 1;
		data.binormal.x -= 1;
		tangentSum += data.tangent ;
		binormalSum += data.binormal ;
	}
	tangentSum.z += 1;
	binormalSum.x += 1;
	 normal = normalize(cross(tangentSum, binormalSum));
	 // normal = normalize(vec3(Normal.x - tiltSum.x, Normal.y, Normal.z - tiltSum.y));
	vec3 viewDir = normalize(viewPos - FragPos);
	Color res = CalcDirLight(dirLight, normal, viewDir);
	vec3 result = mix(res.body, calcEnvironmentMap(normal), 0.3) + res.specular;
	FragColor = vec4(result, 1.0);
	// FragColor.rgb = normal * 0.5 + 0.5;

}
