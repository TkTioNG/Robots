
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <string>

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")

#define WINDOW_TITLE "OpenGL Window"

void setPerspective();
void drawBg();
void drawRobot();
void drawSpike();

GLUquadricObj* myQuadricObject = gluNewQuadric();
int slices = 50;
int stacks = 50;

float camX = 0.5;
float camY = 2.0;
float camZ = 10.0;

float lookX = 0.0;
float lookY = 0.5;
float lookZ = 0.0;

float rotateX = 0.0;
float rotateY = 0.0;
float rotateZ = 0.0;

float translateX = 0.0;
float translateY = 0.0;
float translateZ = 0.0;

float lightX = 0.0;
float lightY = 0.0;
float lightZ = -2.0;

float zoom = 1.0;

GLuint texture = 0;
GLuint texture_earth_spike = 1;

BITMAP BMP;

boolean draw_Spike = false;

float raise_Spike = -3.0;

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			PostQuitMessage(0);
			break;
		}
		case 0x41: // A
		{
			camX -= 0.1;
			break;
		}
		case 0x44: // D
		{
			camX += 0.1;
			break;
		}
		case 0x57: // W
		{
			camY += 0.1;
			break;
		}
		case 0x53: // S
		{
			camY -= 0.1;
			break;
		}
		case 0x51:  // Q
		{
			camZ -= 0.1;
			break;
		}
		case 0x45: // E
		{
			camZ += 0.1;
			break;
		}
		case 0x4A: // J
		{
			lookX -= 0.1;
			break;
		}
		case 0x4C: // L
		{
			lookX += 0.1;
			break;
		}
		case 0x49: // I
		{
			lookY += 0.1;
			break;
		}
		case 0x4B: // K
		{
			lookY -= 0.1;
			break;
		}
		case 0x55: // U
		{
			//lookZ += 0.1;
			if (zoom < 4.0)
			{
				zoom += 0.05;
				setPerspective();
			}
			break;
		}
		case 0x4F: // O
		{
			//lookZ -= 0.1;
			if (zoom >= 0.25)
			{
				zoom -= 0.05;
				setPerspective();
			}
			break;
		}
		case VK_LEFT: // LEFT
		{
			translateX += 0.1;
			break;
		}
		case VK_RIGHT: // RIGHT
		{
			translateX -= 0.1;
			break;
		}
		case VK_UP: // UP
		{
			translateZ += 0.1;
			break;
		}
		case VK_DOWN: // DOWN
		{
			translateZ -= 0.1;
			break;
		}
		case VK_OEM_COMMA: // ,
		{
			translateY += 0.1;
			break;
		}
		case VK_OEM_PERIOD: // .
		{
			translateY -= 0.1;
			break;
		}
		case VK_NUMPAD1: // 1
		{
			//lightX += 0.1;
			rotateY += 1;
			break;
		}
		case VK_NUMPAD3: // 3
		{
			//lightX -= 0.1;
			rotateY -= 1;
			break;
		}
		case VK_NUMPAD5: // 5
		{
			//lightY -= 0.1;
			rotateX += 1;
			break;
		}
		case VK_NUMPAD2: // 2
		{
			//lightY += 0.1;
			rotateX -= 1;
			break;
		}
		case VK_NUMPAD4: // 4
		{
			lightZ -= 0.1;
			//rotateZ += 0.1;
			break;
		}
		case VK_NUMPAD6: // 6
		{
			lightZ += 0.1;
			//rotateZ -= 0.1;
			break;
		}
		case VK_SPACE:
		{
			camX = 0.0;
			camY = 0.0;
			camZ = 7.0;

			lookX = 0.0;
			lookY = 0.0;
			lookZ = 0.0;

			rotateX = 0.0;
			rotateY = 0.0;
			rotateZ = 0.0;

			translateX = 0.0;
			translateY = 0.0;
			translateZ = 0.0;

			lightX = 0.0;
			lightY = 0.0;
			lightZ = 2.0;

			zoom = 1.0;
			break;
		}
		case 0x31:
		{
			draw_Spike = !draw_Spike;
			raise_Spike = -3.0;
			break;
		}
		break;
		}
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

bool initPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------

