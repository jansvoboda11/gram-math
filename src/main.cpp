#include <functional>
#include <iostream>

#include <gram/Evolution.h>
#include <gram/evaluation/driver/SingleThreadDriver.h>
#include <gram/individual/comparer/LowFitnessComparer.h>
#include <gram/individual/crossover/OnePointCrossover.h>
#include <gram/individual/mutation/BernoulliStepGenerator.h>
#include <gram/individual/mutation/FastCodonMutation.h>
#include <gram/language/mapper/ContextFreeMapper.h>
#include <gram/population/initializer/RandomInitializer.h>
#include <gram/population/Population.h>
#include <gram/population/reproducer/PassionateReproducer.h>
#include <gram/population/selector/TournamentSelector.h>
#include <gram/random/number_generator/XorShiftNumberGenerator.h>

#include "MathEvaluator.h"
#include "StreamLogger.h"

using namespace gram;
using namespace std;

shared_ptr<ContextFreeGrammar> getGrammar();
vector<pair<double, double>> getInputsOutputs();

int main() {
  unsigned long populationSize = 500;
  unsigned long genotypeLength = 200;
  unsigned long wrappingsLimit = 3;
  unsigned long tournamentSize = 5;
  Probability mutationProbability = Probability(0.1);
  unsigned long generationsLimit = 101;
  double sufficientFitness = 0.00001;

  auto frontEndComparer = make_shared<LowFitnessComparer>();

  std::function<bool(Population&)> successCondition = [=](Population& currentPopulation) -> bool {
    return currentPopulation.generationNumber() >= generationsLimit
        || currentPopulation.bestFitness(*frontEndComparer) < sufficientFitness;
  };

  auto grammar = getGrammar();
  auto inputsOutputs = getInputsOutputs();

  auto numberGenerator1 = make_unique<XorShiftNumberGenerator>();
  auto numberGenerator2 = make_unique<XorShiftNumberGenerator>();
  auto numberGenerator3 = make_unique<XorShiftNumberGenerator>();
  auto numberGenerator4 = make_unique<XorShiftNumberGenerator>();
  auto numberGenerator5 = make_unique<XorShiftNumberGenerator>();
  auto stepGenerator = make_unique<BernoulliStepGenerator>(mutationProbability, move(numberGenerator5));

  auto comparer = make_unique<LowFitnessComparer>();
  auto selector = make_unique<TournamentSelector>(tournamentSize, move(numberGenerator1), move(comparer));
  auto mutation = make_unique<FastCodonMutation>(move(stepGenerator), move(numberGenerator2));
  auto crossover = make_unique<OnePointCrossover>(move(numberGenerator3));
  auto reproducer = make_shared<PassionateReproducer>(move(selector), move(crossover), move(mutation));

  auto mapper = make_shared<ContextFreeMapper>(grammar, wrappingsLimit);

  RandomInitializer initializer(move(numberGenerator4), genotypeLength);

  auto evaluator = make_unique<MathEvaluator>(inputsOutputs, mapper);
  auto evaluationDriver = make_unique<SingleThreadDriver>(move(evaluator));
  auto logger = make_unique<StreamLogger>(cout, frontEndComparer, mapper);

  Evolution evolution(move(evaluationDriver), move(logger));

  Population population = initializer.initialize(populationSize, reproducer);

  Population result = evolution.run(population, successCondition);

  return 0;
}

//  <expr> ::= "(" <expr> <op> <expr> ")" | <var>
//  <op> ::= "+" | "-" | "*"
//  <var> ::= "x" | "1"
shared_ptr<ContextFreeGrammar> getGrammar() {
  auto expr = make_unique<Rule>("expr");
  auto op = make_unique<Rule>("op");
  auto var = make_unique<Rule>("var");

  auto exprNonTerminal1 = make_unique<NonTerminal>(*expr);
  auto exprNonTerminal2 = make_unique<NonTerminal>(*expr);
  auto opNonTerminal = make_unique<NonTerminal>(*op);
  auto varNonTerminal = make_unique<NonTerminal>(*var);

  auto leftBracket = make_unique<Terminal>("(");
  auto rightBracket = make_unique<Terminal>(")");
  auto plus = make_unique<Terminal>("+");
  auto minus = make_unique<Terminal>("-");
  auto asterisk = make_unique<Terminal>("*");
  auto x = make_unique<Terminal>("x");
  auto one = make_unique<Terminal>("1");

  auto exprOption1 = make_unique<Option>();
  auto exprOption2 = make_unique<Option>();
  auto opOption1 = make_unique<Option>();
  auto opOption2 = make_unique<Option>();
  auto opOption3 = make_unique<Option>();
  auto varOption1 = make_unique<Option>();
  auto varOption2 = make_unique<Option>();

  exprOption1->addTerminal(move(leftBracket));
  exprOption1->addNonTerminal(move(exprNonTerminal1));
  exprOption1->addNonTerminal(move(opNonTerminal));
  exprOption1->addNonTerminal(move(exprNonTerminal2));
  exprOption1->addTerminal(move(rightBracket));

  exprOption2->addNonTerminal(move(varNonTerminal));

  opOption1->addTerminal(move(plus));
  opOption2->addTerminal(move(minus));
  opOption3->addTerminal(move(asterisk));

  varOption1->addTerminal(move(x));
  varOption2->addTerminal(move(one));

  expr->addOption(move(exprOption1));
  expr->addOption(move(exprOption2));

  op->addOption(move(opOption1));
  op->addOption(move(opOption2));
  op->addOption(move(opOption3));

  var->addOption(move(varOption1));
  var->addOption(move(varOption2));

  auto grammar = make_shared<ContextFreeGrammar>();
  grammar->addRule(move(expr));
  grammar->addRule(move(op));
  grammar->addRule(move(var));

  return grammar;
}

//  y = x^4 + x^3 + x^2 + x
vector<pair<double, double>> getInputsOutputs() {
  return vector<pair<double, double>>({
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
  });
};
