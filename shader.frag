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

//Define any in variables from the vertex shader.
in vec3 FragPos;
in vec3 FragNormal;
in vec2 FragTexCoords;

//Define out variable for the fragment shader: color.
out vec4 color;

// Function prototypes
vec3 CalcDirLight(DirLight dirLight, vec3 norm, vec3 viewDir);

void main()
{
	vec3 norm = normalize(FragNormal);
	vec3 viewDir = normalize(viewPos - FragPos);

	//Calculate Directional Light
	vec3 result =  CalcDirLight(dirLight, norm, viewDir);


    color = vec4(result.x, result.y, result.z, 1.0f);
}


vec3 CalcDirLight(DirLight dirLight, vec3 norm, vec3 viewDir)
{
	//Calculate Ambient: Ka * La
	vec3 l_ambient = material.ambient * dirLight.ambient;

	//Calculate Diffuse: Kd * Ld * max (0, n * l)
    vec3 lightDir = normalize(-dirLight.direction);
    float maxDiffuse = max(0.0, dot(norm, lightDir));
	vec3 l_diffuse = material.diffuse * dirLight.diffuse * maxDiffuse;

	//Calculate Specular: Ks * Ls * max (0, r dot v)
	vec3 reflectDir = reflect(-lightDir, norm);
	float maxSpecular = max(0.0, dot(reflectDir, viewDir));
    vec3 l_specular = material.specular * dirLight.specular * pow(maxSpecular, material.shininess);

	return (l_ambient + l_diffuse + l_specular);
}