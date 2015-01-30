#ifndef SAMPLE_SLOPE_H
#define SAMPLE_SLOPE_H

#include "Sample.hpp"

class SampleSlope: public Sample{
  
  double slope;
  VectorXd binErrors;
  void FillBinErrors(const TH1D& h);
  void FillVarFromErrors();
  
public:
  SampleSlope(const TH1D& h, const double slope);
  
};

ostream& operator<<(ostream& output, const SampleSlope& corr);

#endif