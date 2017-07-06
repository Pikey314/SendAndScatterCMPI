#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"


double funkc(double x) {
	return x*x*x+15;
}

double funkc1(double x) {
       return x*x+15;
}


double integrate (double (*func)(double), double begin, double end, int num_points){
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



int main (int argc, char *argv[]){
        int i, liczbaProc, numProc, type = 99, l_przedzialow;
        double poczatek, koniec;
	
	if (argc == 4){
		poczatek = atof (argv[1]);
		koniec = atof (argv[2]);
		l_przedzialow = atoi(argv[3]);
	}
	else 
		exit(-1);
	
	double gap = koniec - poczatek;
	double wynik;
        //double wektorWynikow[l_przedzialow];
	double wynikOst;
	double test;	

	MPI_Status status;
        MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &liczbaProc);
 	MPI_Comm_rank(MPI_COMM_WORLD, &numProc);

	
	if(numProc == 0) {
		for (i=0; i<liczbaProc; i++){
			MPI_Send(&wynik, 1, MPI_DOUBLE, i, type, MPI_COMM_WORLD);
			wynik = integrate(funkc, (poczatek + i*(gap/liczbaProc)) , (poczatek + (i+1)*(gap/liczbaProc)), l_przedzialow/liczbaProc);
	//		wektorWynikow[i] = wynik;	
		}

	} else
		MPI_Recv(&wynik, 1, MPI_DOUBLE, 0, type, MPI_COMM_WORLD, &status);
	//	wektorWynikow[i] = wynik;	
		MPI_Reduce(&wynik, &test, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	printf("Wiadomosc od procesu nr. %d, całka cząstkowa =  %f\n",numProc, wynik);


 	
	//for (i = 0; i<liczbaProc; i++)
	//wynikOst += wektorWynikow[i];
	if (numProc == 0){
	printf ("\nFunkcja - Send, Recv : \n"); 
	printf ("Wynik calkowania funckji to: %f\n", test);
	}
	MPI_Finalize();
	return 0;
}
