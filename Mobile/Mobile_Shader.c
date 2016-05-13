/*********************************************************************
*
*Mobile_Shader.c
*This is an implementation of a Mobile
*
*NOTE: the wand object mentioned resulted in a segmentation fault
*thats why we chose to put the ball object a second time.
*for the next implementation we are going to search for an alternamtive
*object to use
*
*
*Anna-Lena Rädler, Sabrina Schmitzer, Simon Priller
**********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* OpenGL includes */
#include <GL/glew.h>
#include <GL/freeglut.h>

/* Local includes */
#include "LoadShader.h"
#include "Matrix.h"
#include "OBJParser.h"

/* Define handle to a vertex buffer object */
GLuint VBO, VBO1, VBOBall, VBOHeli, VBOBB8, VBOWand;

/* Define handle to a color buffer object */
GLuint CBO, CBO1; 

/* Define handle to an index buffer object */
GLuint IBO, IBO1, IBOBall, IBOHeli, IBOBB8, IBOWand;

/* Arrays for holding vertex data of the models */
GLfloat *vertex_buffer_dataBall;
GLfloat *vertex_buffer_dataHeli;
GLfloat *vertex_buffer_dataBB8;
GLfloat *vertex_buffer_dataWand;

/* Arrays for holding indices of the models */
GLushort *index_buffer_dataBall;
GLushort *index_buffer_dataHeli;
GLushort *index_buffer_dataBB8;
GLushort *index_buffer_dataWand;

/* Structures for loading of OBJ data */
obj_scene_data dataBall;
obj_scene_data dataHeli;
obj_scene_data dataBB8;
obj_scene_data dataWand;

/* Indices to vertex attributes; in this case positon and color */ 
enum DataID {vPosition = 0, vColor = 1}; 

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */ 
float ModelMatrix[16]; /* Big yellow Bar matrix */ 
float BallMatrix[16]; /*for the rotating cube - ball.obj*/
float HelicopterMatrix[16]; /*for the second cube - helicopter */
float BB8Matrix[16]; /*for the tiny cube in the middle - BB8*/
float WandMatrix[16]; /*for cube hanging on the lowest in the middle - wand*/
float FloorMatrix[16]; /*for floor and walls*/

/*float LeftBarMatrix[16];
float RightBarMatrix[16];*/

/* Transformation matrices for initial position */
float TranslateOrigin[16];
float TranslateDown[16]; /*used for bar*/
float RotateX[16];
float RotateZ[16];
float InitialTransform[16];
float RotationMatrix[16];
float RotationMatrixAnim[16]; /*for the rotating cubes*/
float RotationMatrixAnim2[16]; /* for cube rotatio the other way round*/
float TranslateLeft[16]; /*for ballMatrix*/
float TranslateRight[16]; /*for helicopterMatrix*/
float TranslateMiddle[16]; /*for BB8Matrix*/
float TranslateLowest[16]; /*for wandMatrix*/
float TranslateFloor[16];
float InitialTransformCube[16]; /*for rotating cubes*/
float InitialTransformHeli[16];
float InitialTransformBB8[16];
float InitialTransformCube2[16]; /*for cube rotation the other way round*/
float RotateFloor[16];
float RotateHeli[16]; /*rotation Matrix for helicopter object*/
float RotateBB8[16]; /*rotation Matrix for bb8 object*/

float rotationSpeed = 180.0;
float rotateWalls = 3000.0; //automatic cameramode
float stopIt = 1.0;
int switchDirection = 1;
GLboolean anim = GL_TRUE;
GLboolean normalMode = GL_TRUE;
GLboolean automaticMode = GL_TRUE;
float cameraDisp = -30.0;
float cameraPosition = 0.0;

struct LightSource {
	float ambient_color[3];
	float diffuse_color[3];
	float specular_color[3];
	float position[3];
} light, objectLight;

struct Material {
	float ambient_color[3];
	float diffuse_color[3];
	float specular_color[3];
	float specular_shininess;
} material, objectMaterial;



/*yellow bar used as upper layer*/

GLfloat vertex_buffer_data3[] = {
    4.0,0.0,0.0,
    4.0,0.0, -0.5,
    4.0, 1.0, -0.5,
    4.0, 1.0, 0.0,
    -4.0, 0.0, 0.0,
    -4.0, 0.0, -0.5,
    -4.0, 1.0, -0.5,
    -4.0, 1.0, 0.0,
};   

GLfloat color_buffer_data3[] = { /* yellow color for bar*/
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
  
}; 

