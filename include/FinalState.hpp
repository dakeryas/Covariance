#ifndef FINALSTATE_H
#define FINALSTATE_H

#include "State.hpp"

template <class T> 
class FinalState: public State<T>{ //that is a leaf and it only needs an actual spectrum

  T spectrum;

public:
  FinalState(const T& spectrum);
  unsigned getDimensionOfRealisations() const;
  T getRealisation(std::mt19937&);//returns the resulting spectrum which does not require a random generator
  void setSpectrum(const T& spectrum);
  std::unique_ptr<State<T>> clone() const;
  
};

template <class T>
FinalState<T>::FinalState(const T& spectrum):spectrum(spectrum){
  
}  

template <class T>
unsigned FinalState<T>::getDimensionOfRealisations() const{
  
  return spectrum.getDimension();

}

template <class T>
T FinalState<T>::getRealisation(std::mt19937&){
  
  return spectrum;
  
};

template <class T>
void FinalState<T>::setSpectrum(const T& spectrum){
  
  this->spectrum = spectrum;
  
}

template <class T>
std::unique_ptr< State< T > > FinalState<T>::clone() const{
  
  return std::unique_ptr<State<T>>(new FinalState<T>(*this));

}

#endif