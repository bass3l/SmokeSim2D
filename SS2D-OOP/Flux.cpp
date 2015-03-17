#include"Header.h"
#include<math.h>
Flux::Flux(){
	Nature();
	int N = System::N;

	//allocating data
	velocityGrid = new Velocity*[N + 2];
	prevVelocityGrid = new Velocity*[N + 2];
	for (int i = 0; i < N + 2; i++)
	{
		velocityGrid[i] = new Velocity[N + 2];
		prevVelocityGrid[i] = new Velocity[N + 2];
	}

	clearData();
}


void Flux::stepForward(){
	if (System::vortFlag)
	     addVorticityConfinment();
	swap();
	diffuse();
	project();
	swap();
	advect();
	project();

}



void Flux::clearData(){

	int N = System::N;
	for (int i = 0; i < N + 2; i++)
	{
		for (int j = 0; j < N + 2; j++)
		{
			velocityGrid[i][j].u = 0.0f;
			velocityGrid[i][j].v = 0.0f;

			prevVelocityGrid[i][j].u = 0.0f;
			prevVelocityGrid[i][j].v = 0.0f;
		}
	}
}
void Flux::swap(){
		int N = System::N;
		Velocity **tempGrid = new Velocity*[N+2];
		for (int i = 0; i < N + 2; i++)
		{
			tempGrid[i] = new Velocity[N + 2];
		}

		for (int i = 0; i < N + 2; i++)
		{
			for (int j = 0; j < N + 2; j++)
			{
				tempGrid[i][j].u = velocityGrid[i][j].u;
				tempGrid[i][j].v = velocityGrid[i][j].v;

				velocityGrid[i][j].u = prevVelocityGrid[i][j].u;
				velocityGrid[i][j].v = prevVelocityGrid[i][j].v;

				prevVelocityGrid[i][j].u = tempGrid[i][j].u;
				prevVelocityGrid[i][j].v = tempGrid[i][j].v;
			}
		}


		for (int i = 0; i < N + 2; i++)
		{
				delete(tempGrid[i]);
		}
		delete(tempGrid);
	}
	

void Flux::advect(){
		int N = System::N;
		int i0, j0, i1, j1;
		float x, y, s0, t0, s1, t1, dt0;

		dt0 = System::dt*N;
		for (int i = 1; i <= N; i++)
		{
			for (int j = 1; j <= N; j++)
			{
				x = i - dt0 * prevVelocityGrid[i][j].u;
				y = j - dt0 * prevVelocityGrid[i][j].v;

				if (x < 0.5f) //not getting out of the grid 
					x = 0.5f;
				if (x > N + 0.5f) //not getting out of the grid  
					x = N + 0.5f;
				i0 = (int)x;
				i1 = i0 + 1;

				if (y < 0.5f) //not getting out of the grid 
					y = 0.5f;
				if (y > N + 0.5f) //not getting out of the grid 
					y = N + 0.5f;
				j0 = (int)y;
				j1 = j0 + 1;


				s1 = x - i0;
				s0 = 1 - s1;
				t1 = y - j0;
				t0 = 1 - t1;

				velocityGrid[i][j].u = s0*(t0*prevVelocityGrid[i0][j0].u + t1*prevVelocityGrid[i0][j1].u) +
					s1*(t0*prevVelocityGrid[i1][j0].u + t1*prevVelocityGrid[i1][j1].u);
				velocityGrid[i][j].v = s0*(t0*prevVelocityGrid[i0][j0].v + t1*prevVelocityGrid[i0][j1].v) +
					s1*(t0*prevVelocityGrid[i1][j0].v + t1*prevVelocityGrid[i1][j1].v);
			}
		}
		setBoundary();

	}

void Flux::setBoundary(){

		int N = System::N;

		for (int i = 1; i <= N; i++)
		{
			//**** setting sides' boundary:
			//[0][i] iterating left border
			velocityGrid[0][i].v = velocityGrid[1][i].v;
			//[N][i] iterating right border
			velocityGrid[N + 1][i].v = velocityGrid[N][i].v;

			//[i][0] iterating bottom border
			velocityGrid[i][0].v = -velocityGrid[i][1].v;
			//[i][N+1] iterating top border
			velocityGrid[i][N + 1].v = -velocityGrid[i][N].v;

			//**** setting roof and cieling boundary:
		    //[0][i] iterating left border
			velocityGrid[0][i].u = -velocityGrid[1][i].u;
			//[N-1][i] iterating right border
			velocityGrid[N + 1][i].u = -velocityGrid[N][i].u;

			//[i][0] iterating bottom border
			velocityGrid[i][0].u = velocityGrid[i][1].u;
			//[i][N-1] iterating top border
			velocityGrid[i][N + 1].u = velocityGrid[i][N].u;

		}

		//****setting corner boundaries:
		velocityGrid[0][0].v = 0.5f *(velocityGrid[1][0].v + velocityGrid[0][1].v);
		velocityGrid[0][N + 1].v = 0.5f *(velocityGrid[0][N].v + velocityGrid[1][N + 1].v);
		velocityGrid[N + 1][0].v = 0.5f *(velocityGrid[N][0].v + velocityGrid[N + 1][1].v);
		velocityGrid[N + 1][N + 1].v = 0.5f *(velocityGrid[N][N + 1].v + velocityGrid[N + 1][N].v);

		velocityGrid[0][0].u = 0.5f *(velocityGrid[1][0].u + velocityGrid[0][1].u);
		velocityGrid[0][N + 1].u = 0.5f *(velocityGrid[0][N].u + velocityGrid[1][N + 1].u);
		velocityGrid[N + 1][0].u = 0.5f *(velocityGrid[N][0].u + velocityGrid[N + 1][1].u);
		velocityGrid[N + 1][N + 1].u = 0.5f *(velocityGrid[N][N + 1].u + velocityGrid[N + 1][N].u);

	}

