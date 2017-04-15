#include "MathEvaluator.h"

#include <tinyexpr.h>

using namespace gram;
using namespace std;

MathEvaluator::MathEvaluator(vector<pair<double, double>> inputsOutputs, shared_ptr<ContextFreeMapper> mapper)
    : inputsOutputs(inputsOutputs), mapper(mapper) {
  //
}

double MathEvaluator::evaluate(const Genotype& genotype) noexcept {
  string program;

  try {
    program = mapper->map(genotype);
  } catch (...) {
    return 1000;
  }

  double& fitness = phenotypeCache[program];

  if (fitness) {
    return fitness;
  }

  fitness = 0.0;

  double xValue = inputsOutputs[0].first;
  double& x = xValue;
  te_variable variables[] = {{"x", &x}};

  int error;
  te_expr* expression = te_compile(program.c_str(), variables, 2, &error);

  if (!expression) {
    return 1000;
  }

  for (auto& inputOutput : inputsOutputs) {
    x = inputOutput.first;

    double result = te_eval(expression);
    double diff = inputOutput.second - result;

    fitness += diff * diff;
  }

  te_free(expression);

  return fitness;
}
