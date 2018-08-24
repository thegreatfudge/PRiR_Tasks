/*
 * Experiment.cpp
 *
 */

#include<stdlib.h>
#include<iostream>

#include "Experiment.h"
#include "Distribution.h"

#define DEBUG_ON_

using namespace std;

Experiment::Experiment(int balls, int drawsNumber) {
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

double Experiment::getTime(int threadNumber, int numberOfTests){
	double beginningTime, endTime;
	beginningTime = MPI_Wtime();
	for(int i = 0; i < numberOfTests; ++i)
		histogram[singleExperimentResult()]++;
	endTime = MPI_Wtime();
	return endTime - beginningTime;
}

void Experiment::setSamplingWithReplacement() {
	withReplacement = true;
}

void Experiment::setSamplingWithoutReplacement() {
	withReplacement = false;
}

void Experiment::setForbidden( int ball ) {
	#ifdef DEBUG_ON
		cout << "Zablokowano mozliwosc uzywania kuli " << ball << endl;
	#endif
	forbidden[ ball ] = true;
}

void Experiment::setAllowed( int ball ) {
	forbidden[ ball ] = false;
}

void Experiment::clearHistogram() {
	for (long i = 0; i < hmax; i++) {
		histogram[i] = 0;
	}
}

void Experiment::allBallsToUrn() {
	if ( withReplacement ) return;
	for (int i = 0; i < balls; i++)
		used[i] = false;
}

void Experiment::ballIsDrawn( int ball ) {
	if ( withReplacement ) return;

	#ifdef DEBUG_ON
			cout << "Kula o numerze " << ball << " nie moze juz byc ponownie wybrana" << endl;
	#endif

	used[ball] = true;
}

bool Experiment::isAllowed( int ball ) {
	if ( forbidden[ ball ] ) return false; // tej kuli nie mozna uzywac

	if ( withReplacement ) return true; // kule sa zwracane, wiec mozna ich ponownie uzywac

	return ! used[ ball ];
}

void Experiment::setMyMPIModule(MyMPI *mpi) {
	this->myMPI = mpi;
}

long Experiment::singleExperimentResult() {
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

void Experiment::setNumberOfExperiments( long experiments ) {
	this->experiments = experiments;
}

void Experiment::calc() {

	int numberOfProcesses, currentProcess;
    myMPI->MPI_Comm_size( MPI_COMM_WORLD, &numberOfProcesses);
	myMPI->MPI_Comm_rank( MPI_COMM_WORLD, &currentProcess);
	//get time table with times needed of each process to do 10 exsperiments
	timeTable = new double[numberOfProcesses];

	for(int i=0; i<numberOfProcesses; i++)
		timeTable[i] = getTime(i, 10);

	//done

	if(currentProcess == 0){
		cout << "Tutaj proces " << currentProcess << " i moj czas dla policzenia 10 testow to: " << timeTable[currentProcess] << endl;
	}
	else{
		cout << "Tutaj proces " << currentProcess << " i moj czas dla policzenia 10 testow to: " << timeTable[currentProcess] << endl;

	}
	//cout << "TU EXPERIMENT, WTF SIE DZIJE";
	// for (long l = 0; l < experiments; l++) {
	// 	histogram[singleExperimentResult()]++;
	// }
}

long Experiment::getHistogramSize() {
	return hmax;
}

long *Experiment::getHistogram() {
	return histogram;
}

Experiment::~Experiment() {
	delete[] histogram;
	delete[] used;
	delete[] forbidden;
}