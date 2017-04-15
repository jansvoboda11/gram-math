#include "StreamLogger.h"

using namespace gram;
using namespace std;

StreamLogger::StreamLogger(ostream& stream, std::shared_ptr<gram::ContextFreeMapper> mapper)
    : stream(stream), mapper(mapper) {
  //
}

void StreamLogger::logProgress(const Population& population) {
  //
}

void StreamLogger::logResult(const Population& population) {
  const Individual& result = population.bestIndividual();

  string generation = to_string(population.generationNumber());
  string fitness = to_string(result.fitness());
  string program = result.serialize(*mapper);

  stream << generation << "\t" << fitness << "\t" << program << endl;
}
