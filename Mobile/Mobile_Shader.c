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

/* Define handle to a vertex buffer object */
GLuint VBO, VBO2;

/* Define handle to a color buffer object */
GLuint CBO, CBO2; 

/* Define handle to an index buffer object */
GLuint IBO, IBO2;


/* Indices to vertex attributes; in this case positon and color */ 
enum DataID {vPosition = 0, vColor = 1}; 

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */ 
float ModelMatrix[16]; /* Model matrix */ 
float FigureMatrix[16]; /*for the rotating cube*/
float FigureMatrix2[16]; /*for the second cube */
float FigureMatrix3[16]; /*for the tiny cube in the middle*/
float FigureMatrix4[16]; /*for cube hanging on the lowest in the middle*/

/* Transformation matrices for initial position */
float TranslateOrigin[16];
float TranslateDown[16];
float RotateX[16];
float RotateZ[16];
float InitialTransform[16];
float RotationMatrix[16];
float RotationMatrixAnim[16]; /*for the rotating cubes*/
float RotationMatrixAnim2[16];
float TranslateLeft[16];
float TranslateRight[16];
float TranslateMiddle[16];
float TranslateLowest[16];
float InitialTransformCube[16];
float InitialTransformCube2[16];


/*green octangle used as upper layer*/

GLfloat vertex_buffer_data3[] = {
    -3, 1, 3,
     3, 1, 3,
     3, 1, -3,
    -3, 1, -3,
    -3, 1.1, 3,
     3, 1.1, 3,
     3, 1.1, -3,
    -3, 1.1, -3,
     4, 1, 0,
     4, 1.1, 0,
    -4, 1, 0,
    -4, 1.1, 0,
     0, 1, 4,
     0, 1.1, 4,
     0, 1, -4,
     0, 1.1, -4,
};   

GLfloat color_buffer_data3[] = { /* green color for roof*/
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
}; 

