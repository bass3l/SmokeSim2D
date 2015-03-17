#include<stdio.h>
#include"Header.h"
#include<glut.h>

using namespace std;

int main(int argc,char ** argv) {

	glutInit(&argc, argv);
	fprintf(stderr, "|||DAMASCUS University - FITE 2014-2015 1st semister Scientific Calculations|||\n** Real Simulation of Smoke\n> 't' increases Substance Temperature\n> 'T' decreases Substance Temperature\n> 'b' increases the whole system temperature\n> 'B' decreases the whole system temperature\n> 'u' Buoyancy Force on\\off\n> 'v' increases vorticity confinment force\n> 'V' decreases Vorticity Confinment force\n> 'o' Vorticity Confinment on\\off\n> 'c' clears simulation\n> 's' switches velocity field on\\off\n> 'd' increases time step\n> 'D' decreases time step\n> 'f' increases force applied by mouse\n> 'F' decreases force applied\n> 'e' increases diffusion rate\n> 'E' decreases diffusion rate\n> 'y' increases viscosity\n> 'Y' decreases viscosity\n");
		
                   		
	

	// creating a solver :
	Solver *solver = new Solver();
	//preparing OpenGL callback functions and idle function
    solver->openGlutWindow();
	//start our idle function loop
	glutMainLoop();
	exit(0);
}