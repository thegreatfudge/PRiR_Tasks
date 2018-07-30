/*
 * tableProcessor.cpp
 *
 */

#include "tableProcessorS.h"
#include "Helper.h"
#include <iostream>

using namespace std;

tableProcessorS::tableProcessorS() {
	size = 0;
}

tableProcessorS::~tableProcessorS() {
}

void tableProcessorS::setTable(int **table, int size) {
	this->table = table;
	this->size = size;

	table2 = new int*[size];

	for (int i = 0; i < size; i++)
		table2[i] = new int[size];
}

void tableProcessorS::setRepetitions(int repetitions) {
	this->repetitions = repetitions;
}

void tableProcessorS::toSmallTable(int i, int j) {
	int index = 0;
	for (int ii = -2; ii < 3; ii++)
		for (int jj = -2; jj < 3; jj++)
			tmpTable[index++] = table[i + ii][j + jj];
}

void tableProcessorS::calc() {

	for (int i = 0; i < repetitions; i++) {
//		cout << "Repetition #" << i << endl;
		for (int j = 2; j < size - 2; j++)
			for (int k = 2; k < size - 2; k++) {
				toSmallTable(j, k);
				table2[j][k] = Helper::calcSth25(tmpTable);
			}
		// tablica2 -> tablica
		for (int j = 2; j < size - 2; j++)
			for (int k = 2; k < size - 2; k++)
				table[j][k] = table2[j][k];
	}
}

int tableProcessorS::get( int i, int j ) {
	return table[ i ][ j ];
}