GLushort index_buffer_data3[] = { /* Indices for the triangles */
    4,3,0,
    7,3,4,
    4,5,7,
    7,5,6,
    0,1,2,
    2,0,3,
    5,1,2,
    6,2,5,
    3,7,2,
    7,6,3,
    4,0,5,
    0,1,5,
};

GLfloat vertex_buffer_data2[] = { //floor
	9.0, 0.0, -9.0,
	-9.0, 0.0, -9.0,
	-9.0, 0.0, 9.0,
	9.0, 0.0, 9.0,
	9.0, 18.0, -9.0,
	-9.0,18.0, -9.0,
	-9.0, 18.0, 9.0,
};

GLfloat color_buffer_data2[] = {
	0.0, 0.1255, 1.451,
	0.0, 1.1255, 0.451,
	0.0, 0.1255, 1.451,
	0.0, 0.1255, 1.451,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
};

GLushort index_buffer_data2[] = {
	0,1,3,
	1,2,3,
	0,4,1,
	4,5,1,
	5,1,6,
	1,6,2,
};

/*----------------------------------------------------------------*/



/******************************************************************
*
* Display
*
* This function is called when the content of the window needs to be
* drawn/redrawn. It has been specified through 'glutDisplayFunc()';
* Enable vertex attributes, create binding between C program and 
* attribute name in shader
*
*******************************************************************/

/*function that draws vertical lines to connect objects in the mobile*/
void drawVerticalLine(float x, float position, float length){
	
	glBegin(GL_LINES);
		glLineWidth(2.5);
		glVertex3f(x, position, 0.0);
		glVertex3f(x, length, 0);
	glEnd();
}

void Display()
{
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    GLint size; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    /* Associate program with shader matrices */
    GLint projectionUniform = glGetUniformLocation(ShaderProgram, "ProjectionMatrix");
    if (projectionUniform == -1) 
    {
        fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
	exit(-1);
    }
    glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, ProjectionMatrix);
    
    GLint ViewUniform = glGetUniformLocation(ShaderProgram, "ViewMatrix");
    if (ViewUniform == -1) 
    {
        fprintf(stderr, "Could not bind uniform ViewMatrix\n");
        exit(-1);
    }
    glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, ViewMatrix);
   
    GLint RotationUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
    if (RotationUniform == -1) 
    {
        fprintf(stderr, "Could not bind uniform ModelMatrix\n");
        exit(-1);
    }
    
	/* for lightning */
	//first light source -- background light
	GLint l1AmbientUniform = glGetUniformLocation(ShaderProgram, "light.ambient_color"); //load in ambient color (from shader)
	glUniformMatrix4fv(l1AmbientUniform, 1, GL_TRUE, light.ambient_color); //load in ambient color (from struct in program)

	GLint l1DiffuseUniform = glGetUniformLocation(ShaderProgram, "light.diffuse_color");
	glUniform3fv(l1DiffuseUniform, 1, light.diffuse_color);
	GLint l1SpecularUniform = glGetUniformLocation(ShaderProgram, "light.specular_color");
	glUniform3fv(l1SpecularUniform, 1, light.specular_color);
	GLint position1Uniform = glGetUniformLocation(ShaderProgram, "light.position");
	glUniform3fv(position1Uniform, 1, light.position);
	GLint light1PosUniform = glGetUniformLocation(ShaderProgram, "lightPosition"); 
	glUniform3fv(light1PosUniform, 1, light.position);

	//for light source 1
	GLint m1AmbientUniform = glGetUniformLocation(ShaderProgram, "material.ambient_color");
	glUniformMatrix4fv(m1AmbientUniform, 1, GL_TRUE, material.ambient_color);
	GLint m1DiffuseUniform = glGetUniformLocation(ShaderProgram, "material.diffuse_color");
	glUniform3fv(m1DiffuseUniform, 1, material.diffuse_color);
	GLint m1SpecularUniform = glGetUniformLocation(ShaderProgram, "material.specular_color");
	glUniform3fv(m1SpecularUniform, 1, material.specular_color);
	GLint m1ShininessUniform = glGetUniformLocation(ShaderProgram, "material.specular_shininess");
	glUniform1f(m1ShininessUniform, material.specular_shininess);

