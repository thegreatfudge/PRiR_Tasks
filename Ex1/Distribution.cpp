#include "Distribution.h"
#include "DistributionBase.h"

DistributionBase *Distribution::base;

double Distribution::getProbability( int n, int v ) {
	return base->getProbability( n, v );
}

void Distribution::setBase( DistributionBase *b ) {
	base = b;
} 
