#version 400 core
out vec4 FragColor;

const int MAX_WAVES = 50;

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
in vec3 Normal;
in vec2 TexCoords;


uniform DirLight dirLight;
uniform float k;
uniform float uTime;
uniform samplerCube skybox;
uniform Material material;
uniform vec3 viewPos;

uniform Wave[MAX_WAVES] textureWaves;
uniform int numTextureWaves;

vec2 ripple(Wave wave, vec3 bPos){
	float u = dot(wave.dir, bPos.xz ) * wave.freq + uTime * wave.phaseOffset;

	float c1 = wave.dir.x * wave.freq * wave.amp * k;
	float c2 = wave.dir.y * wave.freq * wave.amp * k;
	
	float s = pow(exp(sin(u) - 1.0), k - 1.0);
	float c = cos(u);
	
	float du = c1 * s * c;
	float dv = c2 * s * c;

	return vec2(du, dv);

}

vec3 calcEnvironmentMap(){
	float ratio = 1.00 / 1.33;
	vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(Normal));
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
	for(int i = 0; i <  numTextureWaves; i++){
		tiltSum += ripple(textureWaves[i], VertPos);
	}
	vec3 normal = normalize(vec3(Normal.x - tiltSum.x, Normal.y, Normal.z - tiltSum.y));
	vec3 viewDir = normalize(viewPos - FragPos);
	Color res = CalcDirLight(dirLight, normal, viewDir);
	 vec3 result = mix(res.body, calcEnvironmentMap(), 0.3) + res.specular;
	// vec3 result = res.body + res.specular;
	  FragColor = vec4(result, 1.0);
	 // FragColor = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);

}