/*-------------------------------------------------------------------------------------------*/

	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix); 
	
	drawVerticalLine(0, 0, 1.5);
	drawVerticalLine(-3, -sqrtf(sqrtf(2.0)*1.0)+2, -2.5); //ball
	drawVerticalLine(3.5, -sqrtf(sqrtf(2.0)*1.0)+2, -2.5); //heli
	drawVerticalLine(0, 0, -4); //BB8
	drawVerticalLine(0, -5.2, -9.1); //ball (we wanted to add a wand form harry potter but it always gave us a segmentation fault

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	/*Floor*/
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);					
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, CBO1);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO1);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    MultiplyMatrix(RotateFloor, FloorMatrix, FloorMatrix);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, FloorMatrix);  
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//light source 2
	GLint l2AmbientUniform = glGetUniformLocation(ShaderProgram, "light.ambient_color"); //load in ambient color (from shader)
	glUniformMatrix4fv(l2AmbientUniform, 1, GL_TRUE, objectLight.ambient_color); //load in ambient color (from struct in program)

	GLint l2DiffuseUniform = glGetUniformLocation(ShaderProgram, "light.diffuse_color");
	glUniform3fv(l2DiffuseUniform, 1, objectLight.diffuse_color);
	GLint l2SpecularUniform = glGetUniformLocation(ShaderProgram, "light.specular_color");
	glUniform3fv(l2SpecularUniform, 1, objectLight.specular_color);
	GLint position2Uniform = glGetUniformLocation(ShaderProgram, "light.position");
	glUniform3fv(position2Uniform, 1, objectLight.position);
	GLint objectLightPosUniform = glGetUniformLocation(ShaderProgram, "lightPosition"); 
	glUniform3fv(objectLightPosUniform, 1, objectLight.position);

	//for light source 2
	GLint m2AmbientUniform = glGetUniformLocation(ShaderProgram, "material.ambient_color");
	glUniformMatrix4fv(m2AmbientUniform, 1, GL_TRUE, objectMaterial.ambient_color);
	GLint m2DiffuseUniform = glGetUniformLocation(ShaderProgram, "material.diffuse_color");
	glUniform3fv(m2DiffuseUniform, 1, objectMaterial.diffuse_color);
	GLint m2SpecularUniform = glGetUniformLocation(ShaderProgram, "material.specular_color");
	glUniform3fv(m2SpecularUniform, 1, objectMaterial.specular_color);
	GLint m2ShininessUniform = glGetUniformLocation(ShaderProgram, "material.specular_shininess");
	glUniform1f(m2ShininessUniform, objectMaterial.specular_shininess);

	glBindBuffer(GL_ARRAY_BUFFER, VBOBall); //object files
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind buffer with index data of currently active object -- ball */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOBall);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	MultiplyMatrix(RotationMatrixAnim, BallMatrix, BallMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, BallMatrix);  
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHeli); //object files
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind buffer with index data of currently active object -- heli */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOHeli);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    
    MultiplyMatrix(RotateHeli, HelicopterMatrix, HelicopterMatrix);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, HelicopterMatrix);  
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOBB8); //object files
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind buffer with index data of currently active object -- BB8 */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOBB8);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

    MultiplyMatrix(RotateBB8, BB8Matrix, BB8Matrix);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, BB8Matrix);  
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    

	glBindBuffer(GL_ARRAY_BUFFER, VBOWand); //object files
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind buffer with index data of currently active object -- wand */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOWand);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	
    MultiplyMatrix(RotationMatrixAnim2, WandMatrix, WandMatrix);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, WandMatrix);  
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    /* Disable attributes */
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);   

    /* Swap between front and back buffer */ 
    glutSwapBuffers();
}

/******************************************************************
*
* OnIdle
*
* 
*
*******************************************************************/

