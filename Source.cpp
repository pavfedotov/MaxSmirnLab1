
//------------------------------------------------------- 
//      GL_2D.cpp
//       =========
//  ��������� ������� �����  � OpenGL
//--------------------------------------------------------
#define STRICT

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <gl\gl.h>
#include <gl\\glu.h>
#include <vector>
#include <string>
// #include <gl\\glaux.h>
#pragma comment (linker, "/defaultlib:opengl32.lib")
#pragma comment (linker, "/defaultlib:glu32.lib")


#define MIN(a,b) ((a)>=(b) ? (b) : (a))
#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MsgBox(str) MessageBox(hwndMain, str, aczAppName, 0L)
# define PI           3.14159265358979323846  /* pi */

char aczAppName[512] = "GlBaseSamp";
LPCSTR pczMainWndClass = "WC__FrameWnd";

HINSTANCE hiApp;
HWND hwndMain;
HDC hdcMain;
HGLRC hGlRc;
// ������� ������ � ������� �����������
RECT rcViewport;

//  ������ ������� ������ � ��������� (���������� ���)
# define DIM_SCENE 2.0 
#define N1 4
#define N2 8
struct Vect {
	double x, y;
};

void InitOpenGL();
void UninitOpenGL();
void InitViewport(const int cx, const int cy);
void Draw();
Vect route(Vect, double, double x, double y);
void DrawModel(int, int);
LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);
void MainOnPaint(HWND);
int RegisterWndClasses(void);


const double R = 0.1;
const double W = 0.18;



// ������������� ��������� ��������������� OpenGL
void InitOpenGL()
{
	hdcMain = GetDC(hwndMain);
	int iPixelFormat;
	// ������������� ��������� ������ ��������
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // ������ ���������
		1,                              // ����� ������ OpenGL
		PFD_DRAW_TO_WINDOW |          // ��������� ������ � ����
		PFD_SUPPORT_OPENGL |          // ��������� OpenGL
		PFD_DOUBLEBUFFER,		// ������� �����������
		PFD_TYPE_RGBA,          // ����� � ������ RGBA
		24,                     // 24 ������� �� ����
		0, 0, 0, 0, 0, 0,       // ���� ����� ������������
		0,                      // �� ������������ �����-��������
		0,                      // �������� ������ ������������
		0,                      // ����� ������������ �� ������������
		0, 0, 0, 0,             // ���� ������������ ������������
		32,                     // 32-��������� ����� �������
		0,                      // ����� ��������� �� ������������
		0,                      // ��������������� ����� �� ������������
		PFD_MAIN_PLANE,         // �������� ����
		0,                      // ��������������
		0, 0, 0                 // ����� ���� ������������
	};
	// ������ pix-�������
	iPixelFormat = ChoosePixelFormat(hdcMain, &pfd);
	if (!iPixelFormat)
		MsgBox("�������� ������ ��������");
	// ��������� pix-�������
	if (!SetPixelFormat(hdcMain, iPixelFormat, &pfd))
		MsgBox("������ �������� �� ����������");
	// �������� OpenGL-���������
	hGlRc = wglCreateContext(hdcMain);
	if (!hGlRc)
		MsgBox("�������� OpenGL �� ������");
	// ��������� �������� OpenGL-���������
	if (!wglMakeCurrent(hdcMain, hGlRc))
		MsgBox("�������� OpenGL �� ����������");
}  // func InitOpenGL

 // ������������ ��������� ���������������
void UninitOpenGL()
{
	// �������� ������� �������� ��������������� OpenGL
	wglMakeCurrent(NULL, NULL);
	// ��������� ��������� ��������������� OpenGL
	if (hGlRc)
		wglDeleteContext(hGlRc);
	// ������������ ��������� ������� �������
	ReleaseDC(hwndMain, hdcMain);
}  //func UninitOpenGL

 // ������������� ������� ������
void InitViewport(const int cx,
	const int cy)
{
	RECT rc;
	GetClientRect(hwndMain, &rc);
	// ������ ������� ������ OpenGL �� ����������� ������� ����
	rcViewport.left = 10;
	rcViewport.top = 10;
	int viewSize = MIN(cx, cy) - 20;
	rcViewport.right = rcViewport.left + viewSize;
	rcViewport.bottom = rcViewport.top + viewSize;
	glMatrixMode(GL_PROJECTION);
	glViewport(rcViewport.left,
		cy - rcViewport.bottom,
		rc.right - 10, viewSize);
	glLoadIdentity();
	glOrtho(-DIM_SCENE, DIM_SCENE, -DIM_SCENE, DIM_SCENE, -DIM_SCENE, DIM_SCENE);
	glMatrixMode(GL_MODELVIEW);
} //func InitViewport


 // ����� ������
