#include "SampleSlope.hpp"

ostream& operator<<(ostream& output, const SampleSlope& corr){//for input masses in MeV sets the file into GeV

  output<<"COV = "<<"\n"<<corr.GetVar()<<"\n"<<"\n"
  <<"Relative errors = "<<"\n"<<corr.GetRelErrors()<<"\n"<<"\n";
  return output;
  
}

SampleSlope::SampleSlope(const TH1D& h, const double slope):Sample(h),slope(slope),binErrors(h.GetNbinsX()){
  
  FillBinErrors(h);
  FillVarFromErrors();
  
}

void SampleSlope::FillBinErrors(const TH1D& h){
  
  for(unsigned k = 0; k<binErrors.size(); ++k) binErrors(k) = slope*h.GetBinCenter(k+1)*h.GetBinContent(k+1);
  
}

void SampleSlope::FillVarFromErrors(){

  var = binErrors*binErrors.transpose();

}