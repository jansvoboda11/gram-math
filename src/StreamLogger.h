#ifndef GRAM_MATH_STREAM_LOGGER
#define GRAM_MATH_STREAM_LOGGER

#include <gram/language/mapper/ContextFreeMapper.h>
#include <gram/util/logger/Logger.h>

#include <ostream>

class StreamLogger : public gram::Logger {
 public:
  StreamLogger(std::ostream& stream, std::shared_ptr<gram::ContextFreeMapper> mapper);
  void logProgress(const gram::Population& population);
  void logResult(const gram::Population& population);
 private:
  std::ostream& stream;
  std::shared_ptr<gram::ContextFreeMapper> mapper;
};

#endif // GRAM_MATH_STREAM_LOGGER