void DrawModel(int numVertices, int num)
{
	if (numVertices > 12) {
		MessageBoxA(hwndMain, "�� ����� ���� ������ 12 �� X", "ERROR", ERROR);
		return;
	}
	if (num > 12) {
		MessageBoxA(hwndMain, "�� ����� ���� ������ 12 �� Y", "ERROR", ERROR);
		return;
	}
	glClearColor(1, 1, 1, 1.0);

	// ���������� �����.
	double k_gab = 0.95;


	// �������� ����� �����
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3d(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(-DIM_SCENE * k_gab, -DIM_SCENE * k_gab);
	glVertex2d(DIM_SCENE * k_gab, -DIM_SCENE * k_gab);
	glVertex2d(DIM_SCENE * k_gab, DIM_SCENE * k_gab);
	glVertex2d(-DIM_SCENE * k_gab, DIM_SCENE * k_gab);
	glVertex2d(-DIM_SCENE * k_gab, -DIM_SCENE * k_gab);
	glEnd();




	// ������ ��������� ��������� �������
	double angle;
	double x;
	double y;
	double x0 = 0 - ((DIM_SCENE * k_gab))+(3*R);
	double y0 = (DIM_SCENE * k_gab) - (3 * R);

	//�������
	double r = R;
	double  w = W;
	Vect vertex[N1][N2];
	

	glPointSize(5.0f);

	for (int k = 0; k < num; k++)
	{


		for (int m = 0; m < numVertices; m++) {


			glBegin(GL_POLYGON);
			for (int i = 0; i < 8; ++i) {

				angle = (i * 2 * PI / (8));

				glColor3d(0, 1, 0);
				vertex[m][k].x = x0;
				vertex[m][k].y = y0;
				x = x0 + cos(angle) * r;
				y = y0 + sin(angle) * r;


				glVertex2d(x, y);

			}
			glEnd();

			x0 += r * 3;

		}

		x0 = 0 - ((DIM_SCENE * k_gab)) + (3 * R);
		y0 -= r * 3;
	}
	////////////////////////////////////////
	x0 = 0 - ((DIM_SCENE * k_gab)) + (3 * R);
	y0 = (DIM_SCENE * k_gab) - (3 * R);

	for (int k = 0; k < num; k++)
	{


		for (int m = 0; m < numVertices; m++) {


			glBegin(GL_LINES);
			for (int i = 0; i < 8; ++i) {

				angle = (i * 2 * PI / (8));

				glColor3d(0, 0, 1);

				x = x0 + cos(angle) * r;
				y = y0 + sin(angle) * r;

				glVertex2d(x0, y0);
				glVertex2d(x, y);

			}
			glEnd();

			x0 += r * 3;

		}
		x0 = 0 - ((DIM_SCENE * k_gab)) + (3 * R);
		y0 -= r * 3;
	}
	///////////////////////////////////////////////////
	x0 = 0 - ((DIM_SCENE * k_gab)) + (3 * R);
	y0 = (DIM_SCENE * k_gab) - (3 * R);

	for (int k = 0; k < num; k++)
	{


		for (int m = 0; m < numVertices; m++) {


			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < 8; ++i) {

				angle = (i * 2 * PI / (8));

				glColor3d(0, 0, 0);

				x = x0 + cos(angle) * r;
				y = y0 + sin(angle) * r;

				glVertex2d(x, y);

			}
			glEnd();

			x0 += r * 3;

		}
		x0 = 0 - ((DIM_SCENE * k_gab)) + (3 * R);
		y0 -= r * 3;
	}

	///////////////////////////////////////////////////////////
	int b = 0;
	for (int k = 0; k < num; k++)
	{

		glBegin(GL_LINE_LOOP);
		for (int m = 0; m < numVertices; m++) {


			
			glColor3d(0, 0, 1);
			glVertex2d(vertex[m][b].x, vertex[m][b].y);
			b++;
			if (b >= N2) {
				
				break;

			}
		}
		b = k;
		glEnd();
	}
	
	 b = 0;
	 int a = 1,a_const=1;
	for (int k = 0; k < num; k++)
	{

		glBegin(GL_LINE_LOOP);
		for (int m = 0; m < numVertices; m++) {



			glColor3d(0, 0, 1);
			glVertex2d(vertex[a][b].x, vertex[a][b].y);
			b++;
			a++;
			if (a >= N1) {
				break;
			}
			if (b >= N2) {
				break;
			}
			
		}
		a_const++;
		a = a_const;
		b = 0;
		glEnd();
	}
	///////////////////////////////////////////////////////////
	b = N2-1;
	for (int k = num-1; k > -1; k--)
	{

		glBegin(GL_LINE_LOOP);
		for (int m = 0; m < numVertices; m++) {
			glColor3d(0, 0, 1);

			glVertex2d(vertex[m][b].x, vertex[m][b].y);

			b--;
			if (b < 0) {

				break;

			}

		

		}
		b = k;
		glEnd();
	}


	b = N2 - 1; a = 1, a_const = 1;
	for (int k = num - 1; k > 0; k--)
	{

		glBegin(GL_LINE_LOOP);
		for (int m = 0; m < numVertices; m++) {



			glColor3d(0, 0, 1);
			glVertex2d(vertex[a][b].x, vertex[a][b].y);
			b--; a++;
			if (a >= N1) {
				break;
			}
			if (b < 0) {

				break;

			}
		}
		a_const++;
		a = a_const;
		b = N2 - 1;
		glEnd();
	}


	///////////////////////////////////////////////////////////
	b = N2 - 1;
	for (int k = num - 1; k > -1; k--)
	{

		
		for (int m = 0; m < numVertices; m++) {
			b--;
			if (b < 0) {

				break;

			}

			glBegin(GL_POINTS);
			glColor3d(1, 0, 0);
			glVertex2d((vertex[m][b].x + vertex[m + 1][b + 1].x) / 2, (vertex[m][b].y + vertex[m + 1][b + 1].y) / 2);
			glEnd();
		}
		b = k;
		glEnd();
	}


	b = N2 - 1; a = 0, a_const = 0;
	for (int k = num - 1; k > 0; k--)
	{
		for (int m = 0; m < numVertices; m++) {
			b--; a++;
			if (a >= N1) {
				break;
			}
			if (b < 0) {

				break;

			}
			glBegin(GL_POINTS);
			glColor3d(1, 0, 0);
			glVertex2d((vertex[a][b].x + vertex[a + 1][b + 1].x) / 2, (vertex[a][b].y + vertex[a + 1][b + 1].y) / 2);
			glEnd();
		}
		a_const++;
		a = a_const;
		b = N2 - 1;
		glEnd();
	}

	glFinish();

	SwapBuffers(hdcMain);
}

