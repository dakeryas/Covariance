#ifndef ELAPSED_TIME_H
#define ELAPSED_TIME_H

#include <chrono>
#include <iostream>
#include <iomanip>

class ElapsedTime{ //node interface

  std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::_V2::steady_clock::duration> startTime;
  
public:
  ElapsedTime();
  template <class CastDuration>
  CastDuration getValue() const;

  
};

template <class V, class R>
std::ostream& operator<<(std::ostream& output, const std::chrono::duration<V,R>& duration){
  
  auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
  auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration % std::chrono::hours(1));//the rest of the division by hours(1) is in milliseconds (if R = milli) and the result is cast into minutes
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration % std::chrono::minutes(1));
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration % std::chrono::seconds(1));

  output<<std::setfill('0');
  if(hours.count() > 0) output<<std::setw(2)<< hours.count()<<"h ";
  if(minutes.count() > 0) output<< std::setw(2)<< minutes.count()<<"m ";
  if(seconds.count() > 0) output<< std::setw(2)<< seconds.count()<<"s ";
  output<<std::setw(3)<<milliseconds.count()<<"ms";
  
  return output;
  
}

std::ostream& operator<<(std::ostream& output, const ElapsedTime& elapsedTime);

template <class CastDuration>
CastDuration ElapsedTime::getValue() const{

  return std::chrono::duration_cast<CastDuration>(std::chrono::steady_clock::now() - startTime);
  
}

#endif