// Our Simulation System:
// @members : 
//     N                    our simulation dimensions .
//     dt                   our simulation time step .
//     force                force quantity applied to the simulation when invoking apply forces routines (interacting).
//     source               density quantity applied to the simulation when invoking apply density routines (interacting).
//     buoyancyA, buoyancyB parameters to control the buoyancy force.
//     vortForce            variable to control vorticity confinment force.



#ifndef SYSTEM_CPP
#define SYSTEM_CPP

#include"Header.h"


const int System::N = 128;
float System::dt = 0.1f;
float System::force = 5.0f;
const float System::substance = 100.f;
float System::temperature = 30.0f;
const float System::buoyancyA = 0.000625f;
float System::buoyancyB = 0.15f;
float System::vortForce = 0.05;
bool System::vortFlag = true;
bool System::buoyFlag = true;

// the system that we simulate has a smoke substance, temperature substance and an air flux 
System::System(){
		smoke = new Substance();
		air = new Flux();
		temp = new Substance();
}

//this method is used to step forward the whole simulation
void System::stepForward(){
	    if (buoyFlag)
 	         addBuoyForce(air, temp, smoke);//adding the buoancy force. 
	    
		addForce2Flux(*air);//adding the external forces after advancing dt. 
	    addForce2Dens(*smoke);//adding the external substance after advancing dt.
		addForce2Temp(*temp);//no need to add force on temp routine 'cuz no external temp we add
		
		air->stepForward();        //flux step
		smoke->stepForward(*air);  //smoke step using the flux which the simulation has
		temp->stepForward(*air);   //temp step  using the flux which the simulation has
}
	
void System::addForce2Dens(Substance substance){
	int N = System::N;
	float dt = System::dt;

	for (int i = 0; i < N + 2; i++)
	{
		for (int j = 0; j < N + 2; j++)
		{
			substance.substanceGrid[i][j] += substance.prevSubstanceGrid[i][j] * dt;
		}
	}

}

void System::addForce2Flux(Flux flux){

	int N = System::N;
	float dt = System::dt;

	for (int i = 0; i < N + 2; i++)
	{
		for (int j = 0; j < N + 2; j++)
		{
			flux.velocityGrid[i][j].u += flux.prevVelocityGrid[i][j].u * dt;
			flux.velocityGrid[i][j].v += flux.prevVelocityGrid[i][j].v * dt;
		}
	}

}

void System::addForce2Temp(Substance temp){
	int N = System::N;
	float dt = System::dt;

	for (int i = 0; i < N + 2; i++)
	{
		for (int j = 0; j < N + 2; j++)
		{
			temp.substanceGrid[i][j] += temp.prevSubstanceGrid[i][j] * dt;
		}
	}
}

void System::addBuoyForce(Flux *flux,Substance *temp,Substance *smoke){
	int N = System::N;
	float dt = System::dt;

	//first we calculate average temp: ( ambient temp )
	float avgTemp = 0.0f;
	for (int i = 0; i < N + 2; i++)
	{
		for (int j = 0; j < N + 2; j++)
		{
			avgTemp += temp->substanceGrid[i][j];
		}
	}
	avgTemp /= ((N+2)*(N+2));
	float buoyancyForce = 0.0f;
	for (int i = 0; i < N + 2 ; i++)
	{
		for (int j = 0; j < N + 2; j++)
		{
			buoyancyForce = -buoyancyA * smoke->substanceGrid[i][j] + buoyancyB * (temp->substanceGrid[i][j] - avgTemp);
			flux->prevVelocityGrid[i][j].v += buoyancyForce;
		}
	}

}
#endif