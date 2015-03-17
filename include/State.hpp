#ifndef STATE_H
#define STATE_H

#include <vector>
#include <utility>
#include "Ratio.hpp"

template <class T> 
class State{ //node interface

protected:
  std::vector<std::unique_ptr<State<T>>> daughters;
  
public:
  State()= default;
  State(std::vector<State<T>*> daughters);
  State(const State<T>& other) = delete;
  State<T>& operator=(const State<T>& other) = delete;
  virtual ~State()= default;//to allow for proper destruction of the derived classes
  virtual unsigned getDimensionOfRealisations() const = 0;//returns the dimension of the first found data stored in the daughters
  virtual T getRealisation() = 0;//picks random ratios and returns the resulting spectrum or returns the spectrum for a leaf
  const std::vector<std::unique_ptr<State<T>>>& getDaughters() const;
  
};

template <class T>
std::ostream& operator<<(std::ostream& output, const State<T>& state){
  
  output<<state.getDaughters().size()<<"\n\t";
  for(const auto& daughter : state.getDaughters()) output<<*daughter;
  return output;
  
}

template <class T>
State<T>::State(std::vector< State< T >* > daughters):daughters(daughters.begin(),daughters.end()){

}

template <class T>
const std::vector<std::unique_ptr<State<T>>>& State<T>::getDaughters() const{
  
  return daughters;

}

#endif