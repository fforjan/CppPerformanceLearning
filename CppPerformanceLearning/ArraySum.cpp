#include <vector>
#include "../GoogleBenchmark/benchmark/include/benchmark/benchmark.h"
#pragma comment(lib, "Shlwapi.lib")

const int VAPOR = 0;
const int LIQUID = 1;
const int TOTAL = 2;
const int NOC = 100;
struct Equ_t
{
	double X[TOTAL + 1][NOC];
	double VF;
};

static void Array(benchmark::State& state) {
	double sumX, sumY, sumZXY;
	Equ_t equObj;
	equObj.VF = 1;
	Equ_t* equ = &equObj;

	// Code before the loop is not measured
	for (auto _ : state) {
		for (long i = 0; i < NOC; ++i)
		{
			sumY += equ->X[VAPOR][i];
			sumX += equ->X[LIQUID][i];
			sumZXY += -equ->X[TOTAL][i]
				+ (equ->VF * equ->X[VAPOR][i]
					+ (1.0 - equ->VF)*equ->X[LIQUID][i]);
		}
		benchmark::DoNotOptimize(sumY);
		benchmark::DoNotOptimize(sumX);
		benchmark::DoNotOptimize(sumZXY);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(Array);
