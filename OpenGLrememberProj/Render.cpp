#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

#include "Functions.h"

bool textureMode = true;
bool lightMode = true;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("123.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  

	double N[3];
	double* n = N;

	//double A_T[2] = { -4, -4 };
	//double B_T[2] = { 4, -4 };
	//double C_T[2] = { 4, 4 };
	//double D_T[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	/*glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A_T);
	glTexCoord2d(1, 0);
	glVertex2dv(B_T);
	glTexCoord2d(1, 1);
	glVertex2dv(C_T);
	glTexCoord2d(0, 1);
	glVertex2dv(D_T);

	glEnd();*/

	double A[] = { -14, -5, 5 };
	double B[] = { -11, 0, 5 };
	double C[] = { -7, 0, 5 };
	double D[] = { -10, -3, 5 };

	double A2[] = { 3, 8, 5 };
	double B2[] = { 0, 0, 5 };
	double C2[] = { 3.7, 0, 5 };

	double A3[] = { -4, -1.3, 5 };
	double B3[] = { -7, 0, 5 };
	double C3[] = { 3.7, 0, 5 };

	double A4[] = { -14, -5, 0 };
	double B4[] = { -11, 0, 0 };
	double C4[] = { -7, 0, 0 };
	double D4[] = { -10, -3, 0 };

	double A5[] = { 3, 8, 0 };
	double B5[] = { 0, 0, 0 };
	double C5[] = { 3.7, 0, 0 };

	double A6[] = { -4, -1.3, 0 };
	double B6[] = { -7, 0, 0 };
	double C6[] = { 3.7, 0, 0 };

	double A8[] = { -10, -3, 5 };
	double B8[] = { -10, -3, 0 };
	double C8[] = { -14, -5, 0 };
	double D8[] = { -14, -5, 5 };

	double A9[] = { -14, -5, 5 };
	double B9[] = { -14, -5, 0 };
	double C9[] = { -11, 0, 0 };
	double D9[] = { -11, 0, 5 };

	double A10[] = { -11, 0, 5 };
	double B10[] = { -11, 0, 0 };
	double C10[] = { -7, 0, 0 };
	double D10[] = { -7, 0, 5 };

	double A12[] = { -2, 5, 5 };
	double B12[] = { -2, 5, 0 };
	double C12[] = { 0, 0, 0 };
	double D12[] = { 0, 0, 5 };

	double A13[] = { 0, 0, 5 };
	double B13[] = { 0, 0, 0 };
	double C13[] = { 3, 8, 0 };
	double D13[] = { 3, 8, 5 };

	double A14[] = { 3, 8, 5 };
	double B14[] = { 3, 8, 0 };
	double C14[] = { 4, -4, 0 };
	double D14[] = { 4, -4, 5 };

	double A15[] = { -2, 5, 5 };
	double B15[] = { 0, 0, 5 };
	double C15[] = { -7, 0, 5 };

	double A16[] = { -2, 5, 0 };
	double B16[] = { 0, 0, 0 };
	double C16[] = { -7, 0, 0 };

	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	/*glColor3d(0, 0, 0);*/;
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(1, 0);
	glVertex3dv(B);
	/*glTexCoord2d(1, 1);*/
	glVertex3dv(C);
	glTexCoord2d(0.5, 0.5);
	glVertex3dv(D);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glColor3d(0, 0, 0);
	glTexCoord2d(0, 0);
	glVertex3dv(A2);
	glTexCoord2d(0, 1);
	glVertex3dv(B2);
	glTexCoord2d(1, 1);
	glVertex3dv(C2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glColor3d(0, 0, 0);
	glTexCoord2d(0.8, 0.8);
	glVertex3dv(A3);
	glTexCoord2d(0, 1);
	glVertex3dv(B3);
	glTexCoord2d(1, 1);
	glVertex3dv(C3);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glColor3d(0, 0, 0);
	glTexCoord2d(0, 0);
	glVertex3dv(A4);
	glTexCoord2d(1, 0);
	glVertex3dv(B4);
	glVertex3dv(C4);
	glTexCoord2d(0.5, 0.5);
	glVertex3dv(D4);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(0, 0, 0);
	glTexCoord2d(0, 0);
	glVertex3dv(A5);
	glTexCoord2d(0, 1);
	glVertex3dv(B5);
	glTexCoord2d(1, 1);
	glVertex3dv(C5);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(0, 0, 0);
	glTexCoord2d(0.8, 0.8);
	glVertex3dv(A6);
	glTexCoord2d(0, 1);
	glVertex3dv(B6);
	glTexCoord2d(1, 1);
	glVertex3dv(C6);
	glEnd();

	CalcNormal(B8, A8, C8, n);

	glBegin(GL_QUADS);
	glNormal3dv(N);
	/*glColor4d(1, 0.75, 0.79, 0.8);*/
	glColor3d(1, 0.75, 0.79);
	glTexCoord2d(0, 0);
	glVertex3dv(A8);
	glTexCoord2d(1, 0);
	glVertex3dv(B8);
	glTexCoord2d(1, 1);
	glVertex3dv(C8);
	glTexCoord2d(0, 1);
	glVertex3dv(D8);
	glEnd();


	CalcNormal(B9, A9, C9, n);

	glBegin(GL_QUADS);
	glNormal3dv(N);
	glColor3d(1, 0.75, 0.79);
	glTexCoord2d(0, 0);
	glVertex3dv(A9);
	glTexCoord2d(1, 0);
	glVertex3dv(B9);
	glTexCoord2d(1, 1);
	glVertex3dv(C9);
	glTexCoord2d(0, 1);
	glVertex3dv(D9);
	glEnd();

	CalcNormal(B10, A10, C10, n);

	glBegin(GL_QUADS);
	glNormal3dv(N);
	glColor3d(1, 0.75, 0.79);
	glTexCoord2d(0, 0);
	glVertex3dv(A10);
	glTexCoord2d(1, 0);
	glVertex3dv(B10);
	glTexCoord2d(1, 1);
	glVertex3dv(C10);
	glTexCoord2d(0, 1);
	glVertex3dv(D10);
	glEnd();

	CalcNormal(B12, A12, C12, n);

	glBegin(GL_QUADS);
	glNormal3dv(N);
	glColor3d(0.8, 0.4, 0.5);
	glTexCoord2d(0, 0);
	glVertex3dv(A12);
	glTexCoord2d(1, 0);
	glVertex3dv(B12);
	glTexCoord2d(1, 1);
	glVertex3dv(C12);
	glTexCoord2d(0, 1);
	glVertex3dv(D12);
	glEnd();

	CalcNormal(B13, A13, C13, n);

	glBegin(GL_QUADS);
	glNormal3dv(N);
	glColor3d(0.8, 0.4, 0.5);
	glTexCoord2d(0, 0);
	glVertex3dv(A13);
	glTexCoord2d(1, 0);
	glVertex3dv(B13);
	glTexCoord2d(1, 1);
	glVertex3dv(C13);
	glTexCoord2d(0, 1);
	glVertex3dv(D13);
	glEnd();

	CalcNormal(B14, A14, C14, n);

	glBegin(GL_QUADS);
	glNormal3dv(N);
	glColor3d(1, 0.75, 0.79);
	glTexCoord2d(0, 0);
	glVertex3dv(A14);
	glTexCoord2d(1, 0);
	glVertex3dv(B14);
	glTexCoord2d(1, 1);
	glVertex3dv(C14);
	glTexCoord2d(0, 1);
	glVertex3dv(D14);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glColor3d(0, 0, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(A15);
	glTexCoord2d(0, 0);
	glVertex3dv(B15);
	glTexCoord2d(1, 0);
	glVertex3dv(C15);
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(0, 0, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(A16);
	glTexCoord2d(0, 0);
	glVertex3dv(B16);
	glTexCoord2d(1, 0);
	glVertex3dv(C16);
	glEnd();

	//Низ и верх выпуклости

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, -1);
	glColor3f(0, 0, 0);
	glTexCoord2d(1, 1);
	glVertex3f(-4.5, 2.5, 0);
	for (double a = -2.36; a <= 0.8; a += 0.01) {
		glTexCoord2d(0, 0);
		glVertex2f(-4.5 - cos(a) * 3.52, 2.5 - sin(a) * 3.52);
	}
	glEnd();

	glPushMatrix();
	glTranslatef(0, 0, 5);

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, 1);
	glColor3f(0, 0, 0);
	glTexCoord2d(1, 1);
	glVertex2f(-4.5, 2.5);
	for (double a = -2.36; a <= 0.8; a += 0.01) {
		glTexCoord2d(0, 0);
		glVertex2f(-4.5 - cos(a) * 3.52, 2.5 - sin(a) * 3.52);
	}
	glEnd();

	glPushMatrix();
	glTranslatef(0, 0, -5);

	//Стенка выпуклости

	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 1);
	for (double a = -2.36; a <= 0.8; a += 0.001) {
		double x1 = -4.5 - cos(a) * 3.52;
		double y1 = 2.5 - sin(a) * 3.52;
		double x2 = -4.5 - cos(a + 0.01) * 3.52;
		double y2 = 2.5 - sin(a + 0.01) * 3.52;
		double xx1[3], xx2[3], xx3[3];
		xx1[0] = x1;
		xx1[1] = y1;
		xx1[2] = 0;

		xx2[0] = x1;
		xx2[1] = y1;
		xx2[2] = 5;
		
		xx3[0] = x2;
		xx3[1] = y2;
		xx3[2] = 0;

		CalcNormal(xx2, xx1, xx3, n);
		glNormal3dv(N);

		glVertex3d(x1, y1, 0);
		glVertex3d(x1, y1, 5);
		glVertex3d(x2, y2, 5);
		glVertex3d(x2, y2, 0);
	}
	glEnd();

	//Стена впуклости

	/*glColor4d(1, 0.75, 0.79, 0.8);*/
	glColor3d(1, 0.75, 0.79);
	glBegin(GL_QUADS);

	float X = -3.7917, Y = -14.583, R = 13.14;
	for (float p = 0.934; p <= 2.063; p += 0.001) {
		float x1 = X + cos(p) * R;
		float y1 = Y + sin(p) * R;
		float x2 = X + cos(p + 0.001) * R;
		float y2 = Y + sin(p + 0.001) * R;

		double xx1[3], xx2[3], xx3[3];
		xx1[0] = x1;
		xx1[1] = y1;
		xx1[2] = 0;

		xx2[0] = x1;
		xx2[1] = y1;
		xx2[2] = 5;

		xx3[0] = x2;
		xx3[1] = y2;
		xx3[2] = 0;

		CalcNormal(xx2, xx1, xx3, n);
		N[0] *= (-1);
		N[1] *= (-1);
		glNormal3dv(N);

		glVertex3f(x1, y1, 0);
		glVertex3f(x2, y2, 0);
		glVertex3f(x2, y2, 5);
		glVertex3f(x1, y1, 5);
	}
	glEnd();


	double E[] = { 3.7, 0, 0 };
	double E_1[] = { -7, 0, 0 };
	double E2[] = { 3.7, 0, 5 };
	double E2_1[] = { -7, 0, 5 };

	//1й треугольник с вырезом
#pragma region

	//glNormal3dv(v);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, -1);
	glColor3f(0, 0, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(E);
	for (float a = 4.077; a <= 4.9; a += 0.001) {

		float X1 = X - cos(a) * R;
		float Y1 = Y - sin(a) * R;

		glTexCoord2d(0.5, 0.5);
		glVertex3d(X1, Y1, 0);
	}

	glEnd();
	//glNormal3dv(v);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glColor3f(0, 0, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(E2);
	for (float a = 4.077; a <= 4.9; a += 0.001) {

		float X1 = X - cos(a) * R;
		float Y1 = Y - sin(a) * R;

		glTexCoord2d(0.5, 0.5);
		glVertex3d(X1, Y1, 5);
	}

	glEnd();

	//2й треугольник с вырезом

	//glNormal3dv(v);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, -1);
	glColor3f(0, 0, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(E_1);
	for (float a = 4.7; a <= 5.21; a += 0.001) {

		float X1 = X - cos(a) * R;
		float Y1 = Y - sin(a) * R;

		glTexCoord2d(0.7, 0.7);
		glVertex3d(X1, Y1, 0);
	}

	glEnd();
	//glNormal3dv(v);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glColor3f(0, 0, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(E2_1);
	for (float a = 4.7; a <= 5.21; a += 0.001) {

		float X1 = X - cos(a) * R;
		float Y1 = Y - sin(a) * R;

		glTexCoord2d(0.7, 0.7);
		glVertex3d(X1, Y1, 5);
	}

	glEnd();

#pragma endregion


   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}