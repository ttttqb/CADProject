#include "EulerOp.h"
#include "glext.h"
#include <iostream>
#include <sstream>
#include <iomanip>
using std::stringstream;
using std::cout;
using std::endl;
using std::ends;


// GLUT CALLBACK functions
void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void keyboardCB(unsigned char key, int x, int y);
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);

void initGL();
int  initGLUT(int argc, char **argv);
bool initSharedMem();
void clearSharedMem();
void initLights();
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
void toOrtho();
void toPerspective();
GLuint draw1();
GLuint draw2();
void EulerBuild1();
void EulerBuild2(); 
void EulerBuild3();
void drawFace(GLUtesselator*,Face*);
//Loop* findLoop(Solid*, int, int, int);

// constants
const int   SCREEN_WIDTH = 800;
const int   SCREEN_HEIGHT = 600;
const float CAMERA_DISTANCE = 10.0f;
const int   TEXT_WIDTH = 8;
const int   TEXT_HEIGHT = 13;
const int   MAXN = 256;


// global variables
void *font = GLUT_BITMAP_8_BY_13;
int screenWidth;
int screenHeight;
bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
int drawMode;
int maxVertices;
int maxIndices;
GLuint listID[3][2];
bool face_wire;
int listSW;

EulerOp ops;
GLdouble quad[28][3] = { { -2,3,-2 },{ -2,0,-2 },{ 2,0,-2 },{ 2,3,-2 },
{ -2,3,0 },{ -2,0,0 },{ 2,0,0 },{ 2,3,0 },
{-1,2,0}, {-1,1,0},{1,1,0},{1,2,0},
{-1,2,-2},{-1,1,-2},{1,1,-2},{1,2,-2},
{-1,2,-2},{-1,1,-2},{-0.3,1,-2},{-0.3,2,-2},
{0.3,1,-2},{0.3,2,-2},{1,2,-2},{1,1,-2},
{-1,5,-1.5},{-1,3,-1.5},{1,3,-1.5},{1,5,-1.5} };
Vertex *v[MAXN];
//GLfloat color[16][3];
GLfloat rgb[6][3] = { {1,0,0},{0,1,0},{0,0,1},{1,1,0},{0,1,1},{1,0,1} };


int main(int argc, char **argv)
{
	// init global vars
	initSharedMem();

	// init GLUT and GL
	initGLUT(argc, argv);
	initGL();

	//三个演示样例
	EulerBuild1();
	listID[0][0] = draw1();
	listID[0][1] = draw2();
	ops.solidlist.clear();
	ops.vertexlist.clear();
	EulerBuild2();
	listID[1][0] = draw1();
	listID[1][1] = draw2();
	ops.solidlist.clear();
	ops.vertexlist.clear();
	EulerBuild3();
	listID[2][0] = draw1();
	listID[2][1] = draw2();
	ops.solidlist.clear();
	ops.vertexlist.clear();

	// the last GLUT call (LOOP)
	// window will be shown and display callback is triggered by events
	// NOTE: this call never return main().
	glutMainLoop(); /* Start GLUT event-processing loop */

	return 0;
}

void EulerBuild1()//纯欧拉操作建立带孔立方体
{

	for (auto i = 0; i < MAXN; i++) {
		v[i] = nullptr;
	}
	Solid* snow = nullptr;
	Halfedge* henow = nullptr;
	Loop* lnow = nullptr; Loop* lnow2;
	Face* fnow = nullptr;
	//长方体
	//后面
	snow = ops.mvfs(quad[0],  v[0],0);
	fnow = snow->firstFace;
	lnow = fnow->getoutLoop();
	ops.mev(lnow, v[0], v[3], quad[3],3);
	ops.mev(lnow, v[3], v[2], quad[2],2);
	ops.mev(lnow, v[2], v[1], quad[1],1);
	lnow = ops.mef(v[0], v[1], lnow);
	//左面
	ops.mev(lnow, v[0], v[4], quad[4],4);
	ops.mev(lnow, v[1], v[5], quad[5],5);
	lnow = ops.mef(v[4], v[5], lnow);
	//前面
	ops.mev(lnow, v[4], v[7], quad[7],7);
	ops.mev(lnow, v[5], v[6], quad[6],6);
	lnow = ops.mef(v[7], v[6], lnow);
	//上面
	lnow = ops.mef(v[3], v[7], lnow);
	//闭合
	lnow = ops.mef(v[2], v[6], lnow);
	//挖一个孔
	lnow = ops.findLoop(snow, 4, 5, 6);
	if (lnow == nullptr) {
		cout << "no Face existing" << endl;
		return ;
	}
	ops.mev(lnow, v[4], v[8], quad[8], 8);
	ops.mev(lnow, v[8], v[9], quad[9], 9);
	ops.mev(lnow, v[9], v[10], quad[10], 10);
	ops.mev(lnow, v[10], v[11], quad[11], 11);
	lnow = ops.mef(v[8], v[11], lnow);
	lnow = ops.findLoop(snow, 4, 5, 6);
	lnow = ops.kemr(v[4], v[8], lnow);
	//孔的上面
	lnow = ops.findLoop(snow, 8, 9, 10);
	ops.mev(lnow, v[8], v[12], quad[12], 12);
	ops.mev(lnow, v[11], v[15], quad[15], 15);
	lnow = ops.mef(v[15], v[12], lnow);
	//孔的右面
	ops.mev(lnow, v[10], v[14], quad[14], 14);
	lnow = ops.mef(v[14], v[15], lnow);
	//孔的下面
	ops.mev(lnow, v[9], v[13], quad[13], 13);
	lnow = ops.mef(v[13], v[14], lnow);
	//闭合
	lnow = ops.mef(v[12], v[13], lnow);
	//删除孔的底面作为外面的内环
	lnow = ops.findLoop(snow, 12, 13, 14);
	if (lnow == nullptr) {
		cout << "no Face existing" << endl;
		return ;
	}
	lnow2 = ops.findLoop(snow, 0, 1, 2);
	if (lnow2 == nullptr) {
		cout << "no Face existing" << endl;
		return ;
	}
	ops.kfmrh(lnow->face, lnow2->face);
}