void Flux::linearSolve(int N, float a, float c){

		for (int i = 0; i < 20; i++)
		{
			//for every voxel
			for (int j = 1; j <= N; j++)
			{
				for (int k = 1; k <= N; k++)
				{
					velocityGrid[j][k].u = (prevVelocityGrid[j][k].u + a*(velocityGrid[j + 1][k].u + velocityGrid[j - 1][k].u + velocityGrid[j][k + 1].u + velocityGrid[j][k - 1].u)) / c;

					velocityGrid[j][k].v = (prevVelocityGrid[j][k].v + a*(velocityGrid[j + 1][k].v + velocityGrid[j - 1][k].v + velocityGrid[j][k + 1].v + velocityGrid[j][k - 1].v)) / c;
				}
			}
			setBoundary();
		}
	}


void Flux::linearSolve2(int N, float a, float c){
		for (int i = 0; i < 20; i++)
		{
			for (int j = 1; j <= N; j++)
			{
				for (int k = 1; k <= N; k++)
				{
					prevVelocityGrid[j][k].u = (prevVelocityGrid[j][k].v + a*(prevVelocityGrid[j + 1][k].u + prevVelocityGrid[j - 1][k].u + prevVelocityGrid[j][k + 1].u + prevVelocityGrid[j][k - 1].u)) / c;
				}
			}
			for (int i = 1; i <= N; i++)
			{

				prevVelocityGrid[0][i].v = prevVelocityGrid[1][i].v;
				prevVelocityGrid[N + 1][i].v = prevVelocityGrid[N][i].v;
				prevVelocityGrid[i][0].v = prevVelocityGrid[i][1].v;
				prevVelocityGrid[i][N + 1].v = prevVelocityGrid[i][N].v;
				prevVelocityGrid[0][i].u = prevVelocityGrid[1][i].u;
				prevVelocityGrid[N + 1][i].u = prevVelocityGrid[N][i].u;
				prevVelocityGrid[i][0].u = prevVelocityGrid[i][1].u;
				prevVelocityGrid[i][N + 1].u = prevVelocityGrid[i][N].u;
				prevVelocityGrid[0][0].v = 0.5f *(prevVelocityGrid[1][0].v + prevVelocityGrid[0][1].v);
				prevVelocityGrid[0][N + 1].v = 0.5f *(prevVelocityGrid[0][N].v + prevVelocityGrid[1][N + 1].v);
				prevVelocityGrid[N + 1][0].v = 0.5f *(prevVelocityGrid[N][0].v + prevVelocityGrid[N + 1][1].v);
				prevVelocityGrid[N + 1][N + 1].v = 0.5f *(prevVelocityGrid[N][N + 1].v + prevVelocityGrid[N + 1][N].v);
				prevVelocityGrid[0][0].u = 0.5f *(prevVelocityGrid[1][0].u + prevVelocityGrid[0][1].u);
				prevVelocityGrid[0][N + 1].u = 0.5f *(prevVelocityGrid[0][N].u + prevVelocityGrid[1][N + 1].u);
				prevVelocityGrid[N + 1][0].u = 0.5f *(prevVelocityGrid[N][0].u + prevVelocityGrid[N + 1][1].u);
				prevVelocityGrid[N + 1][N + 1].u = 0.5f *(prevVelocityGrid[N][N + 1].u + prevVelocityGrid[N + 1][N].u);
			}
		}
	}

void Flux::diffuse(){
		float a = System::dt * Nature::visc / (System::N*System::N);
		linearSolve(System::N, a, 1 + 4*a);
	}

