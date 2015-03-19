#ifndef CTESTER_H
#define CTESTER_H

#include <deque>
#include "Eigen/Core"

template <class T>
class ConvergenceTester{ 
  
  std::deque<Eigen::MatrixXd> oldVar; //deque to store several covariance matrices to test the Cauchy convergence
  double epsilon;//required relative precision
  unsigned dequeMaxSize;//to have a fixed-size deque
  void feed(const Eigen::MatrixXd& var);//add another var from a sample to the deque and pop the oldest element
  
public:
  ConvergenceTester(double espilon, unsigned dequeMaxSize);//by default only use 3 levels and stops when a relative precision of epsilon is obtained
  void feed(const T& covarianceEstimator);//get the variance matrix from the sample
  double getPrecision() const;//return epsilon
  unsigned getMaxRange() const;//returns max size
  bool converges() const;//returns convergence
  void setMaxRange(unsigned newDequeMaxSize); //change the deque's max size
  void setPrecision(double epsilon);//change epsilon's value

};

template <class T>
std::ostream& operator<<(std::ostream& output, const ConvergenceTester<T>& convergenceTester){
  
  output<<"Relative accuracy demanded = "<<convergenceTester.getPrecision()<<"\n"
    <<"Number of close matrices demanded = "<<"\n"<<convergenceTester.getMaxRange()<<"\n";
  return output;
  
}


template <class T>
ConvergenceTester<T>::ConvergenceTester(double espilon, unsigned dequeMaxSize):epsilon(espilon),dequeMaxSize(dequeMaxSize){

}

template <class T>
void ConvergenceTester<T>::feed(const Eigen::MatrixXd& var){
  
  if(oldVar.empty() || var.size() == oldVar.front().size()){
    
    if(oldVar.size() < dequeMaxSize){
      
      oldVar.emplace_back(var);
      
    }
    else{
      
      oldVar.pop_front();
      oldVar.emplace_back(var);
      
    }
    
  }

}

template <class T>
void ConvergenceTester<T>::feed(const T& covarianceEstimator){
  
  feed(covarianceEstimator.getCovarianceMatrix());

}

template <class T>
double ConvergenceTester<T>::getPrecision() const{
  
  return epsilon;

}

template <class T>
unsigned ConvergenceTester<T>::getMaxRange() const{
  
  return dequeMaxSize;

}

template <class T>
bool ConvergenceTester<T>::converges() const{

  if(oldVar.size() != dequeMaxSize) return false; //if the deque is not full yet (or even empty), it doesn't make sense to say it converges
  else{
    
    for (auto itVar = oldVar.begin()+1; itVar !=oldVar.end(); ++itVar)
      if (!(oldVar.front() - *itVar).isMuchSmallerThan(oldVar.front(), epsilon)) return false;
    
    return true;
    
  }
  
}

template <class T>
void ConvergenceTester<T>::setMaxRange(unsigned newDequeMaxSize){
  
  if(newDequeMaxSize < dequeMaxSize) oldVar.resize(newDequeMaxSize);
  else dequeMaxSize = newDequeMaxSize; // we are now able to feed the ConvergenceTester with new elements

}

template <class T>
void ConvergenceTester<T>::setPrecision(double epsilon){
  
  this->epsilon = epsilon;

}

#endif