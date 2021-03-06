/*
 * Experiment.h
 *
 */

#ifndef EXPERIMENT_H_
#define EXPERIMENT_H_

#include "MyMPI.h"

class Experiment {
private:
	long *histogram; // histogram
	long *histogramLocal;
	bool *used; // wektor pomocniczy przy losowaniu bez zwracania
	bool *forbidden; // tych kulek nie mozna uzywac
	int drawsNumber; // liczba kulek do wylosowania
	int balls; // liczba kulek
	long hmax; // maksymalny indeks histogramu
	bool withReplacement; // losowanie ze zwracaniem
	long experiments;
	double *timeTable; //tablica czasów dla threadów
	int *numberOfTestsForEachProcess;

	long singleExperimentResult();
	void allBallsToUrn(); // kasowanie pamieci o wybranych z urny kulach
	bool isAllowed( int ball ); // sprawdza czy kuli mozna uzywac i czy jest w urnie
	void ballIsDrawn( int ball ); // informuje, ze kula zostala wyciagnieta z urny

	MyMPI *myMPI;
public:
	Experiment(int balls, int drawsNumber);
	void setMyMPIModule(MyMPI *mpi);
	virtual ~Experiment();

	double getTime(int threadNumber, int numberOfTests);

	void setSamplingWithReplacement(); // losowanie ze zwracaniem
	void setSamplingWithoutReplacement(); // losowanie bez zwracania
	void setNumberOfExperiments( long experiments ); // liczba pojedynczych eksperymentow

	void setForbidden( int ball ); // wskazanej kuli nie wolno uzywac w losowaniu
	void setAllowed( int ball ); // wskazanej kuli mozna uzywac w losowaniu

	void calc();
	void clearHistogram(); // czysci histogram
	long *getHistogram();
	long getHistogramSize();
};

#endif /* EXPERIMENT_H_ */
