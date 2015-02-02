#include "spectrum.hpp"
#include "ConvergenceTester.hpp"
#include "SampleSlope.hpp"

void CovarianceLi(const path& database, const char* outname, const double epsilon = 1e-5, const double magnetism_error = 0.01){
  
  const vector<double> rbLi = {0.297, 0.158, 0.015, 0.011, 0.027};
  const vector<double> rbLi_e = {0.03, 0.03 , 0.005, 0.003, 0.002};
  const vector<double> r_1181 = {0.47, 0.28, 0.12, 0.11, 0.02, 0.01};
  const vector<double> r_1181_e = {0.07, 0.06, 0.08, 0.06, 0.01, 0.01};//need to match the ordering of std::sort on boost::path
  
  const vector<double> rebin_edges = {0, 14};
  const vector<double> rebin_widths = {0.1};
   
  spectrum spe_243(database, "_243_");
  spectrum spe_278(database, "_278_");
  spectrum spe_794(database, "_794_");
  spectrum spe_1128(database, "_1128_");
  spectrum spe_1181(database, "_1181_", r_1181, true, r_1181_e);//the errors will be set to zero by default
  
  const vector<spectrum*> branches ({&spe_243, &spe_278, &spe_794, &spe_1128, &spe_1181});
  spectrum spe_tot(branches, rbLi, true, rbLi_e);
  
  Sample lithium(spe_tot.GetResultingSpectrum());
  ConvergenceTester tester(epsilon);//create a convergence tester at two levels first
  
  while (!tester.converges()){
    
    spe_243.NewEvent();
    spe_278.NewEvent();
    spe_794.NewEvent();
    spe_1128.NewEvent();
    spe_1181.NewEvent();
    spe_tot.UpdateFromSpectra(branches);
    
    lithium.Update(spe_tot.GetResultingSpectrum());
    tester.feed(lithium);
    
  }
  
  SampleSlope magnetism(lithium.GetMeanSpectrum(rebin_edges.front(), rebin_edges.back()), magnetism_error);//apply an error whose energy dependency is 'magnetism_error' times the BinCenter(k) to the bin contents of the mean lithium spectrum
  cout<<magnetism;
  
  lithium += magnetism;
  cout<<lithium;
  lithium.SaveToRoot(outname, rebin_edges, rebin_widths);//save the mean_spectrum and the var matrix, use the  edges and the widths specified to create a bining for the mean_spectrum
  
}

void CovarianceHe(const path& database, const char* outname, const double epsilon = 1e-5, const double magnetism_error = 0.01){
  
  const vector<double> rbHe = {0.08, 0.08, 0.009};
  const vector<double> rbHe_e = {0.04, 0.04, 0.001};
  
  const vector<double> rebin_edges = {0, 14};
  const vector<double> rebin_widths = {0.1};
  
  spectrum spe_321(database, "_321_");
  spectrum spe_54(database, "_54_");
  spectrum spe_967(database, "_967_");

  const vector<spectrum*> branches ({&spe_321, &spe_54, &spe_967});
  spectrum spe_tot(branches, rbHe, true, rbHe_e);
  
  Sample helium(spe_tot.GetResultingSpectrum());
  ConvergenceTester tester(epsilon);
  
  while (!tester.converges()){
    
    spe_321.NewEvent();
    spe_54.NewEvent();
    spe_967.NewEvent();
    spe_tot.UpdateFromSpectra(branches);
    
    helium.Update(spe_tot.GetResultingSpectrum());
    tester.feed(helium);
    
  }
  
  SampleSlope magnetism(helium.GetMeanSpectrum(rebin_edges.front(), rebin_edges.back()), magnetism_error);//apply an error whose energy dependency is 'magnetism_error' times the BinCenter(k) to the bin contents of the mean helium spectrum
  cout<<magnetism;
  
  helium += magnetism;
  cout<<helium;
  helium.SaveToRoot(outname, rebin_edges, rebin_widths);//save the mean_spectrum and the var matrix, use the  edges and the widths specified to create a bining for the mean_spectrum
 
}

int main (int argc, char* argv[]){
  
  if (argc == 2 && is_directory(path (argv[1]))) CovarianceLi(path(argv[1]), "correlation.root");
  else if (argc == 3 && is_directory(path (argv[1]))) CovarianceLi(path(argv[1]), argv[2]);
  else if(argc == 4 && is_directory(path (argv[1]))) CovarianceLi(path(argv[1]), argv[2], stod(argv[3]));
  else if(argc == 5 && is_directory(path (argv[1]))) CovarianceLi(path(argv[1]), argv[2], stod(argv[3]), stod(argv[4]));
  else cout<<"You must provide the path to the folder where the  events are stored and an outfile name to store the mean spectrum"<<endl;
  
  return 0;
  
}


