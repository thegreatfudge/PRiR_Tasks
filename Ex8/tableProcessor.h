/*
 * tableProcessor.h
 */

#ifndef TABLEPROCESSOR_H_
#define TABLEPROCESSOR_H_

class tableProcessor {
private:
	int **table;
	int **table2;
	int tmpTable[25];
	int size;
	int repetitions;

	void toSmallTable( int i, int j );

public:
	tableProcessor();

	// przekazuje kwadratowa tablice do obrobki
	void setTable( int **table, int size );

	// ustawia liczbe powtorek obliczen
	void setRepetitions( int repetitions );

	// prowadzi obliczenia
	void calc();

	// zwraca zawartosc tablicy table[i1][i2]
	int get( int i1, int i2 );

	virtual ~tableProcessor();
};

#endif /* TABLEPROCESSOR_H_ */
