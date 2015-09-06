#include <iostream>
#include <string>
#include "GeometricObject.h"
#include "RectWorld.h"
#include "Sphere.h"
#include "Vec3.h"
#include "SphereGPU.h"

#include <omp.h>


#include <algorithm>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "FrameTextViewGL.h"
// CUDA includes
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <helper_cuda.h>



RectWorld * World;
std::vector<GeometricObject*>* Objects;

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

typedef Vec3 Angle;
typedef Vec3 Color;
float eyePosx = -25000;
Vec3 ViewPort(eyePosx + 23000, image_width2, image_height2);
Vec3 eyePos(eyePosx, 0, 0);
Angle ViewPortAngle(0, 0, 0);
Vec3 Light(0, 0, 500);
Color LightColor(255, 127, 100);
Color AmbientColor(50, 50, 50);


GLuint bufferID;

struct cudaGraphicsResource* ObjectsBufferCuda;
GLuint ObjectsBuffer;
struct cudaGraphicsResource *bufferCuda;
GLuint textureID;
float ka = 0.2f;

//GLWindow Texts
std::vector<TextViewGL*> textViews; 
TextViewGL* Mode;


void Initialize();
extern "C" void CudaMain(float*, unsigned int*, int, int);
extern "C" void SetMemoryOnGPU(int, SphereGpu*);
extern "C" void FreeMemoryOnGPU();
extern bool CreateWorld(std::string);
extern int elmSize;

void FreeMemory();


float* SphereOnGpu;



int rgbToInt(float r, float g, float b)
{
	if (r > 1.0f)
		r = 1.0f;
	if (g > 1.0f)
		g = 1.0f;
	if (b > 1.0f)
		b = 1.0f;
	return (int(r * 255) << 16) | (int(g * 255) << 8) | int(b * 255);
}

int DetermineColor(Vec3 ray, Vec3 Normal, Vec3 hitPoint, int objId)
{
	Vec3 SourceToLight = Light - hitPoint;
	SourceToLight.Normalize();
	Normal.Normalize();
	bool willBeShaded = false;

	for (int id = 0; id < Objects->size(); id++)
	{
		Vec3 N, hP;
		float dist;
		if (id != objId && (*Objects)[id]->isRayIntersects(SourceToLight, hitPoint, N, hP, dist))
		{
			willBeShaded = true;
			break;
		}
	}

	Vec3 amb = (*Objects)[objId]->getDiffuse();
	Vec3 baseColor = Vec3(amb.getX()*ka, amb.getY()*ka, amb.getZ()*ka);
	if (willBeShaded)
	{
		return rgbToInt(baseColor.getX(), baseColor.getY(), baseColor.getZ());
	}
	else{
		float kd = max(0, SourceToLight.dotProduct(Normal));
		Vec3 h = SourceToLight - ray;
		h.Normalize();
		float ks = pow(max(0, h.dotProduct(Normal)), 20.0);
		baseColor = baseColor + Vec3(amb.getX()*kd, amb.getY()*kd, amb.getZ()*kd);
		baseColor = baseColor + Vec3(amb.getX()*ks, amb.getY()*ks, amb.getZ()*ks);
		return rgbToInt(baseColor.getX(), baseColor.getY(), baseColor.getZ());
	}
}


void RayTaceInnerLoop(int * dst, int i, int k)
{
	for (int j = -ViewPort.getZ() / 2, t = 0; j < ViewPort.getZ() / 2; j++, t++)
	{
		Vec3 src(ViewPort.getX(), i, j);
		Vec3 ray = Vec3(1, 0, 0);
		/*Vec3 ray = Vec3(ViewPort.getX(), i, j) - eyePos;
		ray.rotate(ViewPortAngle);
		ray.Normalize();*/
		float minDist = 1000000;
		Vec3 Normal, hitPoint;
		int objectId;
		for (int id = 0; id < Objects->size(); id++)
		{
			float dist;
			Vec3 N, hP;
			if ((*Objects)[id]->isRayIntersects(ray, src, N, hP, dist))
			{
				if (dist < minDist)
				{
					minDist = dist;
					Normal = N;
					hitPoint = hP;
					objectId = id;
				}
			}
		}

		if (minDist != 1000000)
			dst[t * (int)ViewPort.getY() + k] = DetermineColor(ray, Normal, hitPoint, objectId);
		else
			dst[t * (int)ViewPort.getY() + k] = rgbToInt(0.8f, 0.8f, 0.8f);
		//World->SendRay(collidedObj,size,ray,eyePos,Objects);
		//Color pixelC = DetermineColor(collidedObj,size);
	}
}
void RayTraceCPU(int *dst)
{
	for (int i = -ViewPort.getY() / 2, k = 0; i < ViewPort.getY() / 2; i++, k++)
	{
		RayTaceInnerLoop(dst, i, k);
	}
}

