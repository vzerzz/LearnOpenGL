#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material{
    // vec3 ambient;
    // vec3 diffuse;
    sampler2D diffuse; // 漫反射和环境光采用同一个贴图
    sampler2D specular;
    // sampler2D emission; 自发光项
    // vec3 specular;
    float shininess;
};

struct DirLight{
    vec3 direction; // 平行光源

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    vec3 position; // 点光源
    // 距离衰减
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight{
    vec3 position;
    vec3 direction;

    float cutOff; //聚光
    float outerCutOff; //外围
    // 距离衰减
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};
  
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

// uniform float matrixLight;
// uniform float matrixMove;

vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);

    // 平行光
    vec3 result = CalDirLight(dirLight, Normal, viewDir);
    // 点光源
    for(int i=0; i < NR_POINT_LIGHTS; i++)
        result += CalPointLight(pointLights[i], Normal, FragPos, viewDir);
    // 聚光
    result += CalSpotLight(spotLight, Normal, FragPos, viewDir);

    FragColor = vec4(result, 1.0);

}

vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir){
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;
    vec3 diffuse = light.diffuse * max(dot(norm, lightDir), 0.0) * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * vec3(texture(material.specular, TexCoords));

    return ambient + diffuse + specular;
}


vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;
    vec3 diffuse = light.diffuse * max(dot(norm, lightDir), 0.0) * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * vec3(texture(material.specular, TexCoords));

    //距离衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;
    vec3 diffuse = light.diffuse * max(dot(norm, lightDir), 0.0) * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * vec3(texture(material.specular, TexCoords));

    // 聚光 平滑过渡
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    
    diffuse *= intensity;
    specular *= intensity;

    //距离衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}