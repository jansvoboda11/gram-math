#include <iostream>
#include <fstream>

#include <gram/Evolution.h>
#include <gram/util/number_generator/TwisterNumberGenerator.h>
#include <gram/population/selector/TournamentSelector.h>
#include <gram/util/bool_generator/TwisterBoolGenerator.h>
#include <gram/population/initializer/RandomInitializer.h>
#include <gram/language/parser/BnfRuleParser.h>

using namespace gram::evolution;
using namespace gram::individual;
using namespace gram::population;
using namespace gram::language;
using namespace gram::util;

class FakeEvaluator : public Evaluator {
  int evaluate(std::string program) {
    return 0;
  }
};

class FakeFitnessCalculator : public FitnessCalculator {
 public:
  double calculate(int desired, int actual) {
    return std::abs(desired - actual);
  }
};

std::string loadFile(std::string name) {
  std::ifstream grammarFile(name);

  if (!grammarFile.is_open()) {
    return "";
  }

  std::string content((std::istreambuf_iterator<char>(grammarFile)), std::istreambuf_iterator<char>());

  return content;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    return 1;
  }

  std::string grammarString = loadFile(argv[1]);

  unsigned long max = std::numeric_limits<unsigned long>::max();

  std::unique_ptr<NumberGenerator> numberGenerator1 = std::make_unique<TwisterNumberGenerator>(max);
  std::unique_ptr<NumberGenerator> numberGenerator2 = std::make_unique<TwisterNumberGenerator>(29);
  std::unique_ptr<NumberGenerator> numberGenerator3 = std::make_unique<TwisterNumberGenerator>(11);
  std::unique_ptr<NumberGenerator> numberGenerator4 = std::make_unique<TwisterNumberGenerator>(11);
  std::unique_ptr<BoolGenerator> boolGenerator = std::make_unique<TwisterBoolGenerator>(0.1);

  TournamentSelector selector(std::move(numberGenerator1));
  Mutation mutation(std::move(boolGenerator), std::move(numberGenerator2));
  Crossover crossover(std::move(numberGenerator3));
  auto generator = std::make_shared<Generator>(selector, crossover, mutation);

  BnfRuleParser parser;

  std::shared_ptr<Grammar> grammar = parser.parse(grammarString);

  Mapper mapper(grammar);
  auto language = std::make_shared<Language>(grammar, mapper);

  RandomInitializer initializer(std::move(numberGenerator4), language, 100);

  FakeEvaluator evaluator;
  FakeFitnessCalculator calculator;
  auto processor = std::make_shared<Processor>(evaluator, calculator);

  Evolution evolution(processor);

  Population population = initializer.initialize(1000, generator);

  Individual result = evolution.run(population, 1470);

  std::cout << result.fitness() << " : " << result.serialize() << std::endl;

  return 0;
}