void RayTraceomp(int *dst)
{
	int i, k;
	int min = -ViewPort.getY() / 2;
	int max = ViewPort.getY() / 2;
#pragma omp parallel for num_threads(4) private(i,k)
	for (i = min; i < max; i++)
	{
		k = i + max;
		RayTaceInnerLoop(dst, i, k);
	}
}
void UpdateObjects(){
	for (int i = 0; i< Objects->size(); i++)
		(*Objects)[i]->Move();
}

void UpdateObjectsMP(){
#pragma omp parallel for schedule(static)
	for (int i = 0; i< Objects->size(); i++)
		(*Objects)[i]->Move();
}

void updateBufferCPU()
{
	GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

	int* dst = (int*)ptr;
	UpdateObjects();
	RayTraceCPU((int*)ptr);

	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer


}

void updateBufferopenMP()
{
	GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

	int* dst = (int*)ptr;
	UpdateObjects();
	RayTraceomp((int*)ptr);

	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer


}

float clamp2(float x, float a, float b)
{
	return max(a, min(b, x));
}

void keyBoardListener(unsigned char key, int _x, int _y)
{
	if (key == '1' || key == '2' || key == '3')
	{
		ProcessingType = (int)(key - '0');
		switch (ProcessingType)
		{
			case 1:
				Mode->SetText("Mode:CPU");
				break;
			case 2:
				Mode->SetText("Mode:OpenMP");
				break;
			case 3:
				Mode->SetText("Mode:CUDA");
				break;
			default:
				break;
		}
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

	if (ProcessingType == 1)
	{
		updateBufferCPU();
	}
	else if (ProcessingType == 2)
	{
		updateBufferopenMP();
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
	for (int i = 0; i < Objects->size(); i++)
	{
		Vec3 Pos, Color;
		float r;
		bool toRight;
		(*Objects)[i]->getInfo(Pos, r, Color, toRight);
		obj[i * 8] = Pos.getX();
		obj[i * 8 + 1] = Pos.getY();
		obj[i * 8 + 2] = Pos.getZ();
		obj[i * 8 + 3] = r;
		obj[i * 8 + 4] = Color.getX();
		obj[i * 8 + 5] = Color.getY();
		obj[i * 8 + 6] = Color.getZ();
		obj[i * 8 + 7] = toRight ? 1.0f : -1.0f;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER); // release pointer to mapping buffer

}


//SetView
void SetTexts()
{
	textViews.push_back(new FrameTextViewGL());
	Mode = new TextViewGL("Mode: CPU");
	textViews.push_back(Mode);

}


void InitializeMemoryGPU()
{
	glGenBuffers(2, &ObjectsBuffer);
	// Make this the current UNPACK buffer (OpenGL is state-based)
	glBindBuffer(GL_ARRAY_BUFFER, ObjectsBuffer);
	// Allocate data for the buffer. 4-channel 8-bit image
	int size = Objects->size();
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

	glutMainLoop();


}

template <class T>
void DeletePtrVector(std::vector<T> &vect)
{
	for (T obj : vect)
		delete obj;
	vect.clear();
}

void FreeMemory()
{
	DeletePtrVector(textViews);
	DeletePtrVector(*Objects);
	delete World;
}

int main()
{
	Objects = new std::vector<GeometricObject*>();
	CreateWorld("World.txt");
	elmSize = Objects->size();
	//CopyToGpu();
	checkCudaErrors(cudaGLSetGLDevice(0));
	//SetMemoryOnGPU(size, SphereOnCpu);
 
	Initialize();
	
	return 0;
}
