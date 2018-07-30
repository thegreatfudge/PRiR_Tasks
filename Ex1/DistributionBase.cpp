#include <math.h>
#include <mpi.h>
#include <iostream>
#include "DistributionBase.h"

using namespace std;

DistributionBase::DistributionBase( double *probability, int size ) {
	this->probability = probability;
	this->size = size;
	this->slowDown = 0;
	this->localCalls = 0;
	totalCalls = 0;
	slowDownHelper = 0.0;
	calls = NULL;
}

void DistributionBase::myusleep(const long slowDown) {
	for (long i = 0; i < slowDown; i++)
		slowDownHelper += exp(sin(i * 0.0001));
}

void DistributionBase::setSlowDown( long slowDown ) {
	this->slowDown = slowDown;
}

double DistributionBase::getProbability( int n, int v ) {
	localCalls++;
	myusleep( slowDown );
	if ( v < size ) {
		return probability[ n ];
	} else {
		return 0.0;
	}
}

void DistributionBase::shareTotalCalls() {
	int rank;

	MPI_Comm_rank( MPI_COMM_WORLD, &rank);

	if ( rank ) {
		MPI_Send( &localCalls, 1, MPI_LONG, 0, 1, MPI_COMM_WORLD );
	} else {
	    int processes;
		MPI_Status status;
		MPI_Comm_size( MPI_COMM_WORLD, &processes);
		long result;
		calls = new long[ processes ];
		totalCalls = localCalls;
		cout << " W procesie " << 0 << " wykonano " << localCalls << " obliczen" << endl;
		for ( int i = 1; i < processes; i++ ) {
			MPI_Recv( &result, 1, MPI_LONG, i, 1, MPI_COMM_WORLD, &status );
			cout << " W procesie " << i << " wykonano " << result << " obliczen" << endl;
			totalCalls += result;
                        calls[ i ] = result;
		}
		cout << " Wszystkie wywolania getProbability to " << totalCalls << endl;
	}
}

long DistributionBase::getTotalCalls() {
	if ( totalCalls == 0 ) return localCalls;
	return totalCalls;
}

long DistributionBase::getCalls( int process ) {
	if ( calls == NULL ) {
		return 0;
	} else {
		return calls[ process ];
	}
}

void DistributionBase::clearCalls() {
	localCalls = 0;
}
