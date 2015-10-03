#include <iostream>
#include <string>
#include <omp.h>

#include "Objects\WorldObjects\GeometricObject.h"
#include "Objects\WorldObjects\Sphere.h"
#include "Utils\Vec3.h"
#include "SphereGPU.h"



#include <algorithm>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ViewUtils\FrameTextViewGL.h"
// CUDA includes
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <helper_cuda.h>

#include "RayTracers\RayTracingController.h"
#include "RayTracers\RayTracingStrategySequential.h"
#include "RayTracers\RayTracingStratingOpenMP.h"
#include "RayTracers\RayTracingStrategyCUDA.h"
#include "World.h"


//******************//
//***TEST
#define WILLTEST false
extern int TestMain();
//******************//

unsigned int window_width2 = 768;
unsigned int window_height2 = 512;
unsigned int image_width2 = window_width2;
unsigned int image_height2 = window_height2;
int iGLUTWindowHandle2 = 0;
/*0 = CPU
**1 = openMP
**2 = CUDA
*/
int ProcessingType = 1;

float eyePosx = -25000;
Vec3 ViewPort(eyePosx + 23000, image_width2, image_height2);
Vec3 eyePos(eyePosx, 0, 0);
Angle ViewPortAngle(0, 0, 0);
//Color LightColor(255, 127, 100);
//Color AmbientColor(50, 50, 50);


GLuint bufferID;

struct cudaGraphicsResource* ObjectsBufferCuda;
GLuint ObjectsBuffer;
struct cudaGraphicsResource *bufferCuda;
GLuint textureID;

extern int elmSize;
//GLWindow Texts
std::vector<TextViewGL*> textViews; 
TextViewGL* Mode;

//RayTracers
enum RayTracingOptions{Sequential=1,OpenMP=2,CUDA=3};
RayTracingController mRayTracerController;

//World
World* world;


void Initialize();
extern bool CreateWorld(std::string);
extern "C" void CudaMain(float*, unsigned int*, int, int);
extern "C" void SetMemoryOnGPU(int, SphereGpu*);
extern "C" void FreeMemoryOnGPU();

void FreeMemory();


float* SphereOnGpu;

float clamp2(float x, float a, float b)
{
	return max(a, min(b, x));
}

void keyBoardListener(unsigned char key, int _x, int _y)
{
	//StrategySettingKeys
	if (key == '1' || key == '2' || key == '3')
	{
		//lambda for the first time yeyyy
		mRayTracerController.SetStrategy((int)(key - '0'), [](char* str){
			Mode->SetText(str);
		});
		if (key != '3')
			world->SetUpdateType((int)(key - '0'));
		ProcessingType = (int)(key - '0');
	}
}

//DISPLAY 
void textDisplay()
{

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_DEPTH_BUFFER_BIT);
	
	for (TextViewGL *v : textViews)
	{
		v->draw();
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

}

void display()
{

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	//glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


	// Select the appropriate buffer
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, bufferID);
	// Select the appropriate texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Make a texture from the buffer
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, window_width2, window_height2,
		GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	if (ProcessingType == 1 || ProcessingType == 2)
	{
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

		int* dst = (int*)ptr;
		world->UpdateObjects();
		mRayTracerController.Apply(world, dst, ViewPort);

		 glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
	}
	else
	{
		unsigned int *out_data;
		size_t num_bytes;


		cudaGraphicsMapResources(1, &bufferCuda, 0);
		cudaGraphicsResourceGetMappedPointer((void **)&out_data, &num_bytes,
			bufferCuda);


		cudaGraphicsMapResources(1, &ObjectsBufferCuda, 0);
		size_t num_bytes2;
		cudaGraphicsResourceGetMappedPointer((void**)&SphereOnGpu, &num_bytes2, ObjectsBufferCuda);

		CudaMain(SphereOnGpu, out_data, image_height2, image_width2);

		cudaGraphicsUnmapResources(1, &ObjectsBufferCuda, 0);
		cudaGraphicsUnmapResources(1, &bufferCuda, 0);
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glViewport(0, 0, window_width2, window_height2);


	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0, -1.0, 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0, 1.0, 0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.0, 1.0, 0.5);
	glEnd();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	textDisplay();
	glutSwapBuffers(); glutPostRedisplay();

}
void CopyToGPU()
{


	GLfloat* obj = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	world->CopyToGPUArray(obj);
	elmSize = world->GetObjectSize();
	glUnmapBuffer(GL_ARRAY_BUFFER); // release pointer to mapping buffer
}