void EulerBuild2()//扫成建立带两个孔的立方体
{
	for (auto i = 0; i < MAXN; i++) {
		v[i] = nullptr;
	}
	Solid* snow = nullptr;
	Halfedge* henow = nullptr;
	Loop* lnow = nullptr; Loop* lnow2;
	Face* fnow = nullptr;
	int vnum = 28;
	//长方形
	snow = ops.mvfs(quad[0], v[0], 0);
	fnow = snow->firstFace;
	lnow = fnow->getoutLoop();
	ops.mev(lnow, v[0], v[3], quad[3], 3);
	ops.mev(lnow, v[3], v[2], quad[2], 2);
	ops.mev(lnow, v[2], v[1], quad[1], 1);
	lnow = ops.mef(v[0], v[1], lnow);
	lnow2 = lnow;
	//左边一个洞
	snow = ops.mvfs(quad[16], v[16], 16);
	fnow = snow->firstFace;
	lnow = fnow->getoutLoop();
	ops.mev(lnow, v[16], v[17], quad[17], 17);
	ops.mev(lnow, v[17], v[18], quad[18], 18);
	ops.mev(lnow, v[18], v[19], quad[19], 19);
	lnow = ops.mef(v[16], v[19], lnow);
	
	snow = ops.kfmrh(lnow->face, lnow2->face);
	//右边一个洞
	snow = ops.mvfs(quad[20], v[20], 20);
	fnow = snow->firstFace;
	lnow = fnow->getoutLoop();
	ops.mev(lnow, v[20], v[21], quad[21], 21);
	ops.mev(lnow, v[21], v[22], quad[22], 22);
	ops.mev(lnow, v[22], v[23], quad[23], 23);
	lnow = ops.mef(v[23], v[20], lnow);
	snow = ops.kfmrh(lnow->face, lnow2->face);
	//扫成
	GLdouble vector[3] = { 0,0,1 };

	
	ops.sweep(lnow2->face, vector, 2, vnum);
	//lnow = ops.findLoop(snow, 16, 17, 18);
	//lnow2 = ops.findLoop(snow, 0, 1, 2);
	//ops.kfmrh(lnow->face, lnow2->face);
	//lnow = ops.findLoop(snow, 20, 21, 22);
	//lnow2 = ops.findLoop(snow, 0, 1, 2);
	//ops.kfmrh(lnow->face, lnow2->face);
}

