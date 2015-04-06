#include <thread>
#include "boost/program_options.hpp"
#include "TreeParser.hpp"
#include "VarianceEstimator.hpp"
#include "CovarianceEstimator.hpp"
#include "GaussianRatio.hpp"

using US = UnstableState<Hist>;
using FS = FinalState<Hist>;
namespace bpo = boost::program_options;

void saveCovariance(const US& unstableState, double epsilon, unsigned cauchyNumber){
  
  VarianceEstimator<US> varianceEstimator(unstableState);
  varianceEstimator.estimate(epsilon, cauchyNumber);
  
  std::cout<<varianceEstimator<<std::endl;
  
}

void saveCovariance(const US& unstableState1, const US& unstableState2, double epsilon, unsigned cauchyNumber){
  
  VarianceEstimator<US> varianceEstimator1(unstableState1);
  VarianceEstimator<US> varianceEstimator2(unstableState2);
  CovarianceEstimator<US> covarianceEstimator(unstableState1, unstableState2);
  
  std::thread thread1(&VarianceEstimator<US>::estimate, &varianceEstimator1, epsilon, cauchyNumber);
  std::thread thread2(&VarianceEstimator<US>::estimate, &varianceEstimator2, epsilon, cauchyNumber);
  std::thread thread3(&CovarianceEstimator<US>::estimate, &covarianceEstimator, epsilon, cauchyNumber);
  thread1.join();
  thread2.join();
  thread3.join();
  
  std::cout<<varianceEstimator1<<"\n"<<varianceEstimator2<<"\n"<<covarianceEstimator<<std::endl;
  
}

void startCovariance(const std::vector<std::string>& xmlFiles, double epsilon, unsigned cauchyNumber, bool normaliseData){
  
  std::vector<std::unique_ptr<std::istream>> xmlStreams;//we need pointers for polymorphism (ifstream: public istream)
  for(const auto& xmlFile : xmlFiles) xmlStreams.emplace_back(new std::ifstream(xmlFile));
  
  TreeParser<Hist> treeParser;//the  data files parsed will store "Hist" 's
  std::vector<std::unique_ptr<US>> unstableStates;
  for(const auto& xmlStream : xmlStreams) unstableStates.emplace_back(treeParser.read(*xmlStream, normaliseData));//parse the xml files
  
  if(unstableStates.size() == 1) saveCovariance(*unstableStates.front(), epsilon, cauchyNumber);
  else if(unstableStates.size() == 2) saveCovariance(*unstableStates.front(), *unstableStates.back(), epsilon, cauchyNumber);
  
}

int main (int argc, char* argv[]){
  
  bpo::options_description optionDescription("Covariance Tool usage");
  optionDescription.add_options()
  ("help,h", "Display this help message")
  ("tree,t", bpo::value<std::vector<std::string>>(), "Decay trees to process in the form of XML files (1 or 2 files)")
  ("accuracy,a", bpo::value<double>()->default_value(1e-4) ,"Demanded relative accuracy for the covariance matrix")
  ("consecutive,c", bpo::value<unsigned>()->default_value(3),"Number of close consecutive matrices demanded")
  ("normalise,n", "Normalise to unit area the data found in the data files");

  bpo::positional_options_description positionalOptions;//to use arguments without "--"
  positionalOptions.add("tree", -1);
  
  bpo::variables_map arguments;
  bpo::store(bpo::command_line_parser(argc, argv).options(optionDescription).positional(positionalOptions).run(), arguments);
  bpo::notify(arguments);//the arguments are ready to be used
  
  if(arguments.count("help")) std::cout<<optionDescription<<std::endl;
  else if(arguments.count("tree")){
    
    std::vector<std::string> xmlFiles = arguments["tree"].as<std::vector<std::string>>();
    
    if(xmlFiles.size() != 1 && xmlFiles.size() != 2) std::cout<<"Error : wrong number of input files (1 or 2 needed)"<<std::endl;
    else{
      
      bool regularFiles = true;
      for (const auto& file : xmlFiles) if(!boost::filesystem::is_regular_file(file)){
	
	  std::cout<<"Error: '"<<file<<"' is not a regular file"<<std::endl;
	  regularFiles = false;
	  
      }
      
      if(regularFiles){
	
	if(arguments.count("normalise")) startCovariance(xmlFiles, arguments["accuracy"].as<double>(), arguments["consecutive"].as<unsigned>(), true);
	else startCovariance(xmlFiles, arguments["accuracy"].as<double>(), arguments["consecutive"].as<unsigned>(), false);;
	
      }
	
    }
    
  }
  else std::cout<<"No trees to process were passed (see help)"<<std::endl;
  
  return 0;
  
}


