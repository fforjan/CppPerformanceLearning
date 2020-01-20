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

/**
 * One possible optimization is to avoid using [] but use pointer operations
 */
static void Array_AsPointer(benchmark::State& state) {
	double sumX, sumY, sumZXY;
	Equ_t equObj;
	equObj.VF = 1;
	Equ_t* equ = &equObj;

	// Code before the loop is not measured
	for (auto _ : state) {
		auto xVapor = equ->X[VAPOR];
		auto xLiquid = equ->X[LIQUID];
		auto xTotal = equ->X[TOTAL];

		for (long i = 0; i < NOC; ++i)
		{
			sumY += *xVapor;
			sumX += *xLiquid;
			sumZXY += -*xTotal
				+ (equ->VF * *xVapor
					+ (1.0 - equ->VF) * *xLiquid);

			xVapor++; xLiquid++; xTotal++;
		}
		benchmark::DoNotOptimize(sumY);
		benchmark::DoNotOptimize(sumX);
		benchmark::DoNotOptimize(sumZXY);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(Array_AsPointer);


union compoment {
	__m256d VLT;
	struct {
		double VAPOR;
		double LIQUID;
		double TOTAL;
	};
};

struct Equ_v
{
	compoment Components[NOC];
	double VF;
};

/**
 * Can manual use parallel sum instruction be better ?
 */
static void Array_ManualVectorization(benchmark::State& state) {
	compoment sum;
	sum.VLT = _mm256_setzero_pd();
	Equ_v equObj;
	equObj.VF = 1;
	Equ_v* equ = &equObj;
	double sumZXY;

	// Code before the loop is not measured
	for (auto _ : state) {

		auto* data = equObj.Components;

		for (long i = 0; ++data, i < NOC; ++i)
		{
			sum.VLT = _mm256_add_pd(data->VLT, sum.VLT);
		}

		sumZXY = -sum.TOTAL + equObj.VF * sum.VAPOR + (1 - equObj.VF) * sum.LIQUID;
		benchmark::DoNotOptimize(sum);
		benchmark::DoNotOptimize(sumZXY);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(Array_ManualVectorization);


/**
 * What about rewriting the equation ?
 * we do : sumZXY += -equ->X[TOTAL][i] + (equ->VF * equ->X[VAPOR][i] + (1.0 - equ->VF)*equ->X[LIQUID][i]);
 * So for 2 components :
 *   SumZYX = -X[Total][C1] + VF * X[Vapor][C1] + (1.0-VF)*X[LIQUID][C1]
 *         +  -X[Total][C2] + VF * X[Vapor][C2] + (1.0-VF)*X[LIQUID][C2] 
 * Which can be rewritten :
 *   SumZYX = -(X[Total][C1] +X[Total]C2]) + VF *(X[Vapor][C1] + X[Vapor][C2]) + (1-VF)*( X[LIquid][C1] + X[Liquid][C2])
 */
static void Array_MathRewrite(benchmark::State& state) {
	
	double sumX, sumY, sumT, sumZXY;
	Equ_t equObj;
	equObj.VF = 1;
	Equ_t* equ = &equObj;

	// Code before the loop is not measured
	for (auto _ : state) {
		for (long i = 0; i < NOC; ++i)
		{
			sumY += equ->X[VAPOR][i];
			sumX += equ->X[LIQUID][i];
			sumT += equ->X[TOTAL][i];
		}

		sumZXY = -sumT
			+ equ->VF * sumY
			+ (1.0 - equ->VF) * sumX;

		benchmark::DoNotOptimize(sumY);
		benchmark::DoNotOptimize(sumX);
		benchmark::DoNotOptimize(sumT);
		benchmark::DoNotOptimize(sumZXY);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(Array_MathRewrite);


static void Array_DuffDevice(benchmark::State& state) {
		
	double sumX, sumY, sumT, sumZXY;
	double sumX2, sumY2, sumT2;
	Equ_t equObj;
	equObj.VF = 1;
	Equ_t* equ = &equObj;

	// Code before the loop is not measured
	for (auto _ : state) {
					
		for (long i = 0; i < NOC; i += 2)
		{
			sumY += equ->X[VAPOR][i];
			sumY2 += equ->X[VAPOR][i + 1];
			sumX += equ->X[LIQUID][i];
			sumX2 += equ->X[LIQUID][i + 1];
			sumT += equ->X[TOTAL][i];
			sumT2 += equ->X[TOTAL][i + 1];
		}

		sumY += sumY2;
		sumX += sumX2;
		sumT += sumT2;

		if ((NOC % 2 == 1))
		{
			sumY += equ->X[VAPOR][NOC-1];
			sumX += equ->X[LIQUID][NOC - 1];
			sumT += equ->X[TOTAL][NOC - 1];
		}

		sumZXY = -sumT
			+ equ->VF * sumY
			+ (1.0 - equ->VF) * sumX;

		benchmark::DoNotOptimize(sumY2);
		benchmark::DoNotOptimize(sumY);
		benchmark::DoNotOptimize(sumX2);
		benchmark::DoNotOptimize(sumX);
		benchmark::DoNotOptimize(sumT2);
		benchmark::DoNotOptimize(sumT);
		benchmark::DoNotOptimize(sumZXY);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(Array_DuffDevice);