//SetView
void SetTexts()
{
	textViews.push_back(new FrameTextViewGL());
	Mode = new TextViewGL();
	textViews.push_back(Mode);
}


void InitializeMemoryGPU()
{
	glGenBuffers(2, &ObjectsBuffer);
	// Make this the current UNPACK buffer (OpenGL is state-based)
	glBindBuffer(GL_ARRAY_BUFFER, ObjectsBuffer);
	// Allocate data for the buffer. 4-channel 8-bit image
	int size = world->GetObjectSize();
	glBufferData(GL_ARRAY_BUFFER, size * 8 * sizeof(float),
		NULL, GL_DYNAMIC_COPY);

	checkCudaErrors(cudaGraphicsGLRegisterBuffer(&ObjectsBufferCuda, ObjectsBuffer, cudaGraphicsMapFlagsWriteDiscard));

	CopyToGPU();
}

void Initialize() {

	char fakeParam[] = "fake";
	char *fakeargv[] = { fakeParam, NULL };
	int fakeargc = 1;

	// Create GL context
	glutInit(&fakeargc, fakeargv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(window_width2, window_height2);
	iGLUTWindowHandle2 = glutCreateWindow("Ray Tracer Cuda");

	// initialize necessary OpenGL extensions
	glewInit();

	glClearColor(0.5, 0.5, 0.5, 1.0);
	glDisable(GL_DEPTH_TEST);
	// viewport
	glViewport(0, 0, window_width2, window_height2);

	// projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)window_width2 / (GLfloat)window_height2, 0.1f, 10.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// register callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keyBoardListener);
	glutCloseFunc(FreeMemory);


	// Generate a buffer ID
	glGenBuffers(1, &bufferID);
	// Make this the current UNPACK buffer (OpenGL is state-based)
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, bufferID);
	// Allocate data for the buffer. 4-channel 8-bit image
	glBufferData(GL_PIXEL_UNPACK_BUFFER, window_width2 * window_height2 * 4,
		NULL, GL_DYNAMIC_COPY);

	checkCudaErrors(cudaGraphicsGLRegisterBuffer(&bufferCuda, bufferID, cudaGraphicsMapFlagsWriteDiscard));

	InitializeMemoryGPU();

	// Enable Texturing
	glEnable(GL_TEXTURE_2D);
	// Generate a texture ID
	glGenTextures(1, &textureID);
	// Make this the current texture (remember that GL is state-based)
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Allocate the texture memory. The last parameter is NULL since we only
	// want to allocate memory, not initialize it
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width2, window_height2, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, NULL);
	// Must set the filter mode, GL_LINEAR enables interpolation when scaling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SetTexts();

	//default strategy set
	//default Mode text set
	keyBoardListener('1', 0, 0);

	glutMainLoop();


}

template <class T>
void DeleteArray(std::vector<T> arr)
{
	for (T obj : arr)
		delete obj;
	arr.clear();
}

void FreeMemory()
{
	delete world;
	DeleteArray(textViews);
}

int main()
{
	if (WILLTEST){
		return TestMain();
	}
	else{
		world = new World();
		CreateWorld("World.txt");
		mRayTracerController.AddStrategy(RayTracingOptions::Sequential, new RayTracingStrategySequential());
		mRayTracerController.AddStrategy(RayTracingOptions::OpenMP, new RayTracingStrategyOpenMP());
		mRayTracerController.AddStrategy(RayTracingOptions::CUDA, new RayTracingStrategyCUDA());

		checkCudaErrors(cudaGLSetGLDevice(0));

		Initialize();
		return 0;
	}
}
