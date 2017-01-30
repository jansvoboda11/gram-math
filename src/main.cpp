#include <fstream>
#include <iostream>

#include <gram/language/parser/BnfRuleParser.h>
#include <gram/population/initializer/RandomInitializer.h>
#include <gram/population/selector/TournamentSelector.h>
#include <gram/util/bool_generator/TwisterBoolGenerator.h>
#include <gram/util/number_generator/TwisterNumberGenerator.h>
#include <gram/Evolution.h>

using namespace gram;
using namespace std;

string execute(const string& command) {
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

class FakeEvaluator : public Evaluator {
  double evaluate(string program) const {
    if (program.length() > 40) {
      return 1000.0;
    }

    // y = x^4 + x^3 + x^2 + x + 1

    vector<double> inputs{-10.0, -5.0, 0.0, 5.0, 10.0};
    vector<double> correctResults{9091.0, 521.0, 1.0, 781.0, 11111.0};
    double fitness = 0.0;

    for (unsigned long i = 0; i < inputs.size(); i++) {
      double result = run(program, inputs[i]);

      fitness += abs(correctResults[i] - result);
    }

    cout << "program: ";
    cout << program << endl;
    cout << "fitness: ";
    cout << to_string(fitness) << endl << endl;

    return fitness;
  }

  double run(string program, double x) const {
    string output = execute("python3 -c \"x = " + to_string(x) + "; print(" + program + ")\"");

    try {
      return stod(output);
    } catch (...) {
      return 1000.0;
    }
  }
};

string loadFile(const string& name) {
  ifstream grammarFile(name);

  if (!grammarFile.is_open()) {
    return "";
  }

  string content((istreambuf_iterator<char>(grammarFile)), istreambuf_iterator<char>());

  return content;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    return 1;
  }

  string grammarString = loadFile(argv[1]);

  unsigned long max = numeric_limits<unsigned long>::max();
  unique_ptr<NumberGenerator> numberGenerator1 = make_unique<TwisterNumberGenerator>(max);
  unique_ptr<NumberGenerator> numberGenerator2 = make_unique<TwisterNumberGenerator>(29);
  unique_ptr<NumberGenerator> numberGenerator3 = make_unique<TwisterNumberGenerator>(11);
  unique_ptr<NumberGenerator> numberGenerator4 = make_unique<TwisterNumberGenerator>(11);
  unique_ptr<BoolGenerator> boolGenerator = make_unique<TwisterBoolGenerator>(1.0);

  auto selector = make_unique<TournamentSelector>(move(numberGenerator1));
  auto mutation = make_unique<Mutation>(move(boolGenerator), move(numberGenerator2));
  auto crossover = make_unique<Crossover>(move(numberGenerator3));
  auto reproducer = make_shared<Reproducer>(move(selector), move(crossover), move(mutation));

  BnfRuleParser parser;

  auto grammar = make_shared<ContextFreeGrammar>(parser.parse(grammarString));

  RandomInitializer initializer(move(numberGenerator4), grammar, 50);

  unique_ptr<Evaluator> evaluator = make_unique<FakeEvaluator>();

  Evolution evolution(move(evaluator));

  Population population = initializer.initialize(60, reproducer);

  Individual result = evolution.run(population);

  cout << "result: " << result.fitness() << " : " << result.serialize() << endl;

  return 0;
}
