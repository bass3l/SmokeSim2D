#include"Header.h"
#include<glut.h>
#include<stdio.h>

//initilizing static variables:
int Solver::velWin = 0;
int Solver::mx = 0;
int Solver::my = 0;
int Solver::omx = 0;
int Solver::omy = 0;
int Solver::mouseDown[3] = { 0 };
int Solver::winX = 512;
int Solver::winY = 512;
bool Solver::displayFlag = false;
System *Solver::sys = 0;

//when creating a solver we create our simulation system. 
Solver::Solver(){
		sys = new System();
}

void Solver::pre_display(void)
{
		glViewport(0, 0, winX, winY);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, 1.0, 0.0, 1.0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);
}

void Solver::post_display(void)
{
		glutSwapBuffers();
}


/*
----------------------------------------------------------------------
our drawing functions :
drawVelocity :   drawing only velocity field
drawDensity :    drawing only density field
drawCombinedDisplay : drawing both velocity and density fields on the same window
----------------------------------------------------------------------
*/
void Solver::drawVelocity(void)
	{

		int N = System::N;
		int i, j;
		float x, y, h;

		h = 1.0f / N;

		glColor3f(1.0f, 0.0f, 0.0f);
		glLineWidth(1.0f);

		glBegin(GL_LINES);

		for (i = 1; i <= N; i++) {
			x = (i - 0.5f)*h;
			for (j = 1; j <= N; j++) {
				y = (j - 0.5f)*h;
				
				glVertex2f(x, y);
				glVertex2f(x + sys->air->velocityGrid[i][j].u   , y + sys->air->velocityGrid[i][j].v);
			}
		}

		glEnd();
	}
void Solver::drawDensity(void)
	{
		int N = System::N;
		int i, j;
		float x, y, h, d00, d01, d10, d11;

		h = 1.0f / N;

		glBegin(GL_QUADS);

		for (i = 0; i <= N; i++) {
			x = (i - 0.5f)*h;
			for (j = 0; j <= N; j++) {
				y = (j - 0.5f)*h;

				d00 = sys->smoke->substanceGrid[i][j];
				d01 = sys->smoke->substanceGrid[i][j + 1];
				d10 = sys->smoke->substanceGrid[i + 1][j];
				d11 = sys->smoke->substanceGrid[i + 1][j + 1];


				glColor3f(d00, d00, d00); glVertex2f(x, y);
				glColor3f(d10, d10, d10); glVertex2f(x + h, y);
				glColor3f(d11, d11, d11); glVertex2f(x + h, y + h);
				glColor3f(d01, d01, d01); glVertex2f(x, y + h);
			}
		}

		glEnd();
	}
void Solver::drawCombinedDisplay(void){
		drawDensity();
		drawVelocity();
	}

/*
----------------------------------------------------------------------
relates mouse movements to forces sources
----------------------------------------------------------------------
*/

void Solver::getFromUI()
	{
		int i, j;
		int N = System::N;
		//?? don't know why clearing grids!!
		for (i = 0; i < N + 2; i++) {
			for (int j = 0; j < N + 2; j++)
			{
				sys->air->prevVelocityGrid[i][j].u = 0.0f;
				sys->air->prevVelocityGrid[i][j].v = 0.0f;
				sys->smoke->prevSubstanceGrid[i][j] = 0.0f;
				sys->temp->prevSubstanceGrid[i][j] = 0.0f;
			}
		}

		if (!mouseDown[0] && !mouseDown[2]) return;

		i = (int)((mx / (float)winX)*N);
		j = (int)(((winY - my) / (float)winY)*N);

		if (i<1 || i>N || j<1 || j>N) return;

		if (mouseDown[0]) {
			sys->air->prevVelocityGrid[i][j].u = System::force * (mx - omx);
			sys->air->prevVelocityGrid[i][j].v = System::force * (omy - my);
		}

		if (mouseDown[2]) {
			//adding smoke substances
			sys->smoke->prevSubstanceGrid[i][j] = System::substance;
			sys->temp->prevSubstanceGrid[i][j] = System::temperature;
		}

		omx = mx;
		omy = my;

		return;
	}

/*
----------------------------------------------------------------------
GLUT callback routines
----------------------------------------------------------------------
*/

