#include "ConvergenceTester.hpp"

ConvergenceTester::ConvergenceTester(double espilon, unsigned int dequeMaxSize):oldVar(0),epsilon(espilon){
  
  setMaxRange(dequeMaxSize);

}

void ConvergenceTester::feed(const Eigen::MatrixXd& Var){
  
  if(Var.size() == oldVar.front().size() || oldVar.empty()){
    
    if(oldVar.size() < dequeMaxSize){
      
      oldVar.push_back(Var);
      
    }
    else{
      
      oldVar.pop_front();
      oldVar.push_back(Var);
      
    }
    
  }

}

void ConvergenceTester::feed(const Sample& sample){
  
  feed(sample.GetVar());

}

double ConvergenceTester::getPrecision() const{
  
  return epsilon;

}

unsigned int ConvergenceTester::getMaxRange() const{
  
  return dequeMaxSize;

}

bool ConvergenceTester::converges() const{

  if(oldVar.size() != dequeMaxSize) return false; //if the deque is not full yet (or even empty), it doesn't make sense to say it converges
  else{
    
    for (auto itVar = oldVar.begin()+1; itVar !=oldVar.end(); ++itVar)
      if (!(oldVar.front() - *itVar).isMuchSmallerThan(oldVar.front(), epsilon)) return false;
    
    return true;
    
  }
  
}

void ConvergenceTester::setMaxRange(unsigned int newDequeMaxSize){
  
  if(newDequeMaxSize > 0){
  
    if(newDequeMaxSize < dequeMaxSize) oldVar.resize(newDequeMaxSize);
    else dequeMaxSize = newDequeMaxSize; // we are now able to feed the ConvergenceTester with new elements
    
  }

}

void ConvergenceTester::setPrecision(const double epsilon){
  
  this->epsilon = epsilon;

}