// ������� ��� ��������� n ��������������� � �������� radius � ������� � ����� (x, y)
void DrawRectangles(double x, double y, double radius, int n)
{
	// �������� ��������� �����
	glBegin(GL_LINE_LOOP);

	// ��������� ��������� �����
	glEnd();
}
// ��������� �������� ����
LRESULT WINAPI MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_SIZE:
		InitViewport(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_PAINT:
		MainOnPaint(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0L;
}  // func MainWndProc

 // ���������� WM_PAINT
void MainOnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	BeginPaint(hwnd, &ps);

	DrawModel(N1,N2);
	// ������ ��������������


	EndPaint(hwnd, &ps);
}  //func MainOnPaint

 // ����������� ������� ����
int RegisterWndClasses()
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.hInstance = hiApp;
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// wc.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU);
	wc.lpszClassName = pczMainWndClass;
	if (!RegisterClassEx(&wc))
		return 0;
	return 1;
}  // func RegisterWndClasses

 // ������������� ����������
void InitializeApp()
{
	// ���������������� ����� � ������� ������� ����
	RegisterWndClasses();
	hwndMain = CreateWindow(pczMainWndClass, NULL,
		WS_CAPTION | WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME |
		WS_OVERLAPPED | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0, 0, 800, 800, NULL,
		NULL, hiApp, NULL);
	if (!hwndMain)
		return;
	// ���������������� OpenGL
	InitOpenGL();
	// ������� ����� � ���� ��� ������� ������� ������
	SelectObject(hdcMain, GetStockObject(NULL_BRUSH));
	SelectObject(hdcMain, GetStockObject(WHITE_PEN));
	//
	RECT r;
	GetClientRect(hwndMain, &r);
	InitViewport(r.right, r.bottom);
}  //func InitializeApp

 // ������������ �������� ����������
void UninitializeApp()
{
	UnregisterClass(pczMainWndClass, hiApp);
}  //func UninitializeApp

 // ����� ����� ����������
int APIENTRY WinMain(HINSTANCE hi, HINSTANCE hi_void, LPSTR pc_cl, int wsm)
{
	hiApp = hi;
	InitializeApp();
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
		DispatchMessage(&msg);
	UninitializeApp();
	return 0;
}  //func WinMain


Vect route(Vect mass, double ang, double x, double y) {
	//��������� � ���� ������ ��� ��� �� �� ���
	Vect retur;

	double x1_relative = mass.x - x;
	double 	y1_relative = mass.y - y;

	double x2_relative = (x1_relative * cos(ang)) - (y1_relative * sin(ang));
	double y2_relative = (x1_relative * sin(ang)) + (y1_relative * cos(ang));

	retur.x = x2_relative + x;
	retur.y = y2_relative + y;

	return retur;
}