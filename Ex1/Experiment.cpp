/*
 * Experiment.cpp
 *
 */

#include<stdlib.h>
#include<math.h>
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
	histogramLocal = new long[hmax];
	timeTable = new double[20];
	numberOfTestsForEachProcess = new int[20];
	used = new bool[balls];
	forbidden = new bool[ balls ];
	for ( int i = 0; i < balls; i++ )
		forbidden[ i ] = false;
}

double Experiment::getTime(int threadNumber, int numberOfTests){
	int currentProcess;
	myMPI->MPI_Comm_rank( MPI_COMM_WORLD, &currentProcess);
	double beginningTime = 0, endTime = 0;
	if(threadNumber == currentProcess){
		beginningTime = MPI_Wtime();
		for(int i = 0; i < numberOfTests; ++i)
			histogramLocal[singleExperimentResult()]++;
		endTime = MPI_Wtime();
	}
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
		histogramLocal[i] = 0;
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

	//1. Get time table with times needed of each process to do 10 exsperiments
	//for(int i=0; i<numberOfProcesses; i++){
	double timeResult = getTime(currentProcess, 10);
	MPI_Send(&timeResult, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

	if(currentProcess == 0){
		for(int i=0; i<numberOfProcesses; i++)
			MPI_Recv(&timeTable[i], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//done

		//2. Count how many tests should each process take
		//2a. Sum all the times
		//cout << "timeTable: " << timeTable[0] << " ! " << timeTable[1];
		double sumOfAllTimes = 0;
		for(int i=0; i<numberOfProcesses; ++i)
			sumOfAllTimes += timeTable[i];
		//done
		//2b. Get the sum percentage for each proces
		int percentageForEachProcessTime[numberOfProcesses];
		int numberOfTestsLeft = experiments - numberOfProcesses * 10;
		for(int i=0; i<numberOfProcesses; ++i){
			percentageForEachProcessTime[i] = (timeTable[i]/sumOfAllTimes) * 100;
			if(percentageForEachProcessTime[i] < 35){
				numberOfTestsForEachProcess[i] = (numberOfTestsLeft - ((numberOfProcesses - i) * 100))/(numberOfProcesses - i) * 1.3;
				numberOfTestsLeft -= numberOfTestsForEachProcess[i];
			}
			else if(percentageForEachProcessTime[i] < 70){
				numberOfTestsForEachProcess[i] = (numberOfTestsLeft - ((numberOfProcesses - i) * 100))/(numberOfProcesses - i) * 1.2;
				numberOfTestsLeft -= numberOfTestsForEachProcess[i];
			}
			else{
				numberOfTestsForEachProcess[i] = (numberOfTestsLeft - ((numberOfProcesses - i) * 100))/(numberOfProcesses - i);
				numberOfTestsLeft -= numberOfTestsForEachProcess[i];
			}

			if(numberOfTestsLeft < 0)
				numberOfTestsForEachProcess[i] += numberOfTestsLeft;

			// cout << "Proces " << i << " BEDZIE LICZYL " << numberOfTestsForEachProcess[i] << " TESTOW" << endl;
		}
		//done
	}
	MPI_Bcast(numberOfTestsForEachProcess, numberOfProcesses, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(used, balls, MPI_C_BOOL, 0, MPI_COMM_WORLD);
	MPI_Bcast(forbidden, balls, MPI_C_BOOL, 0, MPI_COMM_WORLD);

	for(int i = 0; i < numberOfTestsForEachProcess[currentProcess]; ++i)
		histogramLocal[singleExperimentResult()]++;

	MPI_Reduce(histogramLocal, histogram, hmax, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
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