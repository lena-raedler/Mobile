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
//#include "OBJParser.h"


/* Indices to vertex attributes; in this case positon and color */ 
enum DataID {vPosition = 0, vColor = 1}; 

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */

struct MobileObject {
	GLuint VBO;
	GLuint CBO;
	GLuint IBO;

	float ModelMatrix[16];
	float TranslateOrigin[16];
	float TranslationMatrix[16]; /*for end position*/
	float RotationMatrix[16];
	float RotateX[16];
	float RotateZ[16];
	float InitialTransform[16];	
	float RotationMatrixAnim[16]; 

	int rotationSpeed;
};

struct MobileObject BigBar;
struct MobileObject LeftBar;

GLfloat vertexBar[] = {
    4.0,0.0,0.0,
    4.0,0.0, -0.5,
    4.0, 1.0, -0.5,
    4.0, 1.0, 0.0,
    -4.0, 0.0, 0.0,
    -4.0, 0.0, -0.5,
    -4.0, 1.0, -0.5,
    -4.0, 1.0, 0.0,
};   

GLfloat colorBar[] = { /* green color for roof*/
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
   0.0, 1.0, 0.0,
  
}; 

GLushort indexBar[] = { /* Indices of 6*2 triangles (10 sides) */
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


void DisplayMobileObjectTop(struct MobileObject* object) {
	glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, object->VBO);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, object->CBO);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->IBO);
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
	

    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, object->ModelMatrix);  

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}

void DisplayMobileObjectMid(struct MobileObject* object, float factor) {
	glEnableVertexAttribArray(vPosition);
    glBindBuffer(GL_ARRAY_BUFFER, object->VBO);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(vColor);
    glBindBuffer(GL_ARRAY_BUFFER, object->CBO);
    glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, 0);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->IBO);
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
	
	
	//SetScaling(factor, factor, factor, object->ModelMatrix);
	//MultiplyMatrix(object->ModelMatrix, object->TranslationMatrix, object->ModelMatrix);
	//MultiplyMatrix(object->RotationMatrixAnim, object->ModelMatrix, object->ModelMatrix); 
	MultiplyMatrix(BigBar.RotationMatrixAnim, object->ModelMatrix, object->ModelMatrix); 
    glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, object->ModelMatrix); 

    /* Issue draw command, using indexed triangle list */
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}


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

	DisplayMobileObjectTop(&BigBar);
	DisplayMobileObjectMid(&LeftBar, 0.5);

    /* Disable attributes */
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);   

    /* Swap between front and back buffer */ 
    glutSwapBuffers();
}


void OnIdleMobileObjectTop(struct MobileObject* object) {
	float angle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (object->rotationSpeed/M_PI);

    /* Time dependent rotation */
	SetRotationY(angle, object->RotationMatrixAnim);

    /* Apply model rotation; finally move cube down */

	//MultiplyMatrix(BigBar.RotationMatrixAnim, object->ModelMatrix, object->ModelMatrix);
	//MultiplyMatrix(object->RotationMatrixAnim, object->InitialTransform, object->ModelMatrix);
	MultiplyMatrix(object->RotationMatrixAnim, object->InitialTransform, object->ModelMatrix);
	MultiplyMatrix(object->TranslationMatrix, object->ModelMatrix, object->ModelMatrix);
}

void OnIdleMobileObjectMid(struct MobileObject* object) {
	float angle = -(glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (object->rotationSpeed/M_PI);

    /* Time dependent rotation */
	SetRotationY(angle, object->RotationMatrixAnim);

    /* Apply model rotation; finally move cube down */

	//MultiplyMatrix(BigBar.RotationMatrixAnim, object->ModelMatrix, object->ModelMatrix);
	MultiplyMatrix(object->RotationMatrixAnim, object->InitialTransform, object->ModelMatrix);
	MultiplyMatrix(object->TranslationMatrix, object->ModelMatrix, object->ModelMatrix);
	MultiplyMatrix(BigBar.RotationMatrixAnim, BigBar.InitialTransform, object->ModelMatrix);
	MultiplyMatrix(object->TranslationMatrix, object->ModelMatrix, object->ModelMatrix);
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
    OnIdleMobileObjectTop(&BigBar);
	OnIdleMobileObjectMid(&LeftBar);

    /* Request redrawing forof window content */  
    glutPostRedisplay();
}


/******************************************************************
*
* SetupDataBuffers
*
* Create buffer objects and load data into buffers
*
*******************************************************************/

void SetupDataBuffers(struct MobileObject* object, GLfloat vertex[], int vertexSize, GLfloat color[], int colorSize, GLushort index[], int indexSize)
{
    glGenBuffers(1, &(object->VBO));
    glBindBuffer(GL_ARRAY_BUFFER, object->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &(object->IBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, index, GL_STATIC_DRAW);

    glGenBuffers(1, &(object->CBO));
    glBindBuffer(GL_ARRAY_BUFFER, object->CBO);
    glBufferData(GL_ARRAY_BUFFER, colorSize, color, GL_STATIC_DRAW);
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

void InitializeObjectMatrices(struct MobileObject* object, float x, float y, float z, float tiltX, float tiltZ, float factor) {
	SetIdentityMatrix(object->ModelMatrix);

	/* Translate and rotate cube onto tip */
    SetTranslation(0, 0, 0, object->TranslateOrigin);
    SetRotationX(tiltX, object->RotateX);
    SetRotationZ(tiltZ, object->RotateZ);	

    /* Translate down */	

    /* Initial transformation matrix */
    SetTranslation(x, y, z, object->TranslationMatrix);
	SetScaling(factor, factor, factor, object->TranslateOrigin);
    MultiplyMatrix(object->RotateX, object->TranslateOrigin, object->InitialTransform);
    MultiplyMatrix(object->RotateZ, object->InitialTransform, object->InitialTransform);
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
    /* Set background (clear) color to dark blue */ 
    glClearColor(0.0, 0.0, 0.4, 0.0);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);    

 
	BigBar.rotationSpeed = 180;
	LeftBar.rotationSpeed = 120;
	SetupDataBuffers(&BigBar, vertexBar, sizeof(vertexBar), colorBar, sizeof(colorBar), indexBar, sizeof(indexBar)); 
    SetupDataBuffers(&LeftBar, vertexBar, sizeof(vertexBar), colorBar, sizeof(colorBar), indexBar, sizeof(indexBar));

    /* Setup shaders and shader program */
    CreateShaderProgram();  

    /* Initialize matrices */
    SetIdentityMatrix(ProjectionMatrix);
    SetIdentityMatrix(ViewMatrix);

    /* Set projection transform */
    float fovy = 45.0;
    float aspect = 1.0; 
    float nearPlane = 1.0; 
    float farPlane = 50.0;
    SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);

    /* Set viewing transform */
    float camera_disp = -30.0;
    SetTranslation(0.0, 0.0, camera_disp, ViewMatrix);

    InitializeObjectMatrices(&BigBar, 0.0, 8.0, 0.0, 0.0, 0.0, 1.0);
	InitializeObjectMatrices(&LeftBar, -4.0, 3.5, 0.0, 0.0, 0.0, 0.5);
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
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 400);
    glutCreateWindow("CG Proseminar - Rotating Cube");

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
