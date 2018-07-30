#include"Experiment.h"
#include"ExperimentS.h"
#include "Distribution.h"
#include "DistributionBase.h"
#include "MyMPI.h"
#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include "myconsts.h"

using namespace std;

bool *canBeUsed; // dana pozycja histogramu moze byc uzyta do testu
long *histogramError; // blad wyznaczenia danej pozycji histogramu
long *histogramShape; // oczekiwany rozklad zliczen w histogramie
double serialComputationTime;
double parallelComputationTime;

void prepareHistogramShape( long * histogram, long histogramSize ) {

    long sum = 0L;

	canBeUsed = new bool[ histogramSize ];
	histogramError = new long[ histogramSize ];
	histogramShape = new long[ histogramSize ];

    for ( long i = 0; i < histogramSize; i++ ) {
	  sum += histogram[ i ];
	  histogramShape[ i ] = histogram[ i ];
	  histogramError[ i ] = (long)( histogram[ i ] * HISTOGRAM_ERROR );
	  if ( histogram[ i ] < ( HISTOGRAM_ESTIMATION * HISTOGRAM_COUNTS_LIMIT ) ) {
		  canBeUsed[ i ] = false;
	  } else {
		  canBeUsed[ i ] = true;
	  	  cout << " h[ " << setw(4) << i << " ] = " << setw(8) << histogram[ i ] << " err = " 
	               << setw(6) << histogramError[ i ] << " usable " << canBeUsed[ i ] << endl;
	  }
	}

	cout << "Ksztalt histogramu na podstawie " << sum << " eksperymentow " << endl;
}

void calcHistogramSerial( ExperimentS *ex ) {
	for ( int i = 0; i < BALLS; i+=2 ) 
		ex->setForbidden( i ); 

	ex->clearHistogram();
	for ( int i = 0; i < REPETITIONS; i++ ) {
                if ( i < 10 ) {
	  	  ex->setForbidden( i );
                }
		ex->calc(); 
	}
}

void calcHistogramParallel( Experiment *ex, int process ) {
	for ( int i = 0; i < REPETITIONS; i++ ) {
		if ( process == 0 ) {
                   if ( i < 10 ) {
			ex->setForbidden( i );
                   }
		}
		ex->calc(); // wszystkie procesy licza
	}
}