void OnIdle()
{
    /*GLUT_ELAPSED_TIME depends on the system where the project is running
     * on some systems it might be slower
    */	

    float angle = switchDirection*((glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (rotationSpeed/M_PI));
    float floorAngle = switchDirection*((glutGet(GLUT_ELAPSED_TIME) / rotateWalls) * (180/M_PI))*stopIt;
    float heliAngle = switchDirection*((glutGet(GLUT_ELAPSED_TIME) /1000.0) * (rotationSpeed/M_PI));
    float bbAngle = switchDirection*((glutGet(GLUT_ELAPSED_TIME) /1000.0) * (rotationSpeed+50/M_PI));
    float figureAngle2 = switchDirection*(-(glutGet(GLUT_ELAPSED_TIME) /1000.0) * (rotationSpeed/M_PI));

    if(anim) {

	    /* Time dependent rotation */
	    SetRotationY(angle, RotationMatrixAnim);
	    SetRotationY(floorAngle, RotateFloor);
		SetRotationY(heliAngle, RotateHeli);
		SetRotationY(bbAngle, RotateBB8);
	    SetRotationY(figureAngle2, RotationMatrixAnim2);
    }


    /* Apply model rotation; finally move cube down */
    MultiplyMatrix(RotationMatrixAnim, InitialTransform, ModelMatrix);
    MultiplyMatrix(TranslateDown, ModelMatrix, ModelMatrix);

    MultiplyMatrix(RotateFloor, InitialTransform, FloorMatrix);
    MultiplyMatrix(TranslateFloor, FloorMatrix, FloorMatrix);
    
    MultiplyMatrix(RotationMatrixAnim,InitialTransformCube, BallMatrix);
    MultiplyMatrix(TranslateLeft, BallMatrix, BallMatrix);
    
	MultiplyMatrix(RotateHeli,InitialTransformHeli, HelicopterMatrix);
	MultiplyMatrix(RotationMatrixAnim, InitialTransformHeli, HelicopterMatrix);
    MultiplyMatrix(TranslateRight, HelicopterMatrix, HelicopterMatrix);

	MultiplyMatrix(RotateBB8,InitialTransformBB8, BB8Matrix);
    MultiplyMatrix(RotationMatrixAnim,InitialTransformBB8, BB8Matrix);
    MultiplyMatrix(TranslateMiddle, BB8Matrix, BB8Matrix);

    MultiplyMatrix(RotationMatrixAnim2, InitialTransformCube2, WandMatrix);
    MultiplyMatrix(TranslateLowest, WandMatrix, WandMatrix);

    /* Request redrawing for of window content */  
    glutPostRedisplay();
}


/******************************************************************
*
* SetupDataBuffers
*
* Create buffer objects and load data into buffers
*
*******************************************************************/

void SetupDataBuffers()
{
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data3), vertex_buffer_data3, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data3), index_buffer_data3, GL_STATIC_DRAW);

    glGenBuffers(1, &CBO);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data3), color_buffer_data3, GL_STATIC_DRAW);
    
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data2), vertex_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data2), index_buffer_data2, GL_STATIC_DRAW);

    glGenBuffers(1, &CBO1);
    glBindBuffer(GL_ARRAY_BUFFER, CBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data2), color_buffer_data2, GL_STATIC_DRAW);
    

/******************************************************* -- Ball -- *******************************************************/

	glGenBuffers(1, &VBOBall);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBall);
	glBufferData(GL_ARRAY_BUFFER, dataBall.vertex_count*3*sizeof(GLfloat), vertex_buffer_dataBall, GL_STATIC_DRAW);

	glGenBuffers(1, &IBOBall);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOBall);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataBall.face_count*3*sizeof(GLushort), index_buffer_dataBall, GL_STATIC_DRAW);


/****************************************************** -- Heli -- ********************************************************/

	glGenBuffers(1, &VBOHeli);
	glBindBuffer(GL_ARRAY_BUFFER, VBOHeli);
	glBufferData(GL_ARRAY_BUFFER, dataHeli.vertex_count*3*sizeof(GLfloat), vertex_buffer_dataHeli, GL_STATIC_DRAW);

	glGenBuffers(1, &IBOHeli);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOHeli);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataHeli.face_count*3*sizeof(GLushort), index_buffer_dataHeli, GL_STATIC_DRAW);

/****************************************************** -- BB8 -- ********************************************************/

	glGenBuffers(1, &VBOBB8);
	glBindBuffer(GL_ARRAY_BUFFER, VBOBB8);
	glBufferData(GL_ARRAY_BUFFER, dataBB8.vertex_count*3*sizeof(GLfloat), vertex_buffer_dataBB8, GL_STATIC_DRAW);

	glGenBuffers(1, &IBOBB8);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOBB8);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataBB8.face_count*3*sizeof(GLushort), index_buffer_dataBB8, GL_STATIC_DRAW);

/****************************************************** -- Wand -- *********************************************************/

	glGenBuffers(1, &VBOWand);
	glBindBuffer(GL_ARRAY_BUFFER, VBOWand);
	glBufferData(GL_ARRAY_BUFFER, dataWand.vertex_count*3*sizeof(GLfloat), vertex_buffer_dataWand, GL_STATIC_DRAW);

	glGenBuffers(1, &IBOWand);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOWand);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataWand.face_count*3*sizeof(GLushort), index_buffer_dataWand, GL_STATIC_DRAW);
}


/******************************************************************
*
* AddShader
*
* This function creates and adds individual shaders
*
*******************************************************************/

void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType)
{
    /* Create shader object */
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) 
    {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    /* Associate shader source code string with shader object */
    glShaderSource(ShaderObj, 1, &ShaderCode, NULL);

    GLint success = 0;
    GLchar InfoLog[1024];

    /* Compile shader source code */
    glCompileShader(ShaderObj);
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) 
    {
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    /* Associate shader with shader program */
    glAttachShader(ShaderProgram, ShaderObj);
}


