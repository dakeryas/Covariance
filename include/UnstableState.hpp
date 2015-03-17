#ifndef INTERMEDIATE_STATE_H
#define INTERMEDIATE_STATE_H

#include "Eigen/Core"
#include "State.hpp"
#include "UniformRatio.hpp"

template <class T> 
class UnstableState: public State<T>{ //node class using the Mersenne Twister generator for the ratios of the edges

  std::vector<std::unique_ptr<Ratio>> ratios;

public:
  UnstableState(std::vector<State<T>*> daughters, std::vector<Ratio*> ratios);
  UnstableState(std::vector<State<T>*> daughters);
  unsigned getDimensionOfRealisations() const;
  T getRealisation();//picks random ratios and returns the resulting spectrum
  void addDaughter(State<T>* daughter);
  void addDaughter(State<T>* daughter, Ratio* ratio);
  
};

template <class T>
UnstableState<T>::UnstableState(std::vector<State<T>*> daughters, std::vector<Ratio*> ratios):State<T>(daughters),ratios(ratios.begin(),ratios.end()){
  
}

template <class T>
UnstableState<T>::UnstableState(std::vector<State<T>*> daughters):UnstableState(daughters, {}){
  
  for(const auto& daughter : State<T>::daughters) ratios.emplace_back(new UniformRatio);
  
}

template <class T>
unsigned UnstableState<T>::getDimensionOfRealisations() const{

  for(const auto& daughter : State<T>::daughters) return daughter->getDimensionOfRealisations();
  return 0;
  
}

template <class T>
T UnstableState<T>::getRealisation(){
  
  T realisation;//use a temporary to allow for a 'std::move'
  realisation.setZero();
  
  Eigen::ArrayXd ratioValues(ratios.size());
  for(auto itPair = std::make_pair(ratioValues.data(), ratios.begin()); itPair.first != ratioValues.data()+ratios.size() && itPair.second != ratios.end(); ++itPair.first, ++itPair.second)
    *itPair.first = (*itPair.second)->getRealisation();
  ratioValues = ratioValues/ratioValues.sum();
  
  for(auto itPair = std::make_pair(ratioValues.data(), State<T>::daughters.begin()); itPair.first != ratioValues.data()+ratios.size() && itPair.second != State<T>::daughters.end(); ++itPair.first, ++itPair.second)
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

#endif