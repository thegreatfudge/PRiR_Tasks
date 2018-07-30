#include"tableProcessor.h"
#include"tableProcessorS.h"


#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

#include <math.h>
#include <mpi.h>

using namespace std;

const int SIZE = 1303;
const int REPETITIONS = 229;

int **table;
int **tableS;
double serialTimeExecution;

void initTable() {

	srandom(time( NULL));

	table = new int*[SIZE];
	tableS = new int*[SIZE];

	for (int i = 0; i < SIZE; i++) {
		table[i] = new int[SIZE];
		tableS[ i ] = new int [ SIZE ];
		for (int j = 0; j < SIZE; j++) {
			table[i][j] = 10 + ((20000 * random()) / RAND_MAX);
			tableS[i][ j ] = table[ i ][ j ];
		}
	}

	// mamy 2 identycznie zainicjowane tabele
}

void calcSerial() {
	struct timeval tf;
	double t0;
	gettimeofday(&tf, NULL);
	t0 = tf.tv_sec + tf.tv_usec * 0.000001;

	tableProcessorS *tp = new tableProcessorS();
	tp->setTable(tableS, SIZE);
	tp->setRepetitions(REPETITIONS);

	tp->calc();

	gettimeofday(&tf, NULL);
	serialTimeExecution = tf.tv_sec + tf.tv_usec * 0.000001 - t0;
}

int calcMPI() {

	int rank;
	int size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	tableProcessor *tp = new tableProcessor();
	struct timeval tf;
	double t0;

	// tylko proces o rank 0 otrzymuje dane
	if (!rank) {
		cout << "MPI - Parallel" << endl;
		gettimeofday(&tf, NULL);
		t0 = tf.tv_sec + tf.tv_usec * 0.000001;

		tp->setTable(table, SIZE);
		tp->setRepetitions(REPETITIONS);
	}

	cout << "Tu proces " << rank << " czekam - I-sza bariera" << endl;
	MPI_Barrier (MPI_COMM_WORLD); // bariera - czekamy na rozeslanie danych

	cout << "Tu proces " << rank << " uruchamiam calc" << endl;
	tp->calc(); // licza wszystkie procesy

	cout << "Tu proces " << rank << " czekam - II-ga bariera" << endl;
	MPI_Barrier(MPI_COMM_WORLD); // bariera - czekamy na zakonczenie obliczen

	if (!rank) { // tylko rank = 0 jest odpytywany o wynik obliczen!
		gettimeofday(&tf, NULL);

		double tk = tf.tv_sec + tf.tv_usec * 0.000001;

		cout << "Zakonczono obliczenia MPI" << endl;

		cout << "---- --- -- - TEST - -- --- ----" << endl;

		int showMAXErrors = 5;
		for ( int i = 0; i < SIZE; i++ )
			for ( int j = 0; j < SIZE; j++ ) {
				if ( tableS[ i ][ j ]  != tp->get( i, j ) ) {
					if ( showMAXErrors > 0 ) {
						cout << "BLAD [" << i << ", " << j << " ] powinno byc " << tableS[ i ][ j ] << " jest "
								<< tp->get( i, j ) << endl;
						showMAXErrors --;

					}
				}
			}

		if ( showMAXErrors == 5 ) {
			cout << "Wersja MPI i sekwencyjna uzyskaly takie same wyniki obliczen" << endl;
		} else {
			cout << "BLAD: Wersja MPI i sekwencyjna zakonczyly prace z innym wynikiem" << endl;
			int maks = 0;
			int i_max;
			int j_max;
			for ( int i = 0; i < SIZE; i++ )
			 for ( int j = 0; j < SIZE; j++ ) 
			   if ( abs( tableS[ i ][ j ] - tp->get( i, j ) ) > maks ) {
			     maks = abs( tableS[ i ][ j ] - tp->get( i, j ) );
			     i_max = i;
			     j_max = j;			   
			   } 
			     
			cout << "Maksymalne roznica wynikow: " << maks << " dla [ " << i_max << ", " << j_max <<
			     "] powinno byc " << tableS[ i_max][ j_max ]
			     << " a jest " << tp->get( i_max, j_max ) << endl;
		}

		cout << "Przyspieszenie : " << serialTimeExecution / ( tk - t0 ) << "x" << endl;
		cout << "Efektywnosc    : " << 100.0 * serialTimeExecution / ( size * ( tk - t0 ) ) << "%" << endl;

	}

	return size;

}

int main(int ac, char **av) {

	int rank;

	MPI_Init(&ac, &av);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if ( ! rank ) {
		cout << "Serial liczy proces #" << rank  << endl;
		initTable();
		calcSerial();
	}

	calcMPI();
	MPI_Finalize();

	return 0;
}

