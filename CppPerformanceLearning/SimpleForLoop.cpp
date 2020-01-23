
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

/**
 * One  optimization is to consider that getter and setter should not be virtual when possible
 */
class StateNoVirtual
{
private:
	int noc;
	double* data;

public:
	StateNoVirtual(int NOC) {
		this->noc = NOC;
		this->data = new double[NOC];
		std::fill_n(this->data, NOC, 0.5);

	}
	long getNOC() { return noc; }

	double* getData() { return data; }
};

static void SimpleLoop_NoVirtual(benchmark::State& state) {

	StateNoVirtual* componentState = new StateNoVirtual(100);
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
BENCHMARK(SimpleLoop_NoVirtual);


/**
 * Other optimization is to enable the compiler to do the optimization because the object is constant
 */
class StateConst
{
private:
	int noc;
	double* data;

public:
	StateConst(int NOC) {
		this->noc = NOC;
		this->data = new double[NOC];
		std::fill_n(this->data, NOC, 0.5);

	}
	virtual long getNOC() const { return noc; }

	virtual double* getData() const { return data; }
};

static void SimpleLoop_Const(benchmark::State& state) {	
	const StateConst componentState(100);
	// Code before the loop is not measured
	for (auto _ : state) {
		for (int i = 0; i < componentState.getNOC(); i++)
		{
			if (componentState.getData()[i] > 0.9)
				break;
		}

		benchmark::DoNotOptimize(componentState);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(SimpleLoop_Const);


/**
 *  one of the issue with the previous is to call the getNOC method and getData into each iterations
 *  those call are costly and can be easily avoided
 */
static void SimpleLoop_LocalVariables(benchmark::State& state) {

	State* componentState = new State(100);
	// Code before the loop is not measured
	for (auto _ : state) {
		auto noc = componentState->getNOC();
		auto data = componentState->getData();
		for (int i = 0; i < noc; i++)
		{
			if (data[i] > 0.9)
				break;
		}

		benchmark::DoNotOptimize(componentState);
		benchmark::ClobberMemory();
	}
}
BENCHMARK(SimpleLoop_LocalVariables);