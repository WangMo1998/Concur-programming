#include "pch.h"
#include <iostream>
#include <map>
#include <pthread.h>
#include <cmath>
#include <chrono>


using namespace std;
using namespace std::chrono;

static const int threadnum = 256;
pthread_mutex_t barr;

class Data {
public:
	int C;
	map<pair<double, double>, double> * A;
	double step_t = 0.1, step_x = 1.0, A0 = 0.02, T = 1.2, V = 15.0;
	int steps_t = 1000, steps_x = 1000;

	Data() {
		C = 1;
		A = new map<pair<double, double>, double>();
	}
};


Data*  D = new Data();



void * Calc(void * gD) {
	int tnum = *(int*)gD;
	double cur_t;
	double cur_x;
	double step_t = D->step_t;
	double step_x = D->step_x;
	int steps_t = D->steps_t;
	int steps_x = D->steps_x;
	double A0 = D->A0;
	double T = D->T;
	double V = D->V;

	//cout << "Thread " << tnum << endl;
	while(true){
		pthread_mutex_lock(&barr);
		cur_t = step_t * (D->C / steps_t + 1);
		cur_x = step_x * (D->C % steps_x);
		if (D->C > steps_t * steps_x) {
			pthread_mutex_unlock(&barr);
			break;
		}
		D->C++;
		//if(D->C % 1000 == 0) cout << D->C << endl;
		pthread_mutex_unlock(&barr);

		double calc_A;

		calc_A = A0 * cos(((2 * 3.14) / T) * (cur_t - (cur_x / V)));

		//cout << step_t << " " << step_x << endl;
		//if(D->C % ((steps_t * steps_x) / 100) == 0) cout << (D->C / (steps_t * steps_x)) << "%" << endl;

		//pthread_mutex_wait(&barr);
		pthread_mutex_lock(&barr);
		D->A->insert(pair<pair<double, double>, double>(pair<double, double>(cur_t, cur_x), calc_A));
		pthread_mutex_unlock(&barr);
	}
	return nullptr;
}

int main()
{
	/*
	Data*  D = new Data();

	D->T = 1.2; // ѕериод волн?
	D->V = 15.0; // —корость волн?
	D->A0 = 0.02; // јмплитуд?колебани?

	D->step_t = 0.1; // Ўа?времен?
	D->step_x = 1.0; // Ўа?рассто¤ни¤

	D->steps_t = 10;
	D->steps_x = 10;
	*/

	milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	pthread_t Threads[threadnum+1];
	pthread_mutex_init(&barr, nullptr);

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	for (int i = 0; i < threadnum; i++) {
		int ii = i;
		void * tn = (int *)&ii;
		pthread_create(&Threads[i], &attr, Calc, tn);
	}

	
	for (int i = 0; i < threadnum; i++) {
		pthread_join(Threads[i], nullptr);
	}

	milliseconds finish = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	cout << (finish - start).count() << endl;
	cout << D->A->at(pair<double, double>(4.0, 45.0)) << endl;
	return 0;
}
