#ifndef MY_CONSTS_H_
#define MY_CONSTS_H_

const int REPETITIONS = 10;
const long CHUNK_SIZE = 1000000; 
const int BALLS = 20;
const int BALLS_TO_DRAW = 13;
const long SLOW_DOWN = 60;
const long SLOWEST_PROCESS_SLOW_DOWN = 150;
const long SLOWEST_PROCESS_RANK = -1;

// o tyle razy wiecej eksperymentow jest uzywanych do wyznaczenia ksztaltu histogramu
const long HISTOGRAM_ESTIMATION = 40;
// minimalna liczba zliczen, ktora moze byc uzyta do testu histogramu
const long HISTOGRAM_COUNTS_LIMIT = 2500;
// oczekiwane przyspieszenie 
const double EXPECTED_EFFICIENCY = 0.85;
// blad wyznaczania histogramu
const double HISTOGRAM_ERROR = 0.075; // 7.5%

#endif /* MY_CONSTS_H_ */
