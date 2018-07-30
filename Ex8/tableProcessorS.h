/*
 * tableProcessorS.h
 *
 */

#ifndef TABLEPROCESSORS_H_
#define TABLEPROCESSORS_H_

class tableProcessorS {
private:
	int **table;
	int **table2;
	int tmpTable[25];
	int size;
	int repetitions;

	void toSmallTable( int i, int j );

public:
	tableProcessorS();

	// przekazuje kwadratowa tablice do obrobki
	void setTable( int **table, int size );

	// ustawia liczbe powtorek obliczen
	void setRepetitions( int repetitions );

	// prowadzi obliczenia
	void calc();

	// zwraca zawartosc tablicy table[i1][i2]
	int get( int i1, int i2 );

	virtual ~tableProcessorS();
};

#endif /* TABLEPROCESSOR_H_ */