void test( long *histogram, long histogramSize, double computationTime, bool speedUpTestON, 
           long totalGetProbabilityCalls, bool distributionUsageTestON, int processes,
		   bool showReport ) {
	// test 1 - suma elementow w histogramie musi odpowiadac liczbie zleconych losowan

	bool testOK = true;
	cout << "------------ TEST 1 --------------" << endl;
	cout << "Test ilosci zliczen w histogramie " << flush;
	long sum = 0;
	for ( long i = 0; i < histogramSize; i++ ) {
		sum += histogram[ i ];
	}
	if ( sum == REPETITIONS * CHUNK_SIZE ) {
		cout << "OK" << endl;
	} else {
		cout << "BLAD, jest " << sum << " powinno byc " << REPETITIONS * CHUNK_SIZE << endl;
		testOK = false;		
	}

	cout << "------------ TEST 2 --------------" << endl;

	bool shapeOK = true;
	for ( long i = 0; i < histogramSize; i++ ) {
		if ( canBeUsed[ i ] ) {
			if ( labs( histogram[ i ] * HISTOGRAM_ESTIMATION - histogramShape[ i ] ) > histogramError[i] ) {
				cout << "BLAD: na pozycji " << i << " jest " << histogram[ i ] * HISTOGRAM_ESTIMATION << " oczekiwano " << histogramShape[ i ] << endl;
				shapeOK = false;
			} 
		}
	}

	cout << "Test ksztaltu histogramu " << flush;
	if ( shapeOK ) {
		cout << "OK" << endl;
	} else {
		cout << "BLAD " << endl;
		testOK = false;		
	}

	cout << "------------ TEST 3 --------------" << endl;

	double speedUp = serialComputationTime / computationTime;
	double expected = processes * EXPECTED_EFFICIENCY;
	cout << "Test przyspieszenia obliczen, uzyskano : " << speedUp << "x " << flush;
	if ( speedUpTestON ) {
		if ( speedUp >= expected ) {
			cout << "OK" << endl;
		} else {
			cout << "BLAD oczekiwano co najmniej " << expected << endl;
			testOK = false;		
		}
	} else {
		cout << endl;
	}

	cout << "------------ TEST 4 --------------" << endl;
// test zgodnosci wywolan probability i zliczen w histogramie

    if ( distributionUsageTestON ) {
		cout << "Test zgodnosci ilosci wykonan getProbability i zliczen w histogramie " << flush;
		long diff = labs( totalGetProbabilityCalls - sum * BALLS_TO_DRAW );
		if ( diff != 0 ) {
			cout << endl;
			long maxDiff = 20 * processes * BALLS_TO_DRAW * REPETITIONS;
			if ( diff <= maxDiff ) {
				cout << "UWAGA: program niepotrzebnie tworzy dodatkowa prace do wykonania : "
				     << endl;
			    cout << "UWAGA: wykonano dodatkowo (i niepotrzebnie) " << diff << " eksperymentow" 
				     << endl;
			} else {
				cout << "  BLAD: totalGetProbabilityCalls = " << 
				totalGetProbabilityCalls << " suma zliczen w histogramie = " << sum * BALLS_TO_DRAW<< endl;
				cout << "  BLAD: roznica pomiedzy wykonana a zadana to " << diff << 
				     endl << "  BLAD: 20xL_LOSOWANxL_PROCESOWxREPETITIONS = " << maxDiff << endl;
				testOK = false; 
			}
		} else {
			cout << "OK" << endl;
		}
	}

	if ( showReport ) {
		if ( testOK ) {
			cout << "OK: OK: OK: OK:   TESTY ZOSTALY ZALICZONE" << endl;
		} else {
			cout << "BLAD: BLAD: BLAD: NIE ZALICZONO TESTOW" << endl;
		}
	}
}

