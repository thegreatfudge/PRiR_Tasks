/*
 * ExperimentS.cpp
 *
 */

#include<stdlib.h>
#include<iostream>

#include "ExperimentS.h"
#include "Distribution.h"

#define DEBUG_ON_

using namespace std;

ExperimentS::ExperimentS(int balls, int drawsNumber) {
	this->balls = balls;
	this->drawsNumber = drawsNumber;

// najgorszy scenariusz - losowanie ze zwracaniem i 
// wylosowano tylko kule o najwyzszym numerze.
	hmax = this->drawsNumber * (balls-1);

	histogram = new long[hmax];
	used = new bool[balls];
	forbidden = new bool[ balls ];
	for ( int i = 0; i < balls; i++ )
		forbidden[ i ] = false;
}

void ExperimentS::setSamplingWithReplacement() {
	withReplacement = true;
}

void ExperimentS::setSamplingWithoutReplacement() {
	withReplacement = false;
}

void ExperimentS::setForbidden( int ball ) {
	#ifdef DEBUG_ON
		cout << "Zablokowano mozliwosc uzywania kuli " << ball << endl;
	#endif
	forbidden[ ball ] = true;
}

void ExperimentS::setAllowed( int ball ) {
	forbidden[ ball ] = false;
}

void ExperimentS::clearHistogram() {
	for (long i = 0; i < hmax; i++) {
		histogram[i] = 0;
	}
}

void ExperimentS::allBallsToUrn() {
	if ( withReplacement ) return;
	for (int i = 0; i < balls; i++)
		used[i] = false;
}

void ExperimentS::ballIsDrawn( int ball ) {
	if ( withReplacement ) return;

	#ifdef DEBUG_ON
			cout << "Kula o numerze " << ball << " nie moze juz byc ponownie wybrana" << endl;
	#endif

	used[ball] = true;
}

bool ExperimentS::isAllowed( int ball ) {
	if ( forbidden[ ball ] ) return false; // tej kuli nie mozna uzywac

	if ( withReplacement ) return true; // kule sa zwracane, wiec mozna ich ponownie uzywac

	return ! used[ ball ];
}

void ExperimentS::setMyMPIModule(MyMPI *mpi) {
	this->myMPI = mpi;
}

long ExperimentS::singleExperimentResult() {
	long sum = 0;
	int ball;
	double p;

	allBallsToUrn();
	for (int i = 0; i < drawsNumber;) {
		ball = (int) (((double) balls * rand()) / ( RAND_MAX + 1.0)); // rand losuje od 0 do RAND_MAX wlacznie

#ifdef DEBUG_ON
		cout << "Propozycja " << ball << endl;
#endif

		if ( ! isAllowed( ball ) ) {
#ifdef DEBUG_ON
		    cout << "Propozycja - ta kula nie moze byc uzyta " << ball << endl;
#endif
			continue; // jeszcze raz losujemy 
		} else {
#ifdef DEBUG_ON
		    cout << "Propozycja - OK " << ball << endl;			
#endif
		}

		p = Distribution::getProbability(i + 1, ball); // pobieramy prawdopodobienstwo 
		// wybrania tej kuli

		if ((rand() / ( RAND_MAX + 1.0)) < p) // akceptacja wyboru kuli z zadanym prawdopodobienstwem
		{
			ballIsDrawn(ball);
			sum += ball; 
			i++;
		}
	}

	return sum;
}

void ExperimentS::setNumberOfExperiments( long experiments ) {
	this->experiments = experiments;
}

void ExperimentS::calc() {
	for (long l = 0; l < experiments; l++) {
		histogram[singleExperimentResult()]++;
	}
}

long ExperimentS::getHistogramSize() {
	return hmax;
}

long *ExperimentS::getHistogram() {
	return histogram;
}

ExperimentS::~ExperimentS() {
	delete[] histogram;
	delete[] used;
	delete[] forbidden;
}
