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

double UniformRatio::getRealisation(std::mt19937& randomGenerator){
  
  return distribution(randomGenerator);
  
}

std::ostream& UniformRatio::print(std::ostream& output) const{
  
  output<<"Lower bound = "<<std::setw(6)<<getLowerBound()<<" Upper bound = "<<std::setw(6)<<getUpperBound();
  return output;

}