GLushort index_buffer_data3[] = { /* Indices of 6*2 triangles (10 sides) */
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
    5,6,9,
    1,2,8,
    2,6,8,
    6,8,9,
    1,5,9,
    1,8,9,
    4,5,13,
    6,7,15,
    4,7,11,
    2,14,15,
    2,6,15,
    3,14,15,
    3,7,15,
    3,7,10,
    7,10,11,
    0,10,11,
    0,4,11,
    0,12,13,
    0,4,13,
    1,12,13,
    1,5,13,
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
    
    MultiplyMatrix(RotationMatrixAnim, FigureMatrix, FigureMatrix);			//rotating cube
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, FigureMatrix);  
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    MultiplyMatrix(RotationMatrixAnim, FigureMatrix2, FigureMatrix2);			//rotating cube
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, FigureMatrix2);  
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    MultiplyMatrix(RotationMatrixAnim, FigureMatrix3, FigureMatrix3);			//rotating cube
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, FigureMatrix3);  
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);	

    MultiplyMatrix(RotationMatrixAnim2, FigureMatrix4, FigureMatrix4);			//rotating cube
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, FigureMatrix4);  
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
    float angle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (180.0/M_PI);
    float figureAngle = (glutGet(GLUT_ELAPSED_TIME) /1000.0) * (180.0/M_PI);
    float figureAngle2 = -(glutGet(GLUT_ELAPSED_TIME) /1000.0) * (180.0/M_PI);


    /* Time dependent rotation */
    SetRotationY(angle, RotationMatrixAnim);

    /* Apply model rotation; finally move cube down */
    MultiplyMatrix(RotationMatrixAnim, InitialTransform, ModelMatrix);
    MultiplyMatrix(TranslateDown, ModelMatrix, ModelMatrix);
    
    SetRotationY(figureAngle, RotationMatrixAnim);
    MultiplyMatrix(RotationMatrixAnim,InitialTransformCube, FigureMatrix);
    MultiplyMatrix(TranslateLeft, FigureMatrix, FigureMatrix);

    SetRotationY(figureAngle, RotationMatrixAnim);
    MultiplyMatrix(RotationMatrixAnim,InitialTransformCube, FigureMatrix2);
    MultiplyMatrix(TranslateRight, FigureMatrix2, FigureMatrix2);

    SetRotationY(figureAngle, RotationMatrixAnim);
    MultiplyMatrix(RotationMatrixAnim,InitialTransformCube, FigureMatrix3);
    MultiplyMatrix(TranslateMiddle, FigureMatrix3, FigureMatrix3);

    SetRotationY(figureAngle2, RotationMatrixAnim2);
    MultiplyMatrix(RotationMatrixAnim2, InitialTransformCube2, FigureMatrix4);
    MultiplyMatrix(TranslateLowest, FigureMatrix4, FigureMatrix4);

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
    
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
    
    glGenBuffers(1, &IBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);
    
    glGenBuffers(1, &CBO2);
    glBindBuffer(GL_ARRAY_BUFFER, CBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);
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
    VertexShaderString = LoadShader("vertexshader.vs");
    FragmentShaderString = LoadShader("fragmentshader.fs");

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

    /* Set background (clear) color to black */ 
    glClearColor(0.0, 0.0, 0.0, 0.0);

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
    SetIdentityMatrix(FigureMatrix);

    /* Set projection transform */
    float fovy = 45.0;
    float aspect = 1.0; 
    float nearPlane = 1.0; 
    float farPlane = 50.0;
    SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);

    /* Set viewing transform */
    float camera_disp = -20.0;
    SetTranslation(0.0, 0.0, camera_disp, ViewMatrix);

    /* Translate and rotate cube onto tip */
    SetTranslation(0, 0, 0, TranslateOrigin);
    SetRotationX(0, RotateX);
    SetRotationZ(0, RotateZ);	

    /* Translate down */	
    SetTranslation(0, sqrtf(sqrtf(90.0) * 1.0), 0, TranslateDown);

    
    /* Initial transformation matrix */
    MultiplyMatrix(RotateX, TranslateOrigin, InitialTransform);
    MultiplyMatrix(RotateZ, InitialTransform, InitialTransform);

    float tmp[16];
    float tmp_x[16];
    float tmp_z[16];
    
    SetTranslation(0, 0, 0, tmp);
    SetRotationX(-45, tmp_x);
    SetRotationZ(35, tmp_z);	

    /* Translate cube to lefthand side */	
    SetTranslation(-3, -sqrtf(sqrtf(2.0) * 1.0)+2, 0, TranslateLeft);
    
    MultiplyMatrix(tmp_x, tmp, InitialTransformCube);
    MultiplyMatrix(tmp_z, InitialTransformCube, InitialTransformCube);

    SetTranslation(0, 0, 0, tmp);
    SetRotationX(-45, tmp_x);
    SetRotationZ(35, tmp_z);	

    /* Translate cube to righthand side */	
    SetTranslation(3, -sqrtf(sqrtf(2.0) * 1.0)+2, 0, TranslateRight);
    
    MultiplyMatrix(tmp_x, tmp, InitialTransformCube);
    MultiplyMatrix(tmp_z, InitialTransformCube, InitialTransformCube);

    float tmp3[16];
    float tmp_x3[16];
    float tmp_z3[16];    
    
    SetTranslation(0, 0, 0, tmp3);
    SetRotationX(-45, tmp_x3);
    SetRotationZ(35, tmp_z3);	

    /* Translate cube to middle and scale it */	
    SetTranslation(0, -sqrtf(sqrtf(2.0) * 1.0)-2, 0, TranslateMiddle);
    SetScaling(0.5, 0.5, 0.5, TranslateMiddle);
    MultiplyMatrix(tmp_x3, tmp3, InitialTransformCube);
    MultiplyMatrix(tmp_z3, InitialTransformCube, InitialTransformCube);

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
    glutMainLoop();

    /* ISO C requires main to return int */
    return 0;
}
