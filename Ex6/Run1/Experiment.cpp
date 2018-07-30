/*
 * Experiment.cpp
 *
 *  Created on: 16 maj 2016
 *      Author: oramus
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

	hmax = 0; // wyznaczamy maksymalna sume
	hmin = 0; // i najmniejsza sume
	for (int i = 0; i < drawsNumber; i++) {
		hmax += balls - i;
		hmin += i + 1; // 1 + 2 + 3 + ... liczba losowan
	}

	cout << "Histogram min: " << hmin << " max: " << hmax << endl;

	histogram = new long[hmax + 1];
	used = new bool[balls];

	for (long i = 0; i < hmax + 1; i++)
		histogram[i] = 0;
}

void Experiment::clearUsed() {
	for (int i = 0; i < balls; i++)
		used[i] = false;
}

long Experiment::singleExperimentResult() {
	long sum = 0;
	int ball;
	double p;

	clearUsed();
	for (int i = 0; i < drawsNumber;) {
		ball = 1 + (int) (((double) balls * rand()) / ( RAND_MAX + 1.0)); // rand losuje od 0 do RAND_MAX wlacznie

		if (used[ball - 1])
			continue;

		p = Distribution::getProbability(i + 1, ball); // pobieramy prawdopodobienstwo wylosowania tej kuli

		if ((rand() / ( RAND_MAX + 1.0)) < p) // akceptacja wyboru kuli z zadanym prawdopodobienstwem
				{
#ifdef DEBUG_ON
			cout << "Dodano kule o numerze " << ball << endl;
#endif
			used[ball - 1] = true;
			sum += ball; // kule maja numery od 1 do balls wlacznie
			i++;
		}
	}

///	cout << "Suma = " << sum << endl;

	return sum;
}

Result * Experiment::calc(long experiments) {

	for (long l = 0; l < experiments; l++) {
		histogram[singleExperimentResult()]++;
	}

	long maxID = 0;
	long minID = 0;
	long maxN = 0;
	long minN = experiments;
	double sum = 0.0;
	long values = 0;

	for (long idx = hmin; idx <= hmax; idx++) {
		if (maxN < histogram[idx]) {
			maxN = histogram[idx];
			maxID = idx;
		}
		sum += idx * histogram[idx];
		values += histogram[idx];
	}
// indeks to wartosc, histogram -> liczba wystapien
	return new Result(maxID, maxN, sum / values, values);
}

Experiment::~Experiment() {
	delete[] histogram;
	delete[] used;
}