int main(int argc, char **argv) {

    int process, processes;
	MyMPI *myMPI = new MyMPI();
	myMPI->MPI_Init(&argc, &argv);
	myMPI->MPI_Comm_rank( MPI_COMM_WORLD, &process);
	myMPI->MPI_Comm_size( MPI_COMM_WORLD, &processes);

	double *probability = new double[ BALLS ];
	
	for ( int i = 0; i < BALLS; i++ )
	   probability[ i ] = 2.0; 

	DistributionBase *db = new DistributionBase( probability, BALLS );
	Distribution::setBase( db );

// na poczatek generowanie rozkladu - bez opóźnienia
 
    if ( process == 0 ) {
   		ExperimentS *ex = new ExperimentS(BALLS, BALLS_TO_DRAW); 
  		ex->setNumberOfExperiments( CHUNK_SIZE * HISTOGRAM_ESTIMATION );
		ex->setSamplingWithReplacement();

		double serialTimeStart0 = MPI_Wtime();
		calcHistogramSerial(ex);
		double serialTimeStop0 = MPI_Wtime();
		cout << "Czas szacowania histogramu: " << serialTimeStop0 - serialTimeStart0 << endl;
		prepareHistogramShape(ex->getHistogram(), ex->getHistogramSize());

		ex = new ExperimentS(BALLS, BALLS_TO_DRAW); 
  		ex->setNumberOfExperiments( CHUNK_SIZE );
		ex->setSamplingWithReplacement();
		db->clearCalls();
		db->setSlowDown( SLOW_DOWN );
		// start pomiaru czasu
		double serialTimeStart = MPI_Wtime();
		calcHistogramSerial(ex);
		double serialTimeStop = MPI_Wtime();
		// stop pomiaru czasu
		serialComputationTime = serialTimeStop - serialTimeStart;
		cout << "Czas wyznaczania histogramu sekwencyjnie: " << serialComputationTime << endl;
		double tenExpTime = 10.0 * serialComputationTime / ( CHUNK_SIZE * REPETITIONS );
		cout << "Czas wykonania 10 eksperymentow " << tenExpTime << endl;
		if ( tenExpTime < MPI_Wtick() * 20 ) {
			cout << "BLAD: spowolnienie obliczen jest zbyt male !!!!!!!!!!" << endl;
		}

		test(ex->getHistogram(), ex->getHistogramSize(),
			serialComputationTime, false, 
			db->getTotalCalls(), true, processes, false );
	}

	MPI_Barrier( MPI_COMM_WORLD );

	db->clearCalls();

	if ( process == SLOWEST_PROCESS_RANK ) {
		db->setSlowDown( SLOWEST_PROCESS_SLOW_DOWN );
	} else {
		db->setSlowDown( SLOW_DOWN );
	}
 
	Experiment *exParallel = new Experiment(BALLS, BALLS_TO_DRAW);

	exParallel->setNumberOfExperiments( CHUNK_SIZE );
	exParallel->setSamplingWithReplacement();
	exParallel->setMyMPIModule( myMPI ); // kazdy obiekt otrzymuje dostep do MPI
	exParallel->clearHistogram();
        srand( 123 + 7 * process ); // inicjalizacja generatora liczb losowych

	if ( process == 0 ) {
		cout << "Start parallel test " << endl;

		for ( int i = 0; i < BALLS; i+=2 ) 
			exParallel->setForbidden( i ); 

		// start pomiaru czasu
		double parallelTimeStart = MPI_Wtime();
		calcHistogramParallel(exParallel, process);
		double parallelTimeStop = MPI_Wtime();
		// stop pomiaru czasu
		parallelComputationTime = parallelTimeStop - parallelTimeStart;
	} else {
		calcHistogramParallel(exParallel, process );		
	}

	db->shareTotalCalls();

	if ( process == 0 ) {
		cout << "---------------------------------------------------" << endl;
		cout << "Czas wyznaczania histogramu sekwencyjnie: " << serialComputationTime << endl;
		cout << "Czas wyznaczania histogramu rownolegle..: " << parallelComputationTime << endl;
		cout << "---------------------------------------------------" << endl;
		test(exParallel->getHistogram(), exParallel->getHistogramSize(),
			parallelComputationTime, true, 
			db->getTotalCalls(), true, processes, true );

		// sprawdzamy podzial pracy pomiedzy procesy
		if ( SLOWEST_PROCESS_RANK >= 0 && SLOWEST_PROCESS_RANK < processes ) {
			long callsInSlowestProcess = db->getCalls( SLOWEST_PROCESS_RANK );
			// o ile powinno byc mniej?
			long callsPerProcess = db->getTotalCalls() / processes;
			double timesSlower = SLOWEST_PROCESS_SLOW_DOWN / SLOW_DOWN;
			cout << "TEST uwzglednienia powolnego procesu" << endl;
			cout << "Wykonano w nim " << callsInSlowestProcess << " oblieczen" << endl;
			// praca nie powinna byc znacznie mniejsza od zwyklego podzialu z uwzglednieniem
			// współczynnika spowolnienia
			long lowerLimit = (long)( callsPerProcess / timesSlower ) - 20;
			// praca nie powinna byc porownywalna z innymi procesami
			long upperLimit = callsPerProcess - processes; 
			cout << "Limity od " << lowerLimit << " do " << upperLimit << endl;
			if ( callsInSlowestProcess < lowerLimit ) {
				cout << "UWAGA: Zbyt malo pracy dla powolnego procesu " << endl;
			}
			if ( callsInSlowestProcess > upperLimit ) {
				cout << "UWAGA: Za duzo pracy dla powolnego procesu " << endl;
			}
		}
	}

	MPI_Barrier( MPI_COMM_WORLD );
	myMPI->MPI_Finalize();

	return 0;
}
