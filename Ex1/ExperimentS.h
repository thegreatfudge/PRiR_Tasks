/*
 * ExperimentS.h
 *
 */

#ifndef EXPERIMENT_S_H_
#define EXPERIMENT_S_H_

#include "MyMPI.h"

class ExperimentS {
private:
	long *histogram; // histogram
	bool *used; // wektor pomocniczy przy losowaniu bez zwracania
	bool *forbidden; // tych kulek nie mozna uzywac 
	int drawsNumber; // liczba kulek do wylosowania
	int balls; // liczba kulek
	long hmax; // maksymalny indeks histogramu
	bool withReplacement; // losowanie ze zwracaniem
	long experiments;

	long singleExperimentResult();
	void allBallsToUrn(); // kasowanie pamieci o wybranych z urny kulach 
	bool isAllowed( int ball ); // sprawdza czy kuli mozna uzywac i czy jest w urnie
	void ballIsDrawn( int ball ); // informuje, ze kula zostala wyciagnieta z urny

	MyMPI *myMPI;
public:
	ExperimentS(int balls, int drawsNumber);
	void setMyMPIModule(MyMPI *mpi);
	virtual ~ExperimentS();

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

#endif /* EXPERIMENT_S_H_ */
