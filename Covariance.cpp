#include <map>
#include "FileReader.hpp"
#include "VarianceEstimator.hpp"
#include "CovarianceEstimator.hpp"
#include "UnstableState.hpp"
#include "FinalState.hpp"
#include "GaussianRatio.hpp"

using US = UnstableState<Hist>;
using FS = FinalState<Hist>;
using GR = GaussianRatio;

void saveCovarianceHe(const boost::filesystem::path& database, double epsilon = 1e-4, unsigned cauchyNumber = 3){
  
  std::map<std::string, Hist> spectra;
  
  spectra["s321_7Li"] = FileReader::read(database/"he_321_7Li_gaus.root");
  spectra["s321_7Li048"] = FileReader::read(database/"he_321_7Li048_gaus.root");
  spectra["s54_7Li"] = FileReader::read(database/"he_54_7Li_gaus.root");
  spectra["s54_7Li048"] = FileReader::read(database/"he_54_7Li048_gaus.root");
  spectra["s54_atn"] = FileReader::read(database/"he_54_atn_gaus.root");
  spectra["s967_7Li"] = FileReader::read(database/"he_967_7Li_gaus.root");
  spectra["s967_7Li048"] = FileReader::read(database/"he_967_7Li048_gaus.root");
  spectra["s967_atn"] = FileReader::read(database/"he_967_atn_gaus.root");
  spectra["s967_5He"] = FileReader::read(database/"he_967_5He_gaus.root");
  
  for(auto& pair : spectra) normalise(pair.second);//normalise all the spectra to one
  
  US helium (
     {new US(
	{new FS(spectra.at("s321_7Li")),
	 new FS(spectra.at("s321_7Li048"))}
      ),
      new US(
	{new FS(spectra.at("s54_7Li")),
	 new FS(spectra.at("s54_7Li048")),
	 new FS(spectra.at("s54_atn"))}
      ),
      new US(
	{new FS(spectra.at("s967_7Li")),
	 new FS(spectra.at("s967_7Li048")),
	 new FS(spectra.at("s967_atn")),
	 new FS(spectra.at("s967_5He"))}
      )},
     {new GR(8, 4),
      new GR(8, 4),
      new GR(0.9, 0.1)}
  );
  
  std::cout<<"Processing decay tree:\n"<<helium<<std::endl;
  VarianceEstimator<US> varianceEstimator(helium);
  varianceEstimator.estimate(epsilon, cauchyNumber);
  std::cout<<varianceEstimator<<std::endl;
  
}

int main (int argc, char* argv[]){
  
  
  if(argc > 1){
    
    std::string databasePath = argv[1];
    
    if(boost::filesystem::is_directory(databasePath)){

      if(argc == 2) saveCovarianceHe(databasePath);
      else if(argc == 3) saveCovarianceHe(databasePath, std::stod(argv[2]));
      else if(argc == 4) saveCovarianceHe(databasePath, std::stod(argv[2]), std::stol(argv[3]));
      
    }
    
    
  }
  
  return 0;
  
}


