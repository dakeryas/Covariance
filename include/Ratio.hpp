#ifndef RATIO_H
#define RATIO_H

#include <random>
#include <memory>
#include <ostream>
#include <iomanip>

class Ratio{ //interface for random ratios
  
public:
  virtual ~Ratio() = default;
  virtual std::unique_ptr<Ratio> clone() const =0;//for polymorphism; the new object needs to be movable so no 'const'
  virtual double getRealisation(std::mt19937& randomGenerator) =0;
  virtual std::ostream& print(std::ostream& output) const =0;
  
};

std::ostream& operator<<(std::ostream& output, const Ratio& ratio);

#endif