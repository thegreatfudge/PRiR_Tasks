#include "Helper.h"
#include "math.h"

// metoda, ktora cos robi z 25-cioma elementami wektora
// moze ona ulec zmianie, ale ta jest wystarczajaco dobra
// do testowania kodu.
int Helper::calcSth25( int *vector ) {
	return Helper::calcSthN( vector, 25 );
}

int Helper::calcSthN( int *vector, int size ) {
	int max = vector[0];
	int min = max;

	for ( int i = 1; i < size; i++ ) {
		if ( vector[ i ] > max ) max = vector[ i ];
		if ( vector[ i ] < min ) min = vector[ i ];
	}

	return ( max + min ) / 2;
}
