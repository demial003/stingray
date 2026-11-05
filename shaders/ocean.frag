#version 400 core
out vec4 FragColor;

struct Material {
  vec3 diffuseColor;
  float shininess;
};

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform DirLight dirLight;
uniform float k;
uniform samplerCube skybox;
uniform Material material;
uniform vec3 viewPos;

vec3 calcEnvironmentMap(){
	float ratio = 1.00 / 1.33;
	vec3 I = normalize(FragPos - viewPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    return vec3(texture(skybox, R).rgb);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);

  float diff = max(dot(normal, lightDir), 0.0);

  vec3 halfDir = normalize(lightDir + viewDir);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(normal, halfDir), 0.0), material.shininess);

  vec3 ambient = light.ambient * material.diffuseColor;
  vec3 diffuse = light.diffuse * diff * material.diffuseColor;
  vec3 specular = light.specular * spec;

  return (ambient + diffuse + specular);
}

void main() {
  vec3 normal = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 result = CalcDirLight(dirLight, normal, viewDir) + calcEnvironmentMap();
  // vec3 result = CalcDirLight(dirLight, norm, viewDir); 
  FragColor = vec4(result, 1.0);

}
