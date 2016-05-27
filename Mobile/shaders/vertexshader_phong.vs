#version 330
//ground functionality found in a youtube tutorial

layout (location = 0) in vec3 vPosition; //vertex position
layout (location = 1) in vec3 outputColor;
layout (location = 2) in vec3 vertex_normal;

//texturing - from example code
layout (location = 3) in vec2 UV;

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix; //camera pos
uniform vec3 lightPosition;

//have to pass these arguments to the fragment shader (phong calculation)
out vec3 vertexNormal;
out vec3 eyeDirection; //camera dir
out vec3 lightDirection;

//texturing - from example code
out vec2 UVcoords;

void main() {
	//multiply normal matrix with normal
	mat4 normalMatrix = transpose(inverse(ModelMatrix));
	vertexNormal = (normalMatrix*vec4(vertex_normal, 0.0)).xyz;

	vec3 vertexInCamSpace = vec4(ViewMatrix*ModelMatrix*vec4(vPosition.x, vPosition.y, vPosition.z, 1.0)).xyz;
	eyeDirection = -vertexInCamSpace.xyz; //eye vector

	vec3 lightInCamSpace = vec4(ViewMatrix*ModelMatrix*vec4(lightPosition.x, lightPosition.y, lightPosition.z, 1.0)).xyz;
	lightDirection = vec3(lightInCamSpace - vertexInCamSpace).xyz;

	//gl_Position — contains the position of the current vertex
	gl_Position = ProjectionMatrix*vec4(vertexInCamSpace, 1.0);

	//texturing - from example code
	UVcoords = UV;
}

