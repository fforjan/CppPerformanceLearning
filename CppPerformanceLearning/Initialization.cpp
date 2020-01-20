#include "../GoogleBenchmark/benchmark/include/benchmark/benchmark.h"

const int NOC = 256;

double * source;
double ** dest;

static void Initialization(benchmark::State& state) {
	source = new double[NOC];
	dest = new double*[NOC];
	for(int i=0; i < NOC; i++)
	{
		source[i] = i;
		dest[i] = new double[NOC];
	}
	
	// Code before the loop is not measured
	for (auto _ : state) {

		for (int i = 0; i < NOC; i++)
		{
			for (int j = 0; i < NOC; i++)
			{
				dest[i][j] = source[i];
			}
		}
		
		benchmark::DoNotOptimize(source);
		benchmark::DoNotOptimize(dest);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(Initialization);