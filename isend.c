#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double integrate(double (*func)(double), double begin, double end, int num_points) {
    double dx, calka;
	int i;

	dx = (end - begin) / (double)num_points;

	calka = 0;
	for (i=1; i<=num_points; i++) {
		calka += func(begin + i * dx);
	}
        calka *= dx;

        return calka;
}

double funkc(double x)
{
    return x*x*x+15;
}

double* tablicaWyjscia(double begin, double end, int num_points, int procNum)
{
    double h = (end - begin) / procNum;
    double *wektor = malloc(sizeof(double) * 3 * procNum);
    int punkty= floor(num_points / procNum);
    int restPoints = num_points - (punkty* procNum);

int i;
    for (i = 0; i < procNum; i++)
    {
        wektor[i*3] = (begin + (i*h));
        wektor[(i*3)+1] = (begin + ((i+1)*h));
        if (restPoints > 0)
        {
            wektor[(i*3)+2] = (float)punkty+ 1;
            restPoints--;
        }
        else
        {
            wektor[(i*3)+2] = (float)punkty;
        }
      }

    return wektor;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        return 1;
    double begin = atof(argv[argc-3]);
    double end = atof(argv[argc-2]);
    int num_points = atoi(argv[argc-1]);
    double *tablica = NULL;
    double *tablica_wyjsciowa = NULL;
	MPI_Request request;

    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0)
    {
        tablica = tablicaWyjscia(begin, end, num_points, world_size);
        tablica_wyjsciowa = malloc(sizeof(double) * world_size);
    }

    double* tablica_wejsica = malloc(sizeof(double) * 3);

   	if (world_rank == 0)
	{
int i;
		for (i = 0; i < world_size; i++)
		{
			MPI_Isend(tablica + (i*3), 3, MPI_DOUBLE, i,
			0, MPI_COMM_WORLD, &request);
		}
	}
	
	MPI_Irecv(tablica_wejsica, 3, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
             &request);
	MPI_Wait(&request, MPI_STATUS_IGNORE);

       double wynik = integrate(funkc, tablica_wejsica[0], tablica_wejsica[1], (int)tablica_wejsica[2]);
    printf("Wiadomosc od procesu nr. %d, calka czastkowa =  %f\n", world_rank, wynik);

	MPI_Isend(&wynik, 1, MPI_DOUBLE, 0,
				0, MPI_COMM_WORLD, &request);
				
	if (world_rank == 0)
	{
int i;
		for (i = 0; i < world_size; i++)
		{
			MPI_Irecv(tablica_wyjsciowa + i, 1, MPI_DOUBLE, i,
					0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, MPI_STATUS_IGNORE);
		}
	}
	
    if (world_rank == 0)
    {
        double wynikCalkowity = 0;
int i;        
for (i = 0; i < world_size; i ++)
        {
            wynikCalkowity += tablica_wyjsciowa[i];
        }
        printf ("\nFunkcja - Isend, Irecv : \n"); 
        printf("Wynik calkowania funckji to: %f\n", wynikCalkowity);
    }
	
	MPI_Finalize();
    return 0;
}
