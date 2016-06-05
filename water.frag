#version 330 core

in vec3 FragPos;
in vec3 FragNormal;

uniform vec3 viewCam;
uniform samplerCube skybox;
//Define out variable for the fragment shader: color.
out vec4 color;

void main()
{
	vec4 total_color = vec4(0, 0, 0, 1);

	//Fake lighting
	vec3 light_color = vec3(0.1, 0.129, 0.929);
	vec3 toLightVector = vec3(1.0, 1.0, -0.2);
	vec3 unitNormal = normalize(FragNormal);
	vec3 unitLightVector = normalize(toLightVector);

	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.1);
	vec3 diffuse = brightness * light_color;

	vec3 unitVectorToCamera = normalize(FragPos - viewCam);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection,unitNormal);
	
	float specularFactor = dot(reflectedLightDirection , unitVectorToCamera);
	specularFactor = max(specularFactor, 0.01);
	float dampedFactor = pow(specularFactor, 0.5);
	float reflectivity = 0.1;
	vec3 finalSpecular = dampedFactor * reflectivity * light_color;

	total_color = vec4(diffuse, 1.0) + vec4(finalSpecular, 1.0);
    
    //Water to air ratio.
    float ratio = 1.0/1.33;
    
    vec3 I = normalize(FragPos - viewCam);
    
    vec3 reflection = reflect(I, normalize(FragNormal));
    vec3 refraction = refract(I, normalize(FragNormal), ratio);

    vec4 colorReflection = texture(skybox, reflection);
    vec4 colorRefraction = texture(skybox, refraction);

	float percent = 0.3;
    
    color = normalize(mix(colorReflection, colorRefraction, percent));

}
    