#include"Header.h"


Substance::Substance(){
		Nature();
		int N = System::N;

		//allocating data
		substanceGrid = new float*[N + 2];
		prevSubstanceGrid = new float*[N + 2];
		for (int i = 0; i < N + 2; i++)
		{
			substanceGrid[i] = new float[N + 2];
			prevSubstanceGrid[i] = new float[N + 2];
		}

		clearData();
	}
void Substance::stepForward(Flux flux){
      	//addForce invoking moved to System class right b4 the invoking of Substance::stepForward()
	    //addForce();
		swap();
		diffuse();
		swap();
		advect(flux);
	}
void Substance::clearData(){

		int N = System::N;

		for (int i = 0; i < N + 2; i++)
		{
			for (int j = 0; j < N + 2; j++)
			{
				substanceGrid[i][j] = 0.0f;
				prevSubstanceGrid[i][j] = 0.0f;
			}
		}
	}
	
void Substance::swap(){
		int N = System::N;

		float ** tempGrid = new float*[N + 2];
		for (int i = 0; i < N + 2; i++)
		{
			tempGrid[i] = new float[N + 2];
		}

		for (int i = 0; i < N + 2; i++)
		{
			for (int j = 0; j < N + 2; j++)
			{
				tempGrid[i][j] = substanceGrid[i][j];
				substanceGrid[i][j] = prevSubstanceGrid[i][j];
				prevSubstanceGrid[i][j] = tempGrid[i][j];
			}
		}
		//deleting temp grid and clearing some space 
		for (int i = 0; i < N + 2; i++)
		{
			delete(tempGrid[i]);
		}
		delete(tempGrid);

	}



void Substance::diffuse(){
		float a = System::dt * Nature::diffuse / (System::N*System::N);
		linearSolve(a, 1 + 4 * a);
}

void Substance::advect(Flux flux){
		int N = System::N;
		int i0, j0, i1, j1;
		float x, y, s0, t0, s1, t1, dt0;

		dt0 = System::dt*N;
		for (int i = 1; i <= N; i++)
		{
			for (int j = 1; j <= N; j++)
			{
				x = i - dt0 * flux.velocityGrid[i][j].u;
				y = j - dt0 * flux.velocityGrid[i][j].v;

				if (x<0.5f) //not getting out of the grid 
					x = 0.5f;
				if (x>N + 0.5f) //not getting out of the grid  
					x = N + 0.5f;
				i0 = (int)x;
				i1 = i0 + 1;

				if (y<0.5f) //not getting out of the grid 
					y = 0.5f;
				if (y>N + 0.5f) //not getting out of the grid 
					y = N + 0.5f;
				j0 = (int)y;
				j1 = j0 + 1;


				s1 = x - i0;
				s0 = 1 - s1;
				t1 = y - j0;
				t0 = 1 - t1;

				substanceGrid[i][j] = s0*(t0*prevSubstanceGrid[i0][j0] + t1*prevSubstanceGrid[i0][j1]) +
					s1*(t0*prevSubstanceGrid[i1][j0] + t1*prevSubstanceGrid[i1][j1]);
			}
		}
		setBoundary();
	}

void Substance::setBoundary(){

		int N = System::N;
		for (int i = 1; i <= N; i++)
		{
			//[0][i] iterating left border
			substanceGrid[0][i] = substanceGrid[1][i];
			//[N+1][i] iterating right border
			substanceGrid[N + 1][i] = substanceGrid[N][i];

			//[i][0] iterating bottom border
			substanceGrid[i][0] = substanceGrid[i][1];
			//[i][N+1] iterating right border
			substanceGrid[i][N + 1] = substanceGrid[i][N];
		}

		//corners boundaries
		substanceGrid[0][0] = 0.5f *(substanceGrid[1][0] + substanceGrid[0][1]);
		substanceGrid[0][N + 1] = 0.5f *(substanceGrid[0][N] + substanceGrid[1][N + 1]);
		substanceGrid[N + 1][0] = 0.5f *(substanceGrid[N][0] + substanceGrid[N + 1][1]);
		substanceGrid[N + 1][N + 1] = 0.5f *(substanceGrid[N][N + 1] + substanceGrid[N + 1][N]);

	}

void Substance::linearSolve(float a, float c){

		int N = System::N;
		for (int i = 0; i < 20; i++)
		{
			for (int j = 1; j <= N; j++)
			{
				for (int k = 1; k <= N; k++)
				{
					substanceGrid[j][k] = (prevSubstanceGrid[j][k] + a*(substanceGrid[j + 1][k] + substanceGrid[j - 1][k] + substanceGrid[j][k + 1] + substanceGrid[j][k - 1])) / c;
				}
			}
			setBoundary();
		}
}