void EulerBuild3()//两个扫成体的合并
{
	for (auto i = 0; i < MAXN; i++) {
		v[i] = nullptr;
	}
	Solid* snow = nullptr;
	Halfedge* henow = nullptr;
	Loop* lnow = nullptr; Loop* lnow2;
	Face* fnow = nullptr;
	int vnum = 28;
	//下方的带孔立方体
	snow = ops.mvfs(quad[0], v[0], 0);
	fnow = snow->firstFace;
	lnow = fnow->getoutLoop();
	ops.mev(lnow, v[0], v[3], quad[3], 3);
	ops.mev(lnow, v[3], v[2], quad[2], 2);
	ops.mev(lnow, v[2], v[1], quad[1], 1);
	lnow = ops.mef(v[0], v[1], lnow);
	lnow2 = lnow;

	//snow = ops.mvfs(quad[16], v[16], 16);
	//fnow = snow->firstFace;
	//lnow = fnow->getoutLoop();
	//ops.mev(lnow, v[16], v[17], quad[17], 17);
	//ops.mev(lnow, v[17], v[18], quad[18], 18);
	//ops.mev(lnow, v[18], v[19], quad[19], 19);
	//lnow = ops.mef(v[16], v[19], lnow);

	//snow = ops.kfmrh(lnow->face, lnow2->face);


	GLdouble vector[3] = { 0,0,1 };
	ops.sweep(lnow->face, vector, 2, vnum);
	for (int i = 28; i < vnum; i++)
		cout << ops.vertexlist[i]->getx() << " " << ops.vertexlist[i]->gety()
		<< " " << ops.vertexlist[i]->getz() 
		<< " " << i << endl;
	lnow2 = ops.findLoop(snow, 0, 3, 29);
	//上方的小立方体
	snow = ops.mvfs(quad[24], v[24], 24);
	fnow = snow->firstFace;
	lnow = fnow->getoutLoop();
	ops.mev(lnow, v[24], v[25], quad[25], 25);
	ops.mev(lnow, v[25], v[26], quad[26], 26);
	ops.mev(lnow, v[26], v[27], quad[27], 27);
	lnow = ops.mef(v[27], v[24], lnow);

	ops.sweep(lnow->face, vector, 1, vnum);
	for (int i = 32; i < vnum; i++)
		cout << ops.vertexlist[i]->getx() << " " << ops.vertexlist[i]->gety()
		<< " " << ops.vertexlist[i]->getz()
		<< " " << i << endl;
	lnow = ops.findLoop(snow, 25, 26, 34);
	//两个立方体合并
	snow = ops.kfmrh(lnow->face, lnow2->face);
}

GLuint draw1()//面体绘制
{
	GLuint id = glGenLists(1);
	if (!id) return id;
	GLUtesselator *tess = gluNewTess();
	if (!tess) return 0;

	gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK *)())glBegin);
	gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK *)())glEnd);
	gluTessCallback(tess, GLU_TESS_VERTEX, (void(__stdcall*)())glVertex3dv);

	int cnt = 0;

	glNewList(id, GL_COMPILE);
	for (auto s = ops.solidlist.begin(); s != ops.solidlist.end(); s++) {
		Face *f;
		f = s->second->firstFace;
		while (f != nullptr) {
			glColor3f(rgb[cnt % 6][0], rgb[cnt % 6][1], rgb[cnt % 6][2]), cnt++;
			drawFace(tess, f);
			f = f->nextf;
		}
		//drawFace(tess, f);
	}
	glEndList();

	gluDeleteTess(tess);

	return id;
}

void drawFace(GLUtesselator* tess, Face* face)//画面
{
	Loop* l = face->getoutLoop();
	Halfedge* he1, *he2;
	gluTessBeginPolygon(tess, 0);
		gluTessBeginContour(tess);//最外环
			he1 = l->firsthe;
			he2 = he1;
			while (he2->nexthe != he1) {
				gluTessVertex(tess, he2->startv->getCord(), he2->startv->getCord());
				he2 = he2->nexthe;
			}
			gluTessVertex(tess, he2->startv->getCord(), he2->startv->getCord());
		gluTessEndContour(tess);
		if (!face->looplist.empty()) {
			for (auto ll = face->looplist.begin(); ll != face->looplist.end(); ll++) {//内环
				gluTessBeginContour(tess);
					he1 = (*ll)->firsthe;
					he2 = he1;
					while (he2->nexthe != he1) {
						gluTessVertex(tess, he2->startv->getCord(), he2->startv->getCord());
						he2 = he2->nexthe;
					}
					gluTessVertex(tess, he2->startv->getCord(), he2->startv->getCord());
				gluTessEndContour(tess);
			}
		}
	gluTessEndPolygon(tess);
}


GLuint draw2()//线框绘制
{
	GLuint id = glGenLists(1);
	if (!id) return id;

	glNewList(id, GL_COMPILE);
	glLineWidth(2);

	glBegin(GL_LINES);
	for (auto s = ops.solidlist.begin(); s != ops.solidlist.end(); s++) {
		Edge *e = s->second->firstEdge;
		while (e != nullptr) {
			glVertex3d(e->lhe->startv->getx(), e->lhe->startv->gety(), e->lhe->startv->getz());
			glVertex3d(e->rhe->startv->getx(), e->rhe->startv->gety(), e->rhe->startv->getz());
			e = e->nexte;
		}
	}
	glEnd();
	glEndList();

	return id;
}

