#include "include/UniformRatio.hpp"

UniformRatio::UniformRatio(double lowerBound, double upperBound):distribution(lowerBound, upperBound){

}

std::unique_ptr<Ratio> UniformRatio::clone() const{
  
  return std::unique_ptr<Ratio>(new UniformRatio(*this));

}

double UniformRatio::getLowerBound() const{
  
  return distribution.a();

}

double UniformRatio::getUpperBound() const{
  
  return distribution.b();

}

double UniformRatio::getRealisation(){
  
  return distribution(randomGenerator);
  
}
