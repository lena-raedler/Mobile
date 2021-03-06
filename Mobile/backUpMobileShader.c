/*********************************************************************
*
*Mobile_Shader.c
*This is an implementation of a Mobile
*
*
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
GLuint VBO, VBO1, VBO2, VBOBall, VBOHeli, VBOHeart, VBOWand;

/* Define handle to a color buffer object */
GLuint CBO, CBO1, CBO2; 

/* Define handle to an index buffer object */
GLuint IBO, IBO1, IBO2, IBOBall, IBOHeli, IBOHeart, IBOWand;

/* Arrays for holding vertex data of the models */
GLfloat *vertex_buffer_dataBall;
GLfloat *vertex_buffer_dataHeli;
GLfloat *vertex_buffer_dataHeart;
GLfloat *vertex_buffer_dataWand;

/* Arrays for holding indices of the models */
GLushort *index_buffer_dataBall;
GLushort *index_buffer_dataHeli;
GLushort *index_buffer_dataHeart;
GLushort *index_buffer_dataWand;

/* Structures for loading of OBJ data */
obj_scene_data dataBall;
obj_scene_data dataHeli;
obj_scene_data dataHeart;
obj_scene_data dataWand;

/* Indices to vertex attributes; in this case positon and color */ 
enum DataID {vPosition = 0, vColor = 1}; 

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */ 
float ModelMatrix[16]; /* Big Green Bar matrix */ 
float BallMatrix[16]; /*for the rotating cube - ball.obj*/
float HelicopterMatrix[16]; /*for the second cube - helicopter */
float HeartMatrix[16]; /*for the tiny cube in the middle - heart*/
float WandMatrix[16]; /*for cube hanging on the lowest in the middle - wand*/
float FloorMatrix[16]; /*for floor and walls*/

float LeftBarMatrix[16];
float RightBarMatrix[16];

/* Transformation matrices for initial position */
float TranslateOrigin[16];
float TranslateDown[16];
float RotateX[16];
float RotateZ[16];
float InitialTransform[16];
float RotationMatrix[16];
float RotationMatrixAnim[16]; /*for the rotating cubes*/
float RotationMatrixAnim2[16]; /* for cube rotatio the other way round*/
float TranslateLeft[16]; /*for ballMatrix*/
float TranslateRight[16]; /*for helicopterMatrix*/
float TranslateMiddle[16]; /*for heartMatrix*/
float TranslateLowest[16]; /*for wandMatrix*/
float TranslateFloor[16];
float InitialTransformCube[16]; /*for rotating cubes*/
float InitialTransformHeart[16];
float InitialTransformCube2[16]; /*for cube rotation the other way round*/
float RotateFloor[16];

float rotationSpeed = 180.0;
int switchDirection = 1;
GLboolean anim = GL_TRUE;
GLboolean normalMode = GL_TRUE;
float cameraDisp = -30.0;
float cameraPosition = 0.0;

/*TODO: rotationMatrix for heli; modularisieren; 2D-gekröse*/

/*green bar used as upper layer*/

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

GLfloat color_buffer_data3[] = { /* green color for roof*/
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
  
}; 