void display()
{
	//--------------------------------
	//	OpenGL drawing
	//--------------------------------

	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	GLfloat lightposition[] = { lightX, lightY, lightZ, 0.1 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightposition);
	GLfloat disfuse[] = { 0.7, 0.7, 0.7, 0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, disfuse);
	GLfloat ambient0[] = { 0.4, 0.4, 0.4, 0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);

	gluLookAt(camX, camY, camZ, lookX, lookY, lookZ, 0.0, 1.0, 0.0);

	glRotatef(rotateX, 1, 0, 0);
	glRotatef(rotateY, 0, 1, 0);
	glRotatef(rotateZ, 0, 0, 1);

	drawRobot();

	//--------------------------------
	//
	//	Draw Here
	//
	//--------------------------------

	glTranslatef(translateX, translateY, translateZ);

	drawBg();

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);


	glBegin(GL_QUADS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3f(-2.0, -1.0, 0.0);
	glVertex3f(-2.0, 1.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, -1.0, 0.0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex3f(1.0, -1.0, 0.0);
	glVertex3f(1.0, 1.0, 0.0);
	glVertex3f(2.41421, 1.0, -1.41421);
	glVertex3f(2.41421, -1.0, -1.41421);
	glEnd();

	//--------------------------------
	//	End of OpenGL drawing
	//--------------------------------
}
//--------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WindowProcedure;
	wc.lpszClassName = WINDOW_TITLE;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc)) return false;

	HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	initPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	glEnable(GL_DEPTH_TEST);
	setPerspective();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	HBITMAP hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), "Sky.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	GetObject(hBMP, sizeof(BMP), &BMP);


	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
	glTexCoord2f(0.0f, 0.0f);

	HBITMAP hBMP2 = (HBITMAP)LoadImage(GetModuleHandle(NULL), "earth_spike.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	GetObject(hBMP2, sizeof(BMP), &BMP);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture_earth_spike);
	glBindTexture(GL_TEXTURE_2D, texture_earth_spike);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
	glTexCoord2f(0.0f, 0.0f);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//--------------------------------
	//	End initialization
	//--------------------------------

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		display();

		SwapBuffers(hdc);
	}

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}

void drawRobot()
{
	/*
	glPushMatrix();
	glColor3f(0.0, 1, 0.0);
	glRotatef(90, 1, 0, 0);
	gluSphere(myQuadricObject, 0.5, slices, stacks);
	glPopMatrix();*/

	glPushMatrix();
	glRotatef(45, 0, 1, 0);

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glEnd();

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glEnd();

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glEnd();

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glEnd();

	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glEnd();

	glColor3f(0.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glEnd();

	glPopMatrix();

	if (draw_Spike)
		drawSpike();

}

void drawSpike()
{
	gluQuadricDrawStyle(myQuadricObject, GLU_FILL);
	gluQuadricNormals(myQuadricObject, GLU_SMOOTH);
	gluQuadricOrientation(myQuadricObject, GLU_INSIDE);
	gluQuadricTexture(myQuadricObject, GL_TRUE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_earth_spike);

	if (raise_Spike <= 0.0)
		raise_Spike += 0.03;

	glPushMatrix();
	glTranslatef(0, -1.2 + raise_Spike, 0);

	glPushMatrix();
	glTranslatef(1.5, 0, 2.95);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.16, 0.0, 2.0, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(1.0, 0, 2.28);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.11, 0.0, 1.8, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.5, 0, 2.38);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.08, 0.0, 1.6, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, 0, 2.53);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.06, 0.0, 1.5, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.5, 0, 2.24);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.13, 0.0, 1.9, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-1.0, 0, 2.12);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.1, 0.0, 1.6, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-1.5, 0, 2.85);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.2, 0.0, 1.2, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.5, 0, 1.70);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.08, 0.0, 1.6, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(1.0, 0, 1.48);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.06, 0.0, 1.5, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.5, 0, 1.38);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.16, 0.0, 2.0, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, 0, 1.53);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.11, 0.0, 1.8, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.5, 0, 1.44);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.13, 0.0, 1.9, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-1.0, 0, 1.32);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.2, 0.0, 1.2, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-1.5, 0, 1.65);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.1, 0.0, 1.6, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.5, 0, 3.50);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.08, 0.0, 1.6, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(1.0, 0, 3.28);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.1, 0.0, 1.6, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.5, 0, 3.18);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.2, 0.0, 1.2, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, 0, 3.33);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.11, 0.0, 1.8, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.5, 0, 3.24);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.13, 0.0, 1.9, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-1.0, 0, 3.12);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.06, 0.0, 1.5, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-1.5, 0, 3.45);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(myQuadricObject, 0.16, 0.0, 2.0, 20, 20);
	glPopMatrix();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void setPerspective()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45 / zoom, 1, 1, 100);
}

void drawBg()
{
	glPushMatrix();
	glColor3f(0.8, 0.6, 0.3);
	glScalef(30, 1, 30);
	glBegin(GL_QUADS);
	glVertex3f(-1, -1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(1, -1, -1);
	glVertex3f(-1, -1, -1);
	glEnd();
	glPopMatrix();

	gluQuadricDrawStyle(myQuadricObject, GLU_FILL);
	gluQuadricNormals(myQuadricObject, GLU_SMOOTH);
	gluQuadricOrientation(myQuadricObject, GLU_INSIDE);
	gluQuadricTexture(myQuadricObject, GL_TRUE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLfloat ambient1[] = { 1.0, 1.0, 1.0, 0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient1);
	glPushMatrix();
	glScalef(30, 5, 30);
	glTranslated(0, -0.4, 0);
	glRotatef(75, 0, 1, 0);
	glRotatef(270, 1, 0, 0);
	// glColor3f(1.0, 0.3, 0.2);
	gluCylinder(myQuadricObject, 1.0, 1.0, 15.0, slices, stacks);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
//--------------------------------------------------------------------