/******************************************************************
*
* CreateShaderProgram
*
* This function creates the shader program; vertex and fragment
* shaders are loaded and linked into program; final shader program
* is put into the rendering pipeline 
*
*******************************************************************/

void CreateShaderProgram()
{
    /* Allocate shader object */
    ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0) 
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    /* Load shader code from file */
    VertexShaderString = LoadShader("shaders/vertexshader_phong.vs");
    FragmentShaderString = LoadShader("shaders/fragmentshader_phong.fs");

    /* Separately add vertex and fragment shader to program */
    AddShader(ShaderProgram, VertexShaderString, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, FragmentShaderString, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024];

    /* Link shader code into executable shader program */
    glLinkProgram(ShaderProgram);

    /* Check results of linking step */
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

    if (Success == 0) 
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Check if shader program can be executed */ 
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);

    if (!Success) 
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    /* Put linked shader program into drawing pipeline */
    glUseProgram(ShaderProgram);
}


/******************************************************************
*
* Initialize
*
* This function is called to initialize rendering elements, setup
* vertex buffer objects, and to setup the vertex and fragment shader
*
*******************************************************************/

void Initialize(void)
{   
	int i;
	int success;

	/* Load first OBJ model */
	char* filename1 = "models/ball.obj"; 
	success = parse_obj_scene(&dataBall, filename1);

	if(!success)
		printf("Could not load ball file. Exiting.\n");

	/*  Copy mesh data from structs into appropriate arrays -- load Ball file */ 
	int vert = dataBall.vertex_count;
	int indx = dataBall.face_count;

	vertex_buffer_dataBall = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
	index_buffer_dataBall = (GLushort*) calloc (indx*3, sizeof(GLushort));

	/* Vertices */
	for(i=0; i<vert; i++) {
		vertex_buffer_dataBall[i*3] = (GLfloat)(*dataBall.vertex_list[i]).e[0];
		vertex_buffer_dataBall[i*3+1] = (GLfloat)(*dataBall.vertex_list[i]).e[1];
		vertex_buffer_dataBall[i*3+2] = (GLfloat)(*dataBall.vertex_list[i]).e[2];
	}

	/* Indices */
	for(i=0; i<indx; i++) {
		index_buffer_dataBall[i*3] = (GLushort)(*dataBall.face_list[i]).vertex_index[0];
		index_buffer_dataBall[i*3+1] = (GLushort)(*dataBall.face_list[i]).vertex_index[1];
		index_buffer_dataBall[i*3+2] = (GLushort)(*dataBall.face_list[i]).vertex_index[2];
	}

	char* filename2 = "models/heli.obj"; 
	success = parse_obj_scene(&dataHeli, filename2);

	if(!success)
		printf("Could not load heli file. Exiting.\n");

	/*  Copy mesh data from structs into appropriate arrays -- load heli file */ 
	vert = dataHeli.vertex_count;
	indx = dataHeli.face_count;

	vertex_buffer_dataHeli = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
	index_buffer_dataHeli = (GLushort*) calloc (indx*3, sizeof(GLushort));

	/* Vertices */
	for(i=0; i<vert; i++) {
		vertex_buffer_dataHeli[i*3] = (GLfloat)(*dataHeli.vertex_list[i]).e[0];
		vertex_buffer_dataHeli[i*3+1] = (GLfloat)(*dataHeli.vertex_list[i]).e[1];
		vertex_buffer_dataHeli[i*3+2] = (GLfloat)(*dataHeli.vertex_list[i]).e[2];
	}

	/* Indices */
	for(i=0; i<indx; i++) {
		index_buffer_dataHeli[i*3] = (GLushort)(*dataHeli.face_list[i]).vertex_index[0];
		index_buffer_dataHeli[i*3+1] = (GLushort)(*dataHeli.face_list[i]).vertex_index[1];
		index_buffer_dataHeli[i*3+2] = (GLushort)(*dataHeli.face_list[i]).vertex_index[2];
	}

	char* filename3 = "models/BB8/bb8.obj"; 
	success = parse_obj_scene(&dataBB8, filename3);

	if(!success)
		printf("Could not load BB8 file. Exiting.\n");

	/*  Copy mesh data from structs into appropriate arrays -- load BB8 file */ 
	vert = dataBB8.vertex_count;
	indx = dataBB8.face_count;

	vertex_buffer_dataBB8 = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
	index_buffer_dataBB8 = (GLushort*) calloc (indx*3, sizeof(GLushort));

	/* Vertices */
	for(i=0; i<vert; i++) {
		vertex_buffer_dataBB8[i*3] = (GLfloat)(*dataBB8.vertex_list[i]).e[0];
		vertex_buffer_dataBB8[i*3+1] = (GLfloat)(*dataBB8.vertex_list[i]).e[1];
		vertex_buffer_dataBB8[i*3+2] = (GLfloat)(*dataBB8.vertex_list[i]).e[2];
	}

	/* Indices */
	for(i=0; i<indx; i++) {
		index_buffer_dataBB8[i*3] = (GLushort)(*dataBB8.face_list[i]).vertex_index[0];
		index_buffer_dataBB8[i*3+1] = (GLushort)(*dataBB8.face_list[i]).vertex_index[1];
		index_buffer_dataBB8[i*3+2] = (GLushort)(*dataBB8.face_list[i]).vertex_index[2];
	}

char* filename4 = "models/ball.obj"; 
	success = parse_obj_scene(&dataWand, filename4);

	if(!success)
		printf("Could not load heli file. Exiting.\n");

	/*  Copy mesh data from structs into appropriate arrays -- load heli file */ 
	vert = dataWand.vertex_count;
	indx = dataWand.face_count;

	vertex_buffer_dataWand = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
	index_buffer_dataWand = (GLushort*) calloc (indx*3, sizeof(GLushort));

	/* Vertices */
	for(i=0; i<vert; i++) {
		vertex_buffer_dataWand[i*3] = (GLfloat)(*dataWand.vertex_list[i]).e[0];
		vertex_buffer_dataWand[i*3+1] = (GLfloat)(*dataWand.vertex_list[i]).e[1];
		vertex_buffer_dataWand[i*3+2] = (GLfloat)(*dataWand.vertex_list[i]).e[2];
	}

	/* Indices */
	for(i=0; i<indx; i++) {
		index_buffer_dataWand[i*3] = (GLushort)(*dataWand.face_list[i]).vertex_index[0];
		index_buffer_dataWand[i*3+1] = (GLushort)(*dataWand.face_list[i]).vertex_index[1];
		index_buffer_dataWand[i*3+2] = (GLushort)(*dataWand.face_list[i]).vertex_index[2];
	}

    /* Set background (clear) color to black */ 
    glClearColor(1.0, 1.0, 1.0, 1.0);

	/* Enable depth testing */
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);    
	glEnable(GL_LIGHTING);

    /* Setup vertex, color, and index buffer objects */
    SetupDataBuffers();

    /* Setup shaders and shader program */
    CreateShaderProgram();  

    /* Initialize matrices */
    SetIdentityMatrix(ProjectionMatrix);
    SetIdentityMatrix(ViewMatrix);
    SetIdentityMatrix(ModelMatrix);
    SetIdentityMatrix(BallMatrix);
    SetIdentityMatrix(FloorMatrix);

    /* Set projection transform */
    float fovy = 45.0;
    float aspect = 1.0; 
    float nearPlane = 1.0; 
    float farPlane = 50.0;
    SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);

    /* Set viewing transform */
    SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);

    /* Translate and rotate cube onto tip */
    SetTranslation(0, 0, 0, TranslateOrigin);
    SetRotationX(0, RotateX);
    SetRotationZ(0, RotateZ);	

    /* Translate down */	
    SetTranslation(0, sqrtf(sqrtf(90)+1), 0, TranslateDown);

    
    /* Initial transformation matrix */
    MultiplyMatrix(RotateX, TranslateOrigin, InitialTransform);
    MultiplyMatrix(RotateZ, InitialTransform, InitialTransform);

    float tmp5[16];
    float tmp_x5[16];
    float tmp_z5[16];    
    
    SetTranslation(0, 0, 0, tmp5);
    SetRotationX(0, tmp_x5);
    SetRotationZ(0, tmp_z5);	

    /* Translation for Floor and walls */	
    SetTranslation(0, -sqrtf(sqrtf(2.0) * 1.0)-10, 0, TranslateFloor);
    MultiplyMatrix(tmp_x5, tmp5, InitialTransform);
    MultiplyMatrix(tmp_z5, InitialTransform, InitialTransform);

	/* for lighting - initialize light */
	//light source 1
	material.ambient_color[0] = (246.0f/255);
	material.ambient_color[1] = (224.0f/255);
	material.ambient_color[2] = (23.0f/255);

	//model color is now yellow because of diffuse light set to yellow
	material.diffuse_color[0] = (246.0f/255);
	material.diffuse_color[1] = (224.0f/255);
	material.diffuse_color[2] = (23.0f/255);

	material.specular_color[0] = 1.0f;
	material.specular_color[1] = 1.0f;
	material.specular_color[2] = 1.0f;

	material.specular_shininess= 15.0f;


	//should be the same as diffuse light color
	light.ambient_color[0] = (246.0f/255);
	light.ambient_color[1] = (224.0f/255);
	light.ambient_color[2] = (23.0f/255);

	//model color is now yellow because of diffuse light set to yellow
	light.diffuse_color[0] = (246.0f/255);
	light.diffuse_color[1] = (224.0f/255);
	light.diffuse_color[2] = (23.0f/255);

	light.specular_color[0] = 1.0f;
	light.specular_color[1] = 1.0f;
	light.specular_color[2] = 1.0f;

	//positioned in roor turns with background wall/floor)
	light.position[0] = -7.0f;
	light.position[1] = 2.0f;
	light.position[2] = -7.0f;


	/**********************************************************************
	* tmp variables are needed to translate all objects to the center	  *
	* instead of defining more global variables we decided to create	  *
	* them locally in this scope										  *
	**********************************************************************/

    float tmp[16];
    float tmp_x[16];
    
    SetTranslation(0, 0, 0, tmp);
    SetRotationX(180, tmp_x);	

    /* Translate cube to lefthand side -- ball */	
    SetTranslation(-3, -sqrtf(sqrtf(2.0) * 1.0)+2, 0, TranslateLeft);
    
    MultiplyMatrix(tmp_x, tmp, InitialTransformCube);

    float tmp1[16];
    float tmp_x1[16];
    
    SetTranslation(0, 0, 0, tmp1);
    SetRotationX(0, tmp_x1);	

    /*Scale and Translate object to righthand side -- helicopter */	
	SetScaling(0.5, 0.5, 0.5, tmp1);
    SetTranslation(3.5, -sqrtf(sqrtf(2.0) * 1.0)+0, 0, TranslateRight);
	MultiplyMatrix(HelicopterMatrix, TranslateRight, HelicopterMatrix);
    MultiplyMatrix(tmp_x1, tmp1, InitialTransformHeli);

    float tmp3[16];
    float tmp_x3[16];   
    
    SetTranslation(0, 0, 0, tmp3);
    SetRotationX(0, tmp_x3);	

    /* Translate cube to middle and scale it -- BB8 */
	SetScaling(0.0125, 0.0125, 0.0125, tmp3);	
    SetTranslation(0, -sqrtf(sqrtf(2.0) * 1.0)-1, 0, TranslateMiddle);
	MultiplyMatrix(BB8Matrix, TranslateMiddle, BB8Matrix);
    MultiplyMatrix(tmp_x3, tmp3, InitialTransformBB8);

    float tmp4[16];
    float tmp_x4[16];    
    
    SetTranslation(0, 0, 0, tmp4);
    SetRotationX(180, tmp_x4);	

    /* Translate cube to middle and scale it */	
    SetTranslation(0, -sqrtf(sqrtf(2.0) * 1.0)-5, 0, TranslateLowest);
    MultiplyMatrix(tmp_x4, tmp4, InitialTransformCube2);


	//light source 2
	objectMaterial.ambient_color[0] = (184.0f/255);
	objectMaterial.ambient_color[1] = (151.0f/255);
	objectMaterial.ambient_color[2] = (206.0f/255);

	//model color is now violet because of diffuse light set to violet
	objectMaterial.diffuse_color[0] = (184.0f/255);
	objectMaterial.diffuse_color[1] = (151.0f/255);
	objectMaterial.diffuse_color[2] = (206.0f/255);

	objectMaterial.specular_color[0] = 1.0f;
	objectMaterial.specular_color[1] = 1.0f;
	objectMaterial.specular_color[2] = 1.0f;

	objectMaterial.specular_shininess= 5.0f;


	//should be the same as diffuse light color
	objectLight.ambient_color[0] = (184.0f/255);
	objectLight.ambient_color[1] = (151.0f/255);
	objectLight.ambient_color[2] = (206.0f/255);

	//model color is now violet because of diffuse light set to violet
	objectLight.diffuse_color[0] = (184.0f/255);
	objectLight.diffuse_color[1] = (151.0f/255);
	objectLight.diffuse_color[2] = (206.0f/255);

	objectLight.specular_color[0] = 1.0f;
	objectLight.specular_color[1] = 1.0f;
	objectLight.specular_color[2] = 1.0f;

	//is positioned in the right front of our model, moves with merrygoround but should not 
	objectLight.position[0] = 10.0f;
	objectLight.position[1] = 1.0f;
	objectLight.position[2] = 10.0f;
}