GLushort index_buffer_data3[] = { /* Indices of 6*2 triangles (10 sides) */
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

/* original implementation of RotatingCube */

GLfloat vertex_buffer_data[] = { /* 8 cube vertices XYZ */
    -1.0, -1.0,  1.0,
     1.0, -1.0,  1.0,
     1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    -1.0, -1.0, -1.0,
     1.0, -1.0, -1.0,
     1.0,  1.0, -1.0,
    -1.0,  1.0, -1.0,
};   

GLfloat color_buffer_data[] = { /* RGB color values for 8 vertices */
    0.0, 0.0, 1.0,
    1.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
    0.0, 1.0, 1.0,
    0.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
}; 

GLushort index_buffer_data[] = { /* Indices of 6*2 triangles (6 sides) */
    0, 1, 2,
    2, 3, 0,
    1, 5, 6,
    6, 2, 1,
    7, 6, 5,
    5, 4, 7,
    4, 0, 3,
    3, 7, 4,
    4, 5, 1,
    1, 0, 4,
    3, 2, 6,
    6, 7, 3,
};

GLfloat vertex_buffer_data2[] = {
	9.0, 0.0, -9.0,
	-9.0, 0.0, -9.0,
	-9.0, 0.0, 9.0,
	9.0, 0.0, 9.0,
	9.0, 18.0, -9.0,
	-9.0,18.0, -9.0,
	-9.0, 18.0, 9.0,
};

GLfloat color_buffer_data2[] = {
	1.0, 0.1255, 0.451,
	1.0, 0.1255, 0.451,
	1.0, 0.1255, 0.451,
	1.0, 0.1255, 0.451,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
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

void Display()
{
    /* Clear window; color specified in 'Initialize()' */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //drawRoom();

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
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);  

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);					
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, CBO2);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	

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
    
	SetScaling(0.5, 0.5, 0.5, HelicopterMatrix);
	MultiplyMatrix(HelicopterMatrix, TranslateRight, HelicopterMatrix);
    MultiplyMatrix(RotationMatrixAnim, HelicopterMatrix, HelicopterMatrix);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, HelicopterMatrix);  
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHeart); //object files
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind buffer with index data of currently active object -- heart */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOHeart);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	SetScaling(0.0125, 0.0125, 0.0125, HeartMatrix);
	MultiplyMatrix(HeartMatrix, TranslateMiddle, HeartMatrix);
    MultiplyMatrix(RotationMatrixAnim, HeartMatrix, HeartMatrix);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, HeartMatrix);  
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    

	glBindBuffer(GL_ARRAY_BUFFER, VBOWand); //object files
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind buffer with index data of currently active object -- wand */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOWand);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	
    MultiplyMatrix(RotationMatrixAnim2, WandMatrix, WandMatrix);
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, WandMatrix);  
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
    float floorAngle = switchDirection*((glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (0/M_PI));
    float figureAngle = switchDirection*((glutGet(GLUT_ELAPSED_TIME) /1000.0) * (rotationSpeed/M_PI));
    float figureAngle2 = switchDirection*(-(glutGet(GLUT_ELAPSED_TIME) /1000.0) * (rotationSpeed/M_PI));

    if(anim) {

	    /* Time dependent rotation */
	    SetRotationY(angle, RotationMatrixAnim);
	    SetRotationY(floorAngle, RotateFloor);
	    SetRotationY(figureAngle, RotationMatrixAnim);
	    SetRotationY(figureAngle2, RotationMatrixAnim2);
    }


    /* Apply model rotation; finally move cube down */
    MultiplyMatrix(RotationMatrixAnim, InitialTransform, ModelMatrix);
    MultiplyMatrix(TranslateDown, ModelMatrix, ModelMatrix);

    MultiplyMatrix(RotateFloor, InitialTransform, FloorMatrix);
    MultiplyMatrix(TranslateFloor, FloorMatrix, FloorMatrix);
    
    MultiplyMatrix(RotationMatrixAnim,InitialTransformCube, BallMatrix);
    MultiplyMatrix(TranslateLeft, BallMatrix, BallMatrix);

    MultiplyMatrix(RotationMatrixAnim,InitialTransformCube, HelicopterMatrix);
    MultiplyMatrix(TranslateRight, HelicopterMatrix, HelicopterMatrix);

    MultiplyMatrix(RotationMatrixAnim,InitialTransformHeart, HeartMatrix);
    MultiplyMatrix(TranslateMiddle, HeartMatrix, HeartMatrix);

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
    

    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
    
    glGenBuffers(1, &IBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);
    
    glGenBuffers(1, &CBO2);
    glBindBuffer(GL_ARRAY_BUFFER, CBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

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

/****************************************************** -- Heart -- ********************************************************/

	glGenBuffers(1, &VBOHeart);
	glBindBuffer(GL_ARRAY_BUFFER, VBOHeart);
	glBufferData(GL_ARRAY_BUFFER, dataHeart.vertex_count*3*sizeof(GLfloat), vertex_buffer_dataHeart, GL_STATIC_DRAW);

	glGenBuffers(1, &IBOHeart);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOHeart);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataHeart.face_count*3*sizeof(GLushort), index_buffer_dataHeart, GL_STATIC_DRAW);

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
    VertexShaderString = LoadShader("shaders/vertexshader.vs");
    FragmentShaderString = LoadShader("shaders/fragmentshader.fs");

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

	char* filename3 = "models/Heart.obj"; 
	success = parse_obj_scene(&dataHeart, filename3);

	if(!success)
		printf("Could not load heart file. Exiting.\n");

	/*  Copy mesh data from structs into appropriate arrays -- load heart file */ 
	vert = dataHeart.vertex_count;
	indx = dataHeart.face_count;

	vertex_buffer_dataHeart = (GLfloat*) calloc (vert*3, sizeof(GLfloat));
	index_buffer_dataHeart = (GLushort*) calloc (indx*3, sizeof(GLushort));

	/* Vertices */
	for(i=0; i<vert; i++) {
		vertex_buffer_dataHeart[i*3] = (GLfloat)(*dataHeart.vertex_list[i]).e[0];
		vertex_buffer_dataHeart[i*3+1] = (GLfloat)(*dataHeart.vertex_list[i]).e[1];
		vertex_buffer_dataHeart[i*3+2] = (GLfloat)(*dataHeart.vertex_list[i]).e[2];
	}

	/* Indices */
	for(i=0; i<indx; i++) {
		index_buffer_dataHeart[i*3] = (GLushort)(*dataHeart.face_list[i]).vertex_index[0];
		index_buffer_dataHeart[i*3+1] = (GLushort)(*dataHeart.face_list[i]).vertex_index[1];
		index_buffer_dataHeart[i*3+2] = (GLushort)(*dataHeart.face_list[i]).vertex_index[2];
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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);    

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
    SetTranslation(0, 8, 0, TranslateDown);

    
    /* Initial transformation matrix */
    MultiplyMatrix(RotateX, TranslateOrigin, InitialTransform);
    MultiplyMatrix(RotateZ, InitialTransform, InitialTransform);

    float tmp[16];
    float tmp_x[16];
    float tmp_z[16];
    
    SetTranslation(0, 0, 0, tmp);
    SetRotationX(0, tmp_x);
    SetRotationZ(45, tmp_z);	

    /* Translate cube to lefthand side -- ball */	
    SetTranslation(-3, -sqrtf(sqrtf(2.0) * 1.0)+2, 0, TranslateLeft);
    
    MultiplyMatrix(tmp_x, tmp, InitialTransformCube);
    MultiplyMatrix(tmp_z, InitialTransformCube, InitialTransformCube);

    SetTranslation(0, 0, 0, tmp);
    SetRotationX(0, tmp_x);
    SetRotationZ(45, tmp_z);	

    /* Translate cube to righthand side -- helicopter */	
    SetTranslation(8, -sqrtf(sqrtf(2.0) * 1.0)+0, 0, TranslateRight);
    
    MultiplyMatrix(tmp_x, tmp, InitialTransformCube);
    MultiplyMatrix(tmp_z, InitialTransformCube, InitialTransformCube);

    float tmp3[16];
    float tmp_x3[16];
    float tmp_z3[16];    
    
    SetTranslation(0, 0, 0, tmp3);
    SetRotationX(0, tmp_x3);
    SetRotationZ(0, tmp_z3);	

    /* Translate cube to middle and scale it -- heart */	
    SetTranslation(-150, -sqrtf(sqrtf(2.0) * 1.0)-250, 0, TranslateMiddle);
    MultiplyMatrix(tmp_x3, tmp3, InitialTransformHeart);
    MultiplyMatrix(tmp_z3, InitialTransformHeart, InitialTransformHeart);

    float tmp4[16];
    float tmp_x4[16];
    float tmp_z4[16];    
    
    SetTranslation(0, 0, 0, tmp4);
    SetRotationX(45, tmp_x4);
    SetRotationZ(-35, tmp_z4);	

    /* Translate cube to middle and scale it */	
    SetTranslation(0, -sqrtf(sqrtf(2.0) * 1.0)-5, 0, TranslateLowest);
    MultiplyMatrix(tmp_x4, tmp4, InitialTransformCube2);
    MultiplyMatrix(tmp_z4, InitialTransformCube2, InitialTransformCube2);


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

}


