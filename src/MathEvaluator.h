#ifndef GRAM_MATH_EVALUATOR
#define GRAM_MATH_EVALUATOR

#include <gram/evaluation/Evaluator.h>
#include <gram/language/mapper/ContextFreeMapper.h>

class MathEvaluator : public gram::Evaluator {
 public:
  MathEvaluator(std::vector<std::pair<double, double>> inputsOutputs, std::shared_ptr<gram::ContextFreeMapper> mapper);
  double evaluate(const gram::Genotype& genotype) noexcept;
 private:
  std::vector<std::pair<double, double>> inputsOutputs;
  std::shared_ptr<gram::ContextFreeMapper> mapper;
  std::unordered_map<std::string, double> phenotypeCache;
};

#endif // GRAM_MATH_EVALUATOR
