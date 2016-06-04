#version 330 core

//Setup the object's material.
in vec3 FragPos;
in vec3 FragNormal;

uniform vec3 viewPos;
uniform samplerCube skybox;
uniform float randVal;
//Define out variable for the fragment shader: color.
out vec4 color;



void main()
{    
	//Fake lighting
	vec3 light_color = vec3(1.0-0.3, 0.929-0.3, 0.929);
	vec3 toLightVector = vec3(1.0, 1.0, -0.2);
	vec3 unitNormal = normalize(FragNormal);
	vec3 unitLightVector = normalize(toLightVector);

	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.1);
	vec3 diffuse = brightness * light_color;

	vec3 unitVectorToCamera = normalize(FragPos - viewPos);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection,unitNormal);
	
	float specularFactor = dot(reflectedLightDirection , unitVectorToCamera);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor,0.5);
	float reflectivity = 0.05;
	vec3 finalSpecular = dampedFactor * reflectivity * light_color;

	color = vec4(diffuse, 1.0) + vec4(finalSpecular, 1.0);
    
    //Refraction ratio for water to air
    float ratio = 1.33/1.00;
    
    vec3 I = normalize(FragPos - viewPos);
    
    vec3 reflect_factor = reflect(I, normalize(FragNormal) );
    vec3 refract_factor = refract(I, normalize(FragNormal), ratio);
	
	vec4 colorReflect = texture(skybox, reflect_factor);
	vec4 colorRefract = texture(skybox, refract_factor);

	colorReflect *= 0.8;
	colorRefract *= 0.3;

	color = colorReflect + colorRefract;

}
    