void Flux::project(){
		int N = System::N;

		for (int i = 1; i <= N; i++){
			for (int j = 1; j <= N; j++)
			{
				prevVelocityGrid[i][j].v = -0.5f*(velocityGrid[i + 1][j].u - velocityGrid[i - 1][j].u + velocityGrid[i][j + 1].v - velocityGrid[i][j - 1].v) / N;
				prevVelocityGrid[i][j].u = 0;
			}
		}

		//setting Boundary on the dummy grid
		for (int i = 1; i <= N; i++)
		{

			prevVelocityGrid[0][i].v = prevVelocityGrid[1][i].v;
			prevVelocityGrid[N + 1][i].v = prevVelocityGrid[N][i].v;
			prevVelocityGrid[i][0].v = prevVelocityGrid[i][1].v;
			prevVelocityGrid[i][N + 1].v = prevVelocityGrid[i][N].v;
			prevVelocityGrid[0][i].u = prevVelocityGrid[1][i].u;
			prevVelocityGrid[N + 1][i].u = prevVelocityGrid[N][i].u;
			prevVelocityGrid[i][0].u = prevVelocityGrid[i][1].u;
			prevVelocityGrid[i][N + 1].u = prevVelocityGrid[i][N].u;

			prevVelocityGrid[0][0].v = 0.5f *(prevVelocityGrid[1][0].v + prevVelocityGrid[0][1].v);
			prevVelocityGrid[0][N + 1].v = 0.5f *(prevVelocityGrid[0][N].v + prevVelocityGrid[1][N + 1].v);
			prevVelocityGrid[N + 1][0].v = 0.5f *(prevVelocityGrid[N][0].v + prevVelocityGrid[N + 1][1].v);
			prevVelocityGrid[N + 1][N + 1].v = 0.5f *(prevVelocityGrid[N][N + 1].v + prevVelocityGrid[N + 1][N].v);
			prevVelocityGrid[0][0].u = 0.5f *(prevVelocityGrid[1][0].u + prevVelocityGrid[0][1].u);
			prevVelocityGrid[0][N + 1].u = 0.5f *(prevVelocityGrid[0][N].u + prevVelocityGrid[1][N + 1].u);
			prevVelocityGrid[N + 1][0].u = 0.5f *(prevVelocityGrid[N][0].u + prevVelocityGrid[N + 1][1].u);
			prevVelocityGrid[N + 1][N + 1].u = 0.5f *(prevVelocityGrid[N][N + 1].u + prevVelocityGrid[N + 1][N].u);
		}
		//here we use another implementation of linear solve, we want here to solve u to v component on the same grid
		//the idea here that we use the u,v components as dummy arrays
		linearSolve2(N, 1, 4);

		for (int i = 1; i <= N; i++){
			for (int j = 1; j <= N; j++)
			{
				velocityGrid[i][j].u -= 0.5f*N*(prevVelocityGrid[i + 1][j].u - prevVelocityGrid[i - 1][j].u);
				velocityGrid[i][j].v -= 0.5f*N*(prevVelocityGrid[i][j + 1].u - prevVelocityGrid[i][j - 1].u);
			}

		}
		
		setBoundary();

	}


float Flux::calculateCurl(int i,int j){
	float du_dy = (velocityGrid[i+1][j].u - velocityGrid[i-1][j].u) * 0.5f;
	float dv_dx = (velocityGrid[i][j + 1].v - velocityGrid[i][j - 1].v) * 0.5f;
	return du_dy - dv_dx;
}

void Flux::addVorticityConfinment(){
	int N = System::N;
	float ** curl = new float*[N + 2];
	float dw_dx, dw_dy;
	float length;
	


	for (int i = 0; i < N + 2; i++)
	{
		curl[i] = new float[N + 2];
	}
	for (int i = 0; i < N + 2; i++)
	{
		for (int j = 0; j < N + 2; j++)
		{
			curl[i][j] = 0.0f;
		}
	}

	//for each cell
	for (int i = 1; i <=N; i++)
	{
		for (int j = 1; j <=N; j++)
		{
			float c = calculateCurl(i, j);
			curl[i][j] = c > 0 ? c : -c;
		}
	}

	for (int i = 1; i <= N ; i++)
	{
		for (int j = 1; j <= N ; j++)
		{
			dw_dx = (curl[i+1][j] - curl[i-1][j]) * 0.5f;
			dw_dy = (curl[i][j + 1] - curl[i][i - 1]) * 0.5f;

			length = 1.0f / ((float)sqrt(dw_dx*dw_dx + dw_dy*dw_dy) + 0.00000001f);

			dw_dx *= length;
			dw_dy *= length;

			float v = curl[i][j];	
			velocityGrid[i][j].u += System::vortForce*dw_dy * -v;
			velocityGrid[i][j].v += System::vortForce*dw_dx * v;
		}
	}

	//deleting curl pointers 
	for (int i = 0; i < N + 2; i++)
	{
		delete(curl[i]);
	}
	delete(curl);
}