/******************************************************************
*
* Keyboard - changed the keys in the example given in the Prosemiar
*
* Function to be called on key press in window; set by
* glutKeyboardFunc(); x and y specify mouse position on keypress;
* not used in this example 
*
*******************************************************************/

void Keyboard(unsigned char key, int x, int y)   
{
	if(key == 'n') {
		automaticMode = GL_TRUE;
		anim = GL_TRUE;
		stopIt = 1.0;
		rotateWalls = 1200.0;
		switchDirection = 1;
		rotationSpeed = 180.0f;
		cameraDisp = -30.0;
		cameraPosition = 0.0;
		SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
	}
	else if(key == '1') {
    	normalMode = GL_TRUE;
		automaticMode = GL_FALSE;
		stopIt = 0.0;
		anim = GL_TRUE;
		switchDirection = 1;
		rotationSpeed = 180.0f;
		cameraDisp = -30.0;
		cameraPosition = 0.0;
		SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
 	}
 	else if(key == '2') {
    	normalMode = GL_FALSE;
		automaticMode = GL_FALSE;
		stopIt = 0.0;
		anim = GL_TRUE;
		switchDirection = 1;
		rotationSpeed = 180.0f;
		cameraDisp = -30.0;
		cameraPosition = 0.0;
		SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
	}
 
	if(normalMode && (!automaticMode)) {
		switch( key ) 
		{
		/* Reset initial rotation of object */
		case 'o':
			anim = GL_TRUE;
			switchDirection = 1;
			rotationSpeed = 180.0f;
			cameraDisp = -30.0;
			cameraPosition = 0.0;
			SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
			break;
		
		    case 's':
			switchDirection *= -1;
			break;

		case 'p':
			if(anim) {
			anim = GL_FALSE;
			}
			else {
			anim = GL_TRUE;
			}
			break;

		    case '+':
		        if(rotationSpeed < 500.0f) {
					rotationSpeed+=10;
				}
			break;
		case '-':
			if(rotationSpeed > 10.0f) {
				rotationSpeed-=10;
			}
			else {
			rotationSpeed = 0.0f;
			}
			break;

		case 'n':
			automaticMode = GL_TRUE;
			break;

		case 'q': case 'Q':  
			exit(0);    
			break;
		}
	}
	else if((!normalMode) && (!automaticMode)) {
		switch(key) {
		case 'w':
			if(cameraDisp < -10) {
				cameraDisp += 2.5;
				SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
			}
			break;

		case 's':
			if(cameraDisp > -40) {
				cameraDisp -= 2.5;
				SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
			}
			break;

		case 'a':
			if(cameraPosition < 20) {
				cameraPosition += 2.5;
				SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
			}
			break;

		case 'd':
			if(cameraPosition > -20) {
				cameraPosition -= 2.5;	
				SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
			}
			break;

		case 'o':
			anim = GL_TRUE;
			switchDirection = 1;
			rotationSpeed = 180.0f;
			cameraDisp = -30.0;
			cameraPosition = 0.0;
			SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
			break;

		case 'n':
			automaticMode = GL_TRUE;
			break;

		case 'q': case 'Q':  
			exit(0);    
			break;
		}
	}

    glutPostRedisplay();
}

