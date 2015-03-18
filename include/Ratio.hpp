#ifndef RATIO_H
#define RATIO_H

#include <random>
#include <chrono>
#include <memory>
#include <iostream>

class Ratio{ //interface for random ratios

protected:  
  std::mt19937 randomGenerator;

public:
  Ratio();
  virtual ~Ratio() = default;
  virtual std::unique_ptr<Ratio> clone() const =0;//for polymorphism; the new object needs to be movable so no 'const'
  virtual double getRealisation()=0;
  
};

#endif