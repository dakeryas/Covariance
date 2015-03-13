#ifndef FINALSTATE_H
#define FINALSTATE_H

#include "State.hpp"

template <class T> 
class FinalState: public State<T>{ //that is a leaf and it only needs an actual spectrum

  T spectrum;

public:
  FinalState(const T& spectrum);
  T getRealisation();//picks random ratios and returns the resulting spectrum or returns getSpectrum() if the ratios are empty
  void setSpectrum(const T& spectrum);
  
};

template <class T>
FinalState<T>::FinalState(const T& spectrum):spectrum(spectrum){
  
}  

template <class T>
T FinalState<T>::getRealisation(){
  
  return spectrum;
  
};

template <class T>
void FinalState<T>::setSpectrum(const T& spectrum){
  
  this->spectrum = spectrum;
  
}

#endif