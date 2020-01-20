
#include "../GoogleBenchmark/benchmark/include/benchmark/benchmark.h"


class State
{
private:
	int noc;
	double * data;

public:
	State(int NOC);
	virtual long getNOC();

	virtual double* getData();
	
};

State::State(int NOC)
{
	this->noc = NOC;
	this->data = new double[NOC];
	std::fill_n(this->data, NOC, 0.5);
}

long State::getNOC()
{
	return this->noc;
}

double* State::getData()
{
	return this->data;
}

static void SimpleLoop(benchmark::State& state) {

	State * componentState = new State(100);
	// Code before the loop is not measured
	for (auto _ : state) {
		for (int i = 0; i < componentState->getNOC(); i++)
		{
			if (componentState->getData()[i] > 0.9)
				break;
		}
		
		benchmark::DoNotOptimize(componentState);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(SimpleLoop);