void Solver::keyFunc(unsigned char key, int x, int y)
	{
		switch (key)
		{
		//clearing and resetting simulation
		case 'c':
		case 'C':
			sys->air->clearData();
			sys->smoke->clearData();
			sys->temp->clearData();
			System::dt = 0.1f;
			System::force = 5.0f;
			System::temperature = 30.0f;
			System::vortForce = 0.05f;
			Nature::visc = 0.0f;
			Nature::diffuse = 0.0f;
			System::buoyancyB = 0.15f;
			break;
		//tempreture controls
		case 't':
			System::temperature += 1.0f;
			break;
		case 'T':
			System::temperature -= 1.0f;
			break;
		//vorticity confinment controls
		case 'v':
			System::vortForce += 0.01f;
			break;
		case 'V' :
			System::vortForce -= 0.01f;
			break;
		case 'o' :
			if (System::vortFlag)
				System::vortFlag = false;
			else
				System::vortFlag = true;
			break;
        //veloctiy-density fields controls
		case 's':
		case 'S':
			if (displayFlag)
				displayFlag = false;
			else
				displayFlag = true;
			break;
		//time step controls
		case 'd':
			System::dt += 0.1f;
			break;
		case 'D' :
			System::dt -= 0.1f;
			break;
		//applied force controls
		case 'f' :
			System::force += 1.0f;
			break;
		case 'F' :
			System::force += 1.0f;
			break;
		//viscousity controls
		case 'y' :
			Nature::visc += 10000000.0f;
			break;
		case 'Y' :
			if (!(Nature::visc <= 0.0f))//making sure no negative viscousity
			     Nature::visc -= 5000000.0f;
			break;
		//diffuse controls
		case 'e' :
			Nature::diffuse = 100000.0f;
			break;
		case 'E' :
			if (!(Nature::diffuse <= 0.0f))//making sure no negative diffusion rate
			Nature::diffuse -= 100000.0f;
			break;
		//buoyancy controls
		case 'b' :
			System::buoyancyB += 0.1f;
			break;
		case 'B' :
			System::buoyancyB -= 0.1f;
			break;
		case 'u' :
			if (System::buoyFlag)
				System::buoyFlag = false;
			else
				System::buoyFlag = true;
			break;
		case 'q':
		case 'Q':
			delete(sys);
			exit(0);
			break;
		}
	}
void Solver::mouseFunc(int button, int state, int x, int y)
	{
		omx = mx = x;
		omy = my = y;

		mouseDown[button] = state == GLUT_DOWN; // pressing state
	}
void Solver::motionFunc(int x, int y)
	{
		mx = x;
		my = y;
	}
void Solver::reshapeFunc(int width, int height)
	{
		glutSetWindow(velWin);
		glutReshapeWindow(width, height);

		winX = width;
		winY = height;
	}
void Solver::idleFunc(void)
{

		getFromUI();
		sys->stepForward();


		glutSetWindow(velWin);
		glutPostRedisplay();
}

void Solver::displayFunc(void)
{
		pre_display();
		if (displayFlag)
			drawDensity();
		else
			drawCombinedDisplay();
		post_display();
}




/*
----------------------------------------------------------------------
open_glut_window --- open a glut compatible window and set callbacks
----------------------------------------------------------------------
*/	
/***glutMotionFunc : glutMotionFunc and glutPassiveMotionFunc set the motion and
*  passive motion callback respectively for the current window. The motion
*  callback for a window is called when the mouse moves within the window
*  while one or more mouse buttons are pressed. The passive motion callback
*  for a window is called when the mouse moves within the window while
*  no mouse buttons are pressed.

* The x and y callback parameters indicate the mouse location in window
* relative coordinates.

**glutKeyboardFunc : glutKeyboardFunc sets the keyboard callback for the current window.
* When a user types into the window, each key press generating an ASCII
* character will generate a keyboard callback. The key callback parameter
* is the generated ASCII character.

**glutMouseFunc : glutMouseFunc sets the mouse callback for the current window. When a
* user presses and releases mouse buttons in the window, each press and
* each release generates a mouse callback. The button parameter is one of
* GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or
* GLUT_RIGHT_BUTTON. For systems with only two mouse buttons, it may
* not be possible to generate GLUT_MIDDLE_BUTTON callback. For systems
* with a single mouse button, it may be possible to generate only a
* GLUT_LEFT_BUTTON callback. The state parameter is either GLUT_UP
* or GLUT_DOWN indicating whether the callback was due to a release or
* press respectively. The x and y callback parameters indicate the window
* relative coordinates when the mouse button state changed.

**glutReshapeFunc : is called when we reshape our window (resize it) and it will invoke our reshape_func which
* will give the simulation the new width and height of the window and stretch our grids accordingly.


*/
	

void Solver::openGlutWindow(void)
{
		

		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

		glutInitWindowPosition(750, 100);
		glutInitWindowSize(winX, winY);
		velWin = glutCreateWindow("Smoke Simulator | 2D Version");

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();

		pre_display();

		glutKeyboardFunc(keyFunc);
		glutMouseFunc(mouseFunc);
		glutMotionFunc(motionFunc);
		glutReshapeFunc(reshapeFunc);
		glutIdleFunc(idleFunc);
		glutDisplayFunc(displayFunc);

	}