///////////////////////////////////////////////////////////////////////////////
// initialize GLUT for windowing
///////////////////////////////////////////////////////////////////////////////
int initGLUT(int argc, char **argv)
{
	// GLUT stuff for windowing
	// initialization openGL window.
	// it is called before any other GLUT routine
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode

	glutInitWindowSize(screenWidth, screenHeight);  // window size

	glutInitWindowPosition(100, 100);               // window location

													// finally, create a window with openGL context
													// Window will not displayed until glutMainLoop() is called
													// it returns a unique ID
	int handle = glutCreateWindow(argv[0]);     // param is the title of window

												// register GLUT callback functions
	glutDisplayFunc(displayCB);
	//glutIdleFunc(displayCB);
	glutTimerFunc(16, timerCB, 1);             // redraw only every given millisec
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);

	return handle;
}



///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void initGL()
{
	glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

												// enable /disable features
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_CULL_FACE);

	// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(0, 0, 0, 0);                   // background color
	glClearStencil(0);                          // clear stencil buffer
	glClearDepth(1.0f);                         // 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);

	initLights();
}






///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem()
{
	screenWidth = SCREEN_WIDTH;
	screenHeight = SCREEN_HEIGHT;

	mouseLeftDown = mouseRightDown = mouseMiddleDown = false;
	mouseX = mouseY = 0;

	cameraAngleX = cameraAngleY = 0.0f;
	cameraDistance = CAMERA_DISTANCE;

	drawMode = 0; // 0:fill, 1: wireframe, 2:points
	maxVertices = maxIndices = 0;

	listSW = 0;
	face_wire = true;
	return true;
}



///////////////////////////////////////////////////////////////////////////////
// clean up global vars
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
}



///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void initLights()
{
	// set up light colors (ambient, diffuse, specular)
	GLfloat lightKa[] = { .2f, .2f, .2f, 1.0f };  // ambient light
	GLfloat lightKd[] = { .7f, .7f, .7f, 1.0f };  // diffuse light
	GLfloat lightKs[] = { 1, 1, 1, 1 };           // specular light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

	// position the light
	float lightPos[4] = { 0, 0, 20, 1 }; // positional light
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}



///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}


///////////////////////////////////////////////////////////////////////////////
// set projection matrix as orthogonal
///////////////////////////////////////////////////////////////////////////////
void toOrtho()
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

	// set orthographic viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



///////////////////////////////////////////////////////////////////////////////
// set the projection matrix as perspective
///////////////////////////////////////////////////////////////////////////////
void toPerspective()
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)(screenWidth) / screenHeight, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

																			   // switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}









//=============================================================================
// CALLBACKS
//=============================================================================

void displayCB()
{
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// save the initial ModelView matrix before modifying ModelView matrix
	glPushMatrix();

	// tramsform camera
	glTranslatef(0, 0, -cameraDistance);
	glRotatef(cameraAngleX, 1, 0, 0);   // pitch
	glRotatef(cameraAngleY, 0, 1, 0);   // heading

	if (face_wire)
		glCallList(listID[listSW][0]);
	else
		glCallList(listID[listSW][1]);

	glPopMatrix();

	glutSwapBuffers();
}


void reshapeCB(int w, int h)
{
	screenWidth = w;
	screenHeight = h;
	toPerspective();
}


void timerCB(int millisec)
{
	glutTimerFunc(millisec, timerCB, millisec);
	glutPostRedisplay();
}


void keyboardCB(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESCAPE
		clearSharedMem();
		exit(0);
		break;

	case 'd': // switch rendering modes (fill -> wire -> point)
	case 'D':
		drawMode = ++drawMode % 3;
		if (drawMode == 0)        // fill mode
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_DEPTH_TEST);
			//glEnable(GL_CULL_FACE);
		}
		else if (drawMode == 1)  // wireframe mode
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_DEPTH_TEST);
			//glDisable(GL_CULL_FACE);
		}
		else                    // point mode
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			glDisable(GL_DEPTH_TEST);
			//glDisable(GL_CULL_FACE);
		}
		break;
	case 's': //Brep生成的线框
	case 'S':
		face_wire = !face_wire;
		break;
	case 'a': //样例选择
	case 'A':
		listSW = ++listSW %3;

	default:
		;
	}
}


void mouseCB(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftDown = false;
	}

	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseRightDown = true;
		}
		else if (state == GLUT_UP)
			mouseRightDown = false;
	}

	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseMiddleDown = true;
		}
		else if (state == GLUT_UP)
			mouseMiddleDown = false;
	}
}


void mouseMotionCB(int x, int y)
{
	if (mouseLeftDown)
	{
		cameraAngleY += (x - mouseX);
		cameraAngleX += (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	if (mouseRightDown)
	{
		cameraDistance -= (y - mouseY) * 0.2f;
		mouseY = y;
	}
}
