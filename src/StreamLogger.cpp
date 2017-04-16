#include "StreamLogger.h"

using namespace gram;
using namespace std;

StreamLogger::StreamLogger(ostream& stream,
                           shared_ptr<IndividualComparer> comparer,
                           shared_ptr<ContextFreeMapper> mapper)
    : stream(stream), comparer(comparer), mapper(mapper) {
  //
}

void StreamLogger::logProgress(const Population& population) {
  //
}

void StreamLogger::logResult(const Population& population) {
  const Individual& result = population.bestIndividual(*comparer);

  string generation = to_string(population.generationNumber());
  string fitness = to_string(result.fitness());
  string expression = result.serialize(*mapper);

  stream << generation << "\t" << fitness << "\t" << expression << endl;
}
