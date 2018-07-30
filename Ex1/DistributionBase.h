/*
 * Distribution.h
 *
 *  Created on: 16 maj 2016
 *      Author: oramus
 */

#ifndef DISTRIBUTION_BASE_H_
#define DISTRIBUTION_BASE_H_

class DistributionBase {
private:
	long localCalls;
	double size;
	double *probability;	
	long slowDown;
	long totalCalls;
	void myusleep(const long slowDown);
	double slowDownHelper;
	long *calls;
public:
	// prawdopowobienstwo wylosowania kuli o numerze v gdy jest ona losowana jako n-ta z kolei
	double getProbability( int n, int v );
	void shareTotalCalls();
	long getTotalCalls();
	long getCalls( int process );
	void clearCalls();
	void setSlowDown( long slowDown );
	
	DistributionBase( double *probability, int size );
};


#endif /* DISTRIBUTION_BASE_H_ */
