#version 330 core

uniform struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} material;

uniform struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} dirLight;

uniform vec3 viewPos;

//Define any out variables from the vertex shader.
in vec3 FragPos;
in vec3 FragNormal;
in vec2 FragTexCoords;

//Define out variable for the fragment shader: color.
out vec4 color;

//Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
	vec3 norm = normalize(FragNormal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

    color = vec4(result.x, result.y, result.z, 1.0);
}

//Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	//Calculate Ambient: Ka * La
	vec3 l_ambient = material.ambient * light.ambient;

	//Calculate Diffuse: Kd * Ld * max (0, n * l);
    vec3 lightDir = normalize(-light.direction);
    float maxDiffuse = max(0.0, dot(normal, lightDir));
	vec3 l_diffuse = material.diffuse * light.diffuse * maxDiffuse;

	//Calculate Specular: Ks * Ls * max (0, r * v);
	vec3 reflectDir = reflect(-lightDir, normal);
	float maxSpecular = max(0.0, dot(viewDir, reflectDir));
    vec3 l_specular = material.specular * light.specular * pow(maxSpecular, material.shininess);

    return (l_ambient + l_diffuse + l_specular);
}