/******************************************************************
*
* Keyboard
*
* Function to be called on key press in window; set by
* glutKeyboardFunc(); x and y specify mouse position on keypress;
* not used in this example 
*
*******************************************************************/

void Keyboard(unsigned char key, int x, int y)   
{
	if(key == '1') {
    	normalMode = GL_TRUE;
		anim = GL_TRUE;
		switchDirection = 1;
		rotationSpeed = 180.0f;
		cameraDisp = -30.0;
		cameraPosition = 0.0;
		SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
 	}
 	else if(key == '2') {
    	normalMode = GL_FALSE;
		anim = GL_TRUE;
		switchDirection = 1;
		rotationSpeed = 180.0f;
		cameraDisp = -30.0;
		cameraPosition = 0.0;
		SetTranslation(cameraPosition, 0.0, cameraDisp, ViewMatrix);
	}
 
	if(normalMode) {
		switch( key ) 
		{
		/* Reset initial rotation of object */
		case 'o':
		   /* SetIdentityMatrix(RotationMatrixAnimX);
			SetIdentityMatrix(RotationMatrixAnimY);
			SetIdentityMatrix(RotationMatrixAnimZ);
			angleX = 0.0;
			angleY = 0.0;
			angleZ = 0.0;*/
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
			
		case 'q': case 'Q':  
			exit(0);    
			break;
		}
	}
	else {
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

		case 'q': case 'Q':  
			exit(0);    
			break;
		}
	}

    glutPostRedisplay();
}

/******************************************************************
*
* Mouse
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
