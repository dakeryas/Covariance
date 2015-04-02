#ifndef STATE_H
#define STATE_H

#include <iomanip>
#include <vector>
#include <utility>
#include "UniformRatio.hpp"

template <class T> 
class State{ //node interface

protected:
  std::vector<std::unique_ptr<State<T>>> daughters;
  std::vector<std::unique_ptr<Ratio>> ratios;//ratios to the daughters
  
public:
  State()= default;
  State(std::vector<State<T>*> daughters, std::vector<Ratio*> ratios);//it always takes ownership of the pointers passed, give the ratios to the daughters
  State(std::vector<State<T>*> daughters);//it always takes ownership of the pointers passed
  State(const State<T>& other);
  State(State<T>&& other) = default;
  State<T>& operator=(const State<T>& other);
  State<T>& operator=(State<T>&& other) = default;
  virtual ~State()= default;//to allow for proper destruction of the derived classes
  virtual unsigned getDimensionOfRealisations() const = 0;//returns the dimension of the first found data stored in the daughters
  virtual T getRealisation(std::mt19937& randomGenerator) = 0;//picks random ratios and returns the resulting spectrum or returns the spectrum for a leaf
  const std::vector<std::unique_ptr<State<T>>>& getDaughters() const;
  std::vector<std::unique_ptr<State<T>>>& getDaughters();//returns assignable reference to the daughters
  virtual void addDaughter(State<T>* daughter);
  virtual void addDaughter(State<T>* daughter, Ratio* ratio);
  virtual std::unique_ptr<State<T>> clone() const = 0;//for the copy constructor that needs polymorphism
  static void print(std::ostream& output, const State<T>& state, unsigned depth);//print state knowing its depth in the root state that contains it
  
};

template <class T>
void State<T>::print(std::ostream& output, const State<T>& state, unsigned depth){

  output<<std::setw(depth)<<' '<<state.getDaughters().size()<<"\n";
  for(const auto& daughter : state.getDaughters()) State<T>::print(output, *daughter, depth+4);
  
}

template <class T>
std::ostream& operator<<(std::ostream& output, const State<T>& state){
  
  State<T>::print(output, state, 0);
  return output;
  
}

template <class T>
State<T>::State(std::vector<State<T>*> daughters, std::vector<Ratio*> ratios):daughters(daughters.begin(),daughters.end()),ratios(ratios.begin(),ratios.end()){
  
}

template <class T>
State<T>::State(std::vector< State<T>* > daughters):State(daughters,{}){
  
  for(const auto& daughter : daughters) ratios.emplace_back(new UniformRatio);

}

template <class T>
State<T>::State(const State<T>& other){//we don't want to move the unique_ptr's from other so we have to litteraly clone them

  daughters.reserve(other.daughters.size());
  for(const auto& daughter : other.daughters) this->daughters.emplace_back(daughter->clone());
  ratios.reserve(other.ratios.size());
  for(const auto& ratio : other.ratios) this->ratios.emplace_back(ratio->clone());
  
}

template <class T>
State<T>& State<T>::operator=(const State<T>& other){
  
  daughters.resize(other.daughters.size());
  for(auto it = std::make_pair(daughters.begin(), other.daughters.begin()); it.first != daughters.end() && it.second != other.daughters.end(); ++it.first, ++it.second)
    *it.first = std::move((*it.second)->clone());
  
  ratios.resize(other.ratios.size());
  for(auto it = std::make_pair(ratios.begin(), other.ratios.begin()); it.first != ratios.end() && it.second != other.ratios.end(); ++it.first, ++it.second)
    *it.first = std::move((*it.second)->clone());

}

template <class T>
const std::vector<std::unique_ptr<State<T>>>& State<T>::getDaughters() const{
  
  return daughters;

}

template <class T>
std::vector<std::unique_ptr<State<T>>>& State<T>::getDaughters(){
  
  return daughters;

}

template <class T>
void State<T>::addDaughter(State<T>* daughter, Ratio* ratio){
  
  daughters.emplace_back(daughter);
  ratios.emplace_back(ratio);
  
}

template <class T>
void State<T>::addDaughter(State<T>* daughter){
  
  addDaughter(daughter, new UniformRatio);
  
}
#endif