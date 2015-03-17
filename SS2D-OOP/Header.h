#ifndef HEADER_H
#define HEADER_H

#include"Nature.h"

//velocity struct
class Velocity {
public:
	float u;
	float v;
	
};

class Flux : public Nature {

public:
	//members:
	Velocity ** velocityGrid;
	Velocity ** prevVelocityGrid;

	//methods:
	void stepForward();
	void clearData();
	Flux();
private:
	void swap();
	//add force routines moved to System Class
	//void addForce();
	void advect();
	void setBoundary();
	void linearSolve(int N, float a, float c);
	void linearSolve2(int N, float a, float c);
	void diffuse();
	void project();

	float calculateCurl(int i, int j);
	void addVorticityConfinment();
};



class Substance : public Nature {

public:
	//members:
	float **substanceGrid;
	float **prevSubstanceGrid;

	//methods:
	Substance();
	void stepForward(Flux flux);
	void clearData();

private:
	void swap();
	//add force routines moved to System class
	//void addForce();
	void diffuse();
	void advect(Flux flux);
	void setBoundary();
	void linearSolve(float a, float c);

};


class System {

public:
	//members:
	static const int N;
	static float dt;
	static float force;
	static const float substance;
	static float temperature;
	static const float buoyancyA;
	static float buoyancyB;
	static float vortForce;
	static bool vortFlag, buoyFlag;
	Substance * smoke;
	Flux * air;
	Substance * temp;
	//methods:
	System();
	void stepForward();
	static void addForce2Dens(Substance sub);
	static void addForce2Flux(Flux flux);
	static void addForce2Temp(Substance temp);
	static void addBuoyForce(Flux *flux,Substance *temp,Substance *smoke);
};



class Solver {

private:
	//methods:
	static void displayFunc();
	static void drawCombinedDisplay();
	static void drawDensity();
	static void drawVelocity();
	static void getFromUI();
	static void idleFunc();
	static void keyFunc(unsigned char key, int x, int y);
	static void motionFunc(int x, int y);
	static void mouseFunc(int button, int state, int x, int y);
	static void post_display();
	static void pre_display();
	static void reshapeFunc(int width, int height);

	//members:
	static int velWin;
	static int mx, my, omx, omy;
	static int mouseDown[];
	static bool displayFlag;
	static System *sys;
	static int winX, winY;
public : 
	Solver();
	static void openGlutWindow(); // this is the only method should be public with the constructor ofcourse.
	
};









#endif