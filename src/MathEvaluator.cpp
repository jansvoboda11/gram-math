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

  if (program.length() > 40) {
    return 1000.0;
  }

  // y = x^4 + x^3 + x^2 + x

  vector<double> inputs{
      -1.0,
      -0.9,
      -0.8,
      -0.7,
      -0.6,
      -0.5,
      -0.4,
      -0.3,
      -0.2,
      -0.1,
       0.0,
       0.1,
       0.2,
       0.3,
       0.4,
       0.5,
       0.6,
       0.7,
       0.8,
       0.9,
       1.0,
  };

  vector<double> correctResults{
      0.0000,
      -0.1629,
      -0.2624,
      -0.3129,
      -0.3264,
      -0.3125,
      -0.2784,
      -0.2289,
      -0.1664,
      -0.0909,
       0.0000,
       0.1111,
       0.2496,
       0.4251,
       0.6496,
       0.9375,
       1.3056,
       1.7731,
       2.3616,
       3.0951,
       4.0000,
  };

  double fitness = 0.0;

  for (unsigned long i = 0; i < inputs.size(); i++) {
    te_variable variables[] = {{"x", &inputs[i]}};

    int error;

    te_expr* expression = te_compile(program.c_str(), variables, 2, &error);

    if (!expression) {
      return 1000;
    }

    double result = te_eval(expression);
    te_free(expression);

    double diff = correctResults[i] - result;

    fitness += diff * diff;
  }

  return fitness;
}

std::unique_ptr<MultiThreadEvaluator> MathEvaluator::clone() {
  return make_unique<MathEvaluator>(mapper);
}
