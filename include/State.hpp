#ifndef STATE_H
#define STATE_H

#include <vector>
#include <utility>
#include "Ratio.hpp"

template <class T> 
class State{ //node interface

private:
  std::vector<std::unique_ptr<State<T>>> daughters;
  
  State(const State<T>& other);
  State<T>& operator=(const State<T>& other);
  
public:
  State(std::vector<State<T>*> daughters);
  virtual ~State()= default;
  virtual T getRealisation() =0;//picks random ratios and returns the resulting spectrum or returns the spectrum for a leaf
  const std::vector<std::unique_ptr<State<T>>>& getDaughters() const;
  
};

template <class T>
std::ostream& operator<<(std::ostream& output, const State<T>& state){
  
  output<<state.getRealisation()<<"\n\t";
  for(const auto& daughter : state.getDaughters()) output<<*daughter;
  return output;
  
}

template <class T>
const std::vector<std::unique_ptr<State<T>>>& State<T>::getDaughters() const{
  
  return daughters;

}

#endif