/******************************************************************
*
* Mouse - was given to us in the Proseminar
*
* Function is called on mouse button press; has been seta
* with glutMouseFunc(), x and y specify mouse coordinates,
* but are not used here.
*
*******************************************************************/

void Mouse(int button, int state, int x, int y) 
{

	if(!normalMode) {
		if(state == GLUT_DOWN) {
		  /* Depending on button pressed, set rotation axis,
		   * turn on animation */
		    switch(button) {
			case GLUT_LEFT_BUTTON:    
			    if(rotationSpeed < 500.0f) {
					rotationSpeed+=10;
				}
			break;

			case GLUT_MIDDLE_BUTTON:  
	  	        switchDirection *= -1;
			    break;
		
			case GLUT_RIGHT_BUTTON: 
			    if(rotationSpeed > 10.0f) {
					rotationSpeed-=10;
				}
				else {
				rotationSpeed = 0.0f;
				}
			break;
			}
		}
	}
}

/******************************************************************
*
* main
*
* Main function to setup GLUT, GLEW, and enter rendering loop
*
*******************************************************************/

int main(int argc, char** argv)
{
    /* Initialize GLUT; set double buffered window and RGBA color model */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(400, 400);
    glutCreateWindow("CG Proseminar - Mobile");

    /* Initialize GL extension wrangler */
    GLenum res = glewInit();
    if (res != GLEW_OK) 
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    /* Setup scene and rendering parameters */
    Initialize();


    /* Specify callback functions;enter GLUT event processing loop, 
     * handing control over to GLUT */
    glutIdleFunc(OnIdle);
    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
    glutMainLoop();

    /* ISO C requires main to return int */
    return 0;
}
