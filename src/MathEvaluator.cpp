#include "MathEvaluator.h"

#include <iostream>

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
    double result = run(program, inputs[i]);

    double diff = correctResults[i] - result;

    fitness += diff * diff;
  }

  cout << "program: ";
  cout << program << endl;
  cout << "fitness: ";
  cout << to_string(fitness) << endl << endl;

  return fitness;
}

std::unique_ptr<MultiThreadEvaluator> MathEvaluator::clone() {
  return make_unique<MathEvaluator>(mapper);
}

double MathEvaluator::run(string program, double x) {
  string output = execute("python3 -c \"x = " + to_string(x) + "; print(" + program + ")\"");

  try {
    return stod(output);
  } catch (...) {
    return 1000.0;
  }
}

string MathEvaluator::execute(const string& command) {
  FILE *pipe = popen(command.c_str(), "r");

  if (!pipe) {
    throw runtime_error("Could not open command line.");
  }

  string result;
  char buffer[128];

  while (fgets(buffer, 128, pipe)) {
    result += buffer;
  }

  pclose(pipe);
  return result;
}
