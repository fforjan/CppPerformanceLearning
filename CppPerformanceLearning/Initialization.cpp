#include "../GoogleBenchmark/benchmark/include/benchmark/benchmark.h"

const int NOC = 256;

static void Initialization(benchmark::State& state) {
	auto source = new double[NOC];
	auto dest = new double*[NOC];
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

/**
 * One idea could be to use STL to initialize this ?
 */
static void Initialization_STL(benchmark::State& state) {
	auto source = new double[NOC];
	auto dest = new double* [NOC];
	for (int i = 0; i < NOC; i++)
	{
		source[i] = i;
		dest[i] = new double[NOC];
	}

	// Code before the loop is not measured
	for (auto _ : state) {

		for (int i = 0; i < NOC; i++)
		{
			std::fill_n(dest[i], NOC, source[i]);
		}

		benchmark::DoNotOptimize(source);
		benchmark::DoNotOptimize(dest);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(Initialization_STL);


/**
 * One idea could be to use STL to initialize this ?
 */
static void Initialization_SingleArray(benchmark::State& state) {
	auto source = new double[NOC];
	auto dest = new double[NOC*NOC];
	for (int i = 0; i < NOC; i++)
	{
		source[i] = i;		
	}

	// Code before the loop is not measured
	for (auto _ : state) {

		auto dataPointer = dest;
		for (int i = 0; i < NOC; ++i)
		{
			for (int j = 0; j < NOC; ++dataPointer,++j)
			{
				*dataPointer = source[i];
			}
		}

		benchmark::DoNotOptimize(source);
		benchmark::DoNotOptimize(dest);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(Initialization_SingleArray);

