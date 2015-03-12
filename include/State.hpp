#ifndef STATE_H
#define STATE_H

#include <vector>
#include <utility>
#include "Eigen/Core"
#include "UniformRatio.hpp"

template <class T> 
class State{ //node class using the Mersenne Twister generator for the ratios of the edges

  T spectrum;
  std::vector<std::unique_ptr<State<T>>> daughters;
  std::vector<std::unique_ptr<Ratio>> ratios;

public:
  State(std::vector<State<T>*> daughters, std::vector<Ratio*> ratios);//you may not have a fixed spectrum for such a state
  State(std::vector<State<T>> daughters);//you may not have a fixed spectrum for such a state
  State(const T& spectrum);//that is a leaf
//   State(const State<T>& other);
  State(State<T>&& other) = default;
//   State<T>& operator=(const State<T>& other);
  State<T>& operator=(State<T>&& other) = default;
  ~State() = default;
  T getRealisation();//picks random ratios and returns the resulting spectrum or returns getSpectrum() if the ratios are empty
  const T& getSpectrum();//updates spectrum and returns it
  const std::vector<State<T*>>& getDaughters();
  void setSpectrum(const T& spectrum);
  void addDaughter(const State<T>& daughter);
  void addDaughter(const State<T>& daughter, Ratio* ratio);
  
};

template <class T>
std::ostream& operator<<(std::ostream& output, const State<T>& state){
  
  output<<state.getSpectrum()<<"\n\t";
  for(auto daughter : state.getDaughters()) output<<*daughter;
  return output;
  
}

template <class T>
State<T>::State(std::vector<State<T>*> daughters, std::vector<Ratio*> ratios):daughters(daughters),ratios(ratios){
  
}

template <class T>
State<T>::State(std::vector<State<T>*> daughters):State(daughters, {}){
  
  for(auto daughter : daughters) ratios.push_back(std::unique_ptr<Ratio>(new UniformRatio));
  
}
  
template <class T>
State<T>::State(const T& spectrum):spectrum(spectrum){
  
}  

// template <class T>
// State<T>::State(const State& other):State(other.spectrum, other.daughters, std::vector<Ratio*> (other.ratios.size())){
// 
//   for(auto itPair = std::make_pair(ratios.begin(), other.ratios.begin()); itPair.first != ratios.end() && itPair.second != other.ratios.end(); ++itPair.first, ++itPair.second)
//     *itPair.first = std::move((*itPair.second)->clone());//deep copy: move semantics are used on the temporary created with 'new' in clone
//   
// }
// 
// template <class T>
// State<T>& State<T>::operator=(const State& other){
// 
//   this->spectrum = spectrum;
//   this->daughters = daughters;
//   ratios.clear();
//   for(auto itPair = std::make_pair(ratios.begin(), other.ratios.begin()); itPair.first != ratios.end() && itPair.second != other.ratios.end(); ++itPair.first, ++itPair.second)
//     *itPair.first = std::move((*itPair.second)->clone());//deep copy: move semantics are used on the temporary created with 'new' in clone
//   
//   return *this;
//   
// }

template <class T>
T State<T>::getRealisation(){
  
  if(!daughters.empty() && !ratios.empty()){
    
    T realisation;//use a temporary to allow for a 'std::move'
    realisation.setZero();
    
    Eigen::ArrayXd ratioValues(ratios.size());
    for(auto itPair = std::make_pair(ratioValues.data(), ratios.begin()); itPair.first != ratioValues.data()+ratios.size() && itPair.second != ratios.end(); ++itPair.first, ++itPair.second)
      *itPair.first = (*itPair.second)->getRealisation();
    ratioValues = ratioValues/ratioValues.sum();
    
    for(auto itPair = std::make_pair(ratioValues.data(), daughters.begin()); itPair.first != ratioValues.data()+ratios.size() && itPair.second != daughters.end(); ++itPair.first, ++itPair.second)
      realisation += (*itPair.first) * (*itPair.second)->getRealisation();
    
    return realisation;

  }
  else return getSpectrum();
  
};

template <class T>
const T& State<T>::getSpectrum(){
  
  return spectrum;
  
};

template <class T>
const std::vector<State<T>*>& State<T>::getDaughters(){
  
  return daughters;

}

template <class T>
void State<T>::setSpectrum(const T& spectrum){
  
  this->spectrum = spectrum;
  
}

template <class T>
void State<T>::addDaughter(State<T>* daughter, Ratio* ratio){
  
  daughters.push_back(std::unique_ptr<State>(daughter));
  ratios.push_back(std::unique_ptr<Ratio>(ratio));
  
}

template <class T>
void State<T>::addDaughter(State<T>* daughter){
  
  addDaughter(daughter, new UniformRatio);
  
}

#endif