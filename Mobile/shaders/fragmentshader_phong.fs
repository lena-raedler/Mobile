#version 330

struct LightSource {
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
	vec3 position;
};

struct Material {
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
	float specular_shininess;
};

uniform LightSource light;
uniform Material material;

in vec3 vertexNormal;
in vec3 eyeDirection;
in vec3 lightDirection;

out vec4 outputColor;

void main() {

	vec3 E = normalize(eyeDirection);
	vec3 N = normalize(vertexNormal);
	vec3 L = normalize(lightDirection);

	vec3 H = normalize(E + L);

	//initialize color values
	vec3 ambient = vec3(0);
	vec3 diffuse = vec3(0);
	vec3 specular = vec3(0);
	
	ambient += light.ambient_color;
	diffuse += light.diffuse_color * max(dot(L,N),0);
	specular += light.specular_color * pow(max(dot(H,N),0), material.specular_shininess); //phong shading formula

	ambient *= material.ambient_color;
	diffuse *= material.diffuse_color;
	specular *= material.specular_color;

	outputColor = vec4(ambient+diffuse+specular, 1.0);
}
