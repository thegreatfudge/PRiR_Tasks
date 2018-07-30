/*
 * Distribution.h
 *
 */

#include"DistributionBase.h"

#ifndef DISTRIBUTION_H_
#define DISTRIBUTION_H_

class Distribution {
private:
	static DistributionBase *base;
public:
	// prawdopowobienstwo wylosowania kuli o numerze v gdy jest ona losowana jako n-ta z kolei
	static double getProbability( int n, int v );
	static void setBase( DistributionBase *base );
};


#endif /* DISTRIBUTION_H_ */
