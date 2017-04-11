#ifndef GRAM_MATH_EVALUATOR
#define GRAM_MATH_EVALUATOR

#include <gram/evaluation/MultiThreadEvaluator.h>
#include <gram/language/mapper/ContextFreeMapper.h>

class MathEvaluator : public gram::MultiThreadEvaluator {
 public:
  MathEvaluator(std::shared_ptr<gram::ContextFreeMapper> mapper);
  double evaluate(gram::Individual& individual);
  std::unique_ptr<gram::MultiThreadEvaluator> clone();
 private:
  std::shared_ptr<gram::ContextFreeMapper> mapper;
  std::unordered_map<std::string, double> storedFitness;
};

#endif // GRAM_MATH_EVALUATOR
