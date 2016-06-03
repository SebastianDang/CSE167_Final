#version 330 core

//Setup the object's material.
in vec3 FragPos;
in vec3 FragNormal;

uniform vec3 viewCam;
uniform samplerCube skybox;
uniform float randVal;
//Define out variable for the fragment shader: color.
out vec4 color;



void main()
{    
	/*
    float randReflect = 1.6666;
    float randRefract = 1.785714;
    float maxDiffuse = max(0.0, dot(FragNormal, viewCam));
    if(maxDiffuse < 0.5) {
        randReflect = randVal;
    }
    else if(maxDiffuse > 0.5) {
        randRefract = randVal;
    }
    */
    
    //Refraction ratio for water to air
    float ratio = 1.33/1.00;
    
    vec3 I = normalize(FragPos - viewCam);
    
    
    vec3 reflect_factor = reflect(I, normalize(FragNormal) /** (maxDiffuse*.3) * (randReflect*.6)*/);
    vec3 refract_factor = refract(I, normalize(FragNormal) /** (maxDiffuse*.5) * (randRefract*.56)*/, ratio);
	
	vec4 colorReflect = texture(skybox, reflect_factor);
	vec4 colorRefract = texture(skybox, refract_factor);

	colorReflect *= 0.0;
	colorRefract *= 0.8;

	color = colorReflect + colorRefract;

	/*
   // color.z = 0.8;
    color.w = 1.0;
    if(color.x > 0.15) color.x = 0.15;
    if(color.y > 0.15) color.x = 0.15;
    */
    


}
    