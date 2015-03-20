#ifndef INTERMEDIATE_STATE_H
#define INTERMEDIATE_STATE_H

#include "Eigen/Core"
#include "State.hpp"
#include "UniformRatio.hpp"

template <class T> 
class UnstableState: public State<T>{ //node class using the Mersenne Twister generator for the ratios of the edges

  std::vector<std::unique_ptr<Ratio>> ratios;

public:
  UnstableState(std::vector<State<T>*> daughters, std::vector<Ratio*> ratios);//it always takes ownership of the pointers passed
  UnstableState(std::vector<State<T>*> daughters);
  UnstableState(const UnstableState<T>& other);
  UnstableState(UnstableState<T>&& other) = default;
  UnstableState<T>& operator=(const UnstableState<T>& other);
  UnstableState<T>& operator=(UnstableState<T>&& other) = default;
  unsigned getDimensionOfRealisations() const;
  T getRealisation();//picks random ratios and returns the resulting spectrum
  void addDaughter(State<T>* daughter);
  void addDaughter(State<T>* daughter, Ratio* ratio);
  std::unique_ptr<State<T>> clone() const;

};

template <class T>
UnstableState<T>::UnstableState(std::vector<State<T>*> daughters, std::vector<Ratio*> ratios):State<T>(daughters),ratios(ratios.begin(),ratios.end()){
  
}

template <class T>
UnstableState<T>::UnstableState(std::vector<State<T>*> daughters):UnstableState(daughters, {}){
  
  for(const auto& daughter : State<T>::daughters) ratios.emplace_back(new UniformRatio);
  
}

template <class T>
UnstableState<T>::UnstableState(const UnstableState< T >& other):State<T>(other){

  ratios.reserve(other.ratios.size());
  for(const auto& ratio : other.ratios) this->ratios.emplace_back(ratio->clone());

}

template <class T>
UnstableState<T>& UnstableState<T>::operator=(const UnstableState<T>& other){
  
  ratios.resize(other.ratios.size());
  for(auto it = std::make_pair(ratios.begin(), other.ratios.begin()); it.first != ratios.end() && it.second != other.ratios.end(); ++it.first, ++it.second)
    *it.first = std::move((*it.second)->clone());

}

template <class T>
unsigned UnstableState<T>::getDimensionOfRealisations() const{

  for(const auto& daughter : State<T>::daughters) return daughter->getDimensionOfRealisations();
  return 0;
  
}

template <class T>
T UnstableState<T>::getRealisation(){

  T realisation;//use a temporary to allow for a 'std::move'
  
  Eigen::ArrayXd ratioValues(ratios.size());
  for(auto itPair = std::make_pair(ratioValues.data(), ratios.begin()); itPair.first != ratioValues.data()+ratios.size() && itPair.second != ratios.end(); ++itPair.first, ++itPair.second)
    *itPair.first = (*itPair.second)->getRealisation();
  ratioValues = ratioValues/ratioValues.sum();

  for(auto itPair = std::make_pair(ratioValues.data(), State<T>::daughters.begin()); itPair.first != ratioValues.data()+ratioValues.size() && itPair.second != State<T>::daughters.end(); ++itPair.first, ++itPair.second)
    realisation += (*itPair.first) * (*itPair.second)->getRealisation();

  return realisation;
  
};

template <class T>
void UnstableState<T>::addDaughter(State<T>* daughter, Ratio* ratio){
  
  State<T>::daughters.emplace_back(daughter);
  ratios.emplace_back(ratio);
  
}

template <class T>
void UnstableState<T>::addDaughter(State<T>* daughter){
  
  addDaughter(daughter, new UniformRatio);
  
}

template <class T>
std::unique_ptr< State< T > > UnstableState<T>::clone() const{
  
  return std::unique_ptr<State<T>>(new UnstableState<T>(*this));

}

#endif