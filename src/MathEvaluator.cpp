#include "MathEvaluator.h"

#include <iostream>

#include <tinyexpr.h>

using namespace gram;
using namespace std;

MathEvaluator::MathEvaluator(std::shared_ptr<gram::ContextFreeMapper> mapper) :mapper(mapper) {
  //
}

double MathEvaluator::evaluate(Individual& individual) {
  string program = individual.serialize(*mapper);

  double& fitness = storedFitness[program];

  if (fitness) {
    return fitness;
  }

  // y = x^4 + x^3 + x^2 + x
  vector<pair<double, double>> inputsOutputs{
      {-1.0,  0.0000},
      {-0.9, -0.1629},
      {-0.8, -0.2624},
      {-0.7, -0.3129},
      {-0.6, -0.3264},
      {-0.5, -0.3125},
      {-0.4, -0.2784},
      {-0.3, -0.2289},
      {-0.2, -0.1664},
      {-0.1, -0.0909},
      { 0.0,  0.0000},
      { 0.1,  0.1111},
      { 0.2,  0.2496},
      { 0.3,  0.4251},
      { 0.4,  0.6496},
      { 0.5,  0.9375},
      { 0.6,  1.3056},
      { 0.7,  1.7731},
      { 0.8,  2.3616},
      { 0.9,  3.0951},
      { 1.0,  4.0000},
  };

  fitness = 0.0;

  double& x = inputsOutputs[0].first;
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

std::unique_ptr<MultiThreadEvaluator> MathEvaluator::clone() {
  return make_unique<MathEvaluator>(mapper);
}
