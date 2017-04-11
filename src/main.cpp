#include <fstream>
#include <iostream>

#include <gram/evaluation/driver/SingleThreadDriver.h>
#include <gram/individual/crossover/OnePointCrossover.h>
#include <gram/individual/mutation/NumberMutation.h>
#include <gram/language/mapper/ContextFreeMapper.h>
#include <gram/language/parser/BnfRuleParser.h>
#include <gram/population/initializer/RandomInitializer.h>
#include <gram/population/selector/TournamentSelector.h>
#include <gram/util/bool_generator/TwisterBoolGenerator.h>
#include <gram/util/logger/NullLogger.h>
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
 public:
  FakeEvaluator(shared_ptr<ContextFreeMapper> mapper) : mapper(mapper) {};

  double evaluate(Individual& individual) {
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
       1.0
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
       4.0000
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

  double run(string program, double x) const {
    string output = execute("python3 -c \"x = " + to_string(x) + "; print(" + program + ")\"");

    try {
      return stod(output);
    } catch (...) {
      return 1000.0;
    }
  }

 private:
  shared_ptr<ContextFreeMapper> mapper;
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

  auto numberGenerator1 = make_unique<TwisterNumberGenerator>();
  auto numberGenerator2 = make_unique<TwisterNumberGenerator>();
  auto numberGenerator3 = make_unique<TwisterNumberGenerator>();
  auto numberGenerator4 = make_unique<TwisterNumberGenerator>();
  auto boolGenerator = make_unique<TwisterBoolGenerator>(1.0);

  auto selector = make_unique<TournamentSelector>(5, move(numberGenerator1));
  auto mutation = make_unique<NumberMutation>(move(boolGenerator), move(numberGenerator2));
  auto crossover = make_unique<OnePointCrossover>(move(numberGenerator3));
  auto reproducer = make_shared<Reproducer>(move(selector), move(crossover), move(mutation));

  BnfRuleParser parser;

  auto grammar = make_shared<ContextFreeGrammar>(parser.parse(grammarString));
  auto mapper = make_shared<ContextFreeMapper>(grammar, 3);

  RandomInitializer initializer(move(numberGenerator4), 200);

  auto evaluator = make_unique<FakeEvaluator>(mapper);
  auto evaluationDriver = make_unique<SingleThreadDriver>(move(evaluator));
  auto logger = make_unique<NullLogger>();

  Evolution evolution(move(evaluationDriver), move(logger));

  Population population = initializer.initialize(500, reproducer);

  Individual result = evolution.run(population, [](Population& currentPopulation) -> bool {
    return currentPopulation.bestFitness() < 0.00001;
  });

  cout << "result: " << result.getFitness() << " : " << result.serialize(*mapper) << endl;

  return 0;
}
