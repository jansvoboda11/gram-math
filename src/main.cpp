#include <fstream>
#include <iostream>

#include <gram/evaluation/driver/MultiThreadDriver.h>
#include <gram/evaluation/driver/SingleThreadDriver.h>
#include <gram/individual/crossover/OnePointCrossover.h>
#include <gram/individual/mutation/FastCodonMutation.h>
#include <gram/language/mapper/ContextFreeMapper.h>
#include <gram/language/parser/BnfRuleParser.h>
#include <gram/population/initializer/RandomInitializer.h>
#include <gram/population/selector/TournamentSelector.h>
#include <gram/util/logger/NullLogger.h>
#include <gram/util/number_generator/MinimalNumberGenerator.h>
#include <gram/Evolution.h>

#include "MathEvaluator.h"

using namespace gram;
using namespace std;

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

  auto numberGenerator1 = make_unique<MinimalNumberGenerator>();
  auto numberGenerator2 = make_unique<MinimalNumberGenerator>();
  auto numberGenerator3 = make_unique<MinimalNumberGenerator>();
  auto numberGenerator4 = make_unique<MinimalNumberGenerator>();
  auto numberGenerator5 = make_unique<MinimalNumberGenerator>();
  auto stepGenerator = make_unique<BernoulliDistributionStepGenerator>(0.1, move(numberGenerator5));

  auto selector = make_unique<TournamentSelector>(5, move(numberGenerator1));
  auto mutation = make_unique<FastCodonMutation>(move(stepGenerator), move(numberGenerator2));
  auto crossover = make_unique<OnePointCrossover>(move(numberGenerator3));
  auto reproducer = make_shared<Reproducer>(move(selector), move(crossover), move(mutation));

  BnfRuleParser parser;

  auto grammar = make_shared<ContextFreeGrammar>(parser.parse(grammarString));
  auto mapper = make_shared<ContextFreeMapper>(grammar, 3);

  RandomInitializer initializer(move(numberGenerator4), 200);

  auto evaluator = make_unique<MathEvaluator>(mapper);
  auto evaluationDriver = make_unique<SingleThreadDriver>(move(evaluator));
  auto logger = make_unique<NullLogger>();

  Evolution evolution(move(evaluationDriver), move(logger));

  Population population = initializer.initialize(500, reproducer);

  Individual result = evolution.run(population, [](Population& currentPopulation) -> bool {
    return currentPopulation.number() >= 101 || currentPopulation.bestFitness() < 0.00001;
  });

  cout << "result: " << result.getFitness() << " : " << result.serialize(*mapper) << endl;

  return 0;
}
