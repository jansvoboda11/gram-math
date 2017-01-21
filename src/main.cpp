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

    // y = x * x + x + 1

    double result1 = run(program, -10.0);
    double result2 = run(program,  -5.0);
    double result3 = run(program,   0.0);
    double result4 = run(program,   5.0);
    double result5 = run(program,  10.0);
    double fitness = abs(91.0 - result1) + abs(21.00 - result2) + abs(1.0 - result3) + abs(31.0 - result4) + abs(111.0 - result5);

    cout << "program: ";
    cout << program << endl;
    cout << "results: ";
    cout << to_string(result1) << ", ";
    cout << to_string(result2) << ", ";
    cout << to_string(result3) << ", ";
    cout << to_string(result4) << ", ";
    cout << to_string(result5) << endl;
    cout << "fitness: ";
    cout << to_string(fitness) << endl;

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

  RandomInitializer initializer(move(numberGenerator4), grammar, 30);

  unique_ptr<Evaluator> evaluator = make_unique<FakeEvaluator>();

  Evolution evolution(move(evaluator));

  Population population = initializer.initialize(40, reproducer);

  Individual result = evolution.run(population);

  cout << "result: " << result.fitness() << " : " << result.serialize() << endl;

  return 0;
}
