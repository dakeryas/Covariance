#include "ElapsedTime.hpp"

std::ostream& operator<<(std::ostream& output, const ElapsedTime& elapsedTime){
  
  output<<elapsedTime.getValue<std::chrono::milliseconds>();
  return output;
  
}

ElapsedTime::ElapsedTime():startTime(std::chrono::steady_clock::now()){
  
}
