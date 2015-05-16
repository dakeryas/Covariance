#ifndef INTERMEDIATE_STATE_H
#define INTERMEDIATE_STATE_H

#include "Eigen/Core"
#include "State.hpp"

template <class T> 
class UnstableState: public State<T>{ //node class using the Mersenne Twister generator for the ratios of the edges

public:
  UnstableState() = default;
  UnstableState(std::vector<State<T>*> daughters, std::vector<Ratio*> ratios);//it always takes ownership of the pointers passed
  UnstableState(std::vector<State<T>*> daughters);
  unsigned getDimensionOfRealisations() const;
  T getTemplateRealisation() const;
  T getRealisation(std::mt19937& randomGenerator);//picks random ratios and returns the resulting spectrum
  std::unique_ptr<State<T>> clone() const;

};

template <class T>
UnstableState<T>::UnstableState(std::vector<State<T>*> daughters, std::vector<Ratio*> ratios):State<T>(daughters,ratios){
  
}

template <class T>
UnstableState<T>::UnstableState(std::vector<State<T>*> daughters):State<T>(daughters){
  
}

template <class T>
unsigned UnstableState<T>::getDimensionOfRealisations() const{

  for(const auto& daughter : this->daughters) return daughter->getDimensionOfRealisations();
  return 0;
  
}

template <class T>
T UnstableState<T>::getTemplateRealisation() const{

  for(const auto& daughter : this->daughters) return daughter->getTemplateRealisation();
  return T();
  
}

template <class T>
T UnstableState<T>::getRealisation(std::mt19937& randomGenerator){

  T realisation;//use a temporary to allow for a 'std::move'
  
  Eigen::ArrayXd ratioValues(this->ratios.size());//need "this" for the dependent name lookup in the base class
  for(auto itPair = std::make_pair(ratioValues.data(), this->ratios.begin()); itPair.first != ratioValues.data()+this->ratios.size() && itPair.second != this->ratios.end(); ++itPair.first, ++itPair.second)
    *itPair.first = (*itPair.second)->getRealisation(randomGenerator);
  ratioValues = ratioValues/ratioValues.sum();

  for(auto itPair = std::make_pair(ratioValues.data(), this->daughters.begin()); itPair.first != ratioValues.data()+ratioValues.size() && itPair.second != this->daughters.end(); ++itPair.first, ++itPair.second)
    realisation += (*itPair.first) * (*itPair.second)->getRealisation(randomGenerator);

  return realisation;
  
};

template <class T>
std::unique_ptr< State< T > > UnstableState<T>::clone() const{
  
  return std::unique_ptr<State<T>>(new UnstableState<T>(*this));

}

#endif