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
  State(std::vector<State<T>*> daughters);//it always takes ownership of the pointers passed
  State(const State<T>& other);
  State(State<T>&& other) = default;
  State<T>& operator=(const State<T>& other);
  State<T>& operator=(State<T>&& other) = default;
  virtual ~State()= default;//to allow for proper destruction of the derived classes
  virtual unsigned getDimensionOfRealisations() const = 0;//returns the dimension of the first found data stored in the daughters
  virtual T getRealisation() = 0;//picks random ratios and returns the resulting spectrum or returns the spectrum for a leaf
  const std::vector<std::unique_ptr<State<T>>>& getDaughters() const;
  virtual std::unique_ptr<State<T>> clone() const = 0;//for the copy constructor that needs polymorphism
  
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
State<T>::State(const State<T>& other){//we don't want to move the unique_ptr's from other so we have to litteraly clone them
  
  daughters.reserve(other.daughters.size());
  for(const auto& daughter : other.daughters) this->daughters.emplace_back(daughter->clone());
  
}

template <class T>
const std::vector<std::unique_ptr<State<T>>>& State<T>::getDaughters() const{
  
  return daughters;

}

#endif