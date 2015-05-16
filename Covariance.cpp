#include "boost/program_options.hpp"
#include "TMatrixD.h"
#include "TreeParser.hpp"
#include "VarianceEstimator.hpp"
#include "CorrelationEstimator.hpp"
#include "GaussianRatio.hpp"

using US = UnstableState<Hist>;
using FS = FinalState<Hist>;
namespace bpo = boost::program_options;

VarianceEstimator<US> getVariance(const US& unstableState, double epsilon, unsigned cauchyNumber){
  
  VarianceEstimator<US> varianceEstimator(unstableState);
  varianceEstimator.estimate(epsilon, cauchyNumber);
  return varianceEstimator;
  
}

CorrelationEstimator<US> getCorrelation(const US& unstableState1, const US& unstableState2, double epsilon, unsigned cauchyNumber){
  
  CorrelationEstimator<US> correlationEstimator(unstableState1, unstableState2);
  correlationEstimator.estimate(epsilon, cauchyNumber);
  return correlationEstimator;
  
}

void saveCovariance(const std::vector<std::string>& xmlFiles, const std::string& outFileName, double epsilon, unsigned cauchyNumber, unsigned verbose, double slope){
  
  std::vector<std::unique_ptr<std::istream>> xmlStreams;//we need pointers for polymorphism (ifstream: public istream)
  for(const auto& xmlFile : xmlFiles) xmlStreams.emplace_back(new std::ifstream(xmlFile));
  
  TreeParser<Hist> treeParser;//the  data files to parse point to "Hist" 's
  std::vector<std::unique_ptr<US>> unstableStates;
  for(const auto& xmlStream : xmlStreams) unstableStates.emplace_back(treeParser.read(*xmlStream, verbose));//parse the xml files
  
  if(unstableStates.size() == 1){
    
    auto varianceEstimator = getVariance(*unstableStates.front(), epsilon, cauchyNumber);
    if(slope > 0) varianceEstimator.addSlopeMatrix(slope);
    if(verbose > 2) std::cout<<varianceEstimator<<std::endl;
    
    TMatrixD covarianceMatrix(varianceEstimator.getCovarianceMatrix().rows(), varianceEstimator.getCovarianceMatrix().cols(), varianceEstimator.getCovarianceMatrix().data());
    auto corr = varianceEstimator.getCorrelationMatrix();
    TMatrixD correlationMatrix(corr.rows(), corr.cols(), corr.data());
    auto mean = unstableStates.front()->getTemplateRealisation();//use a dummy realisation to get the right edges
    mean.setBinContents(varianceEstimator.getMean());
    mean.setErrorsFrom(varianceEstimator.getCovarianceMatrix());
    
    TFile outfile(outFileName.c_str(), "recreate");
    mean.Write();
    covarianceMatrix.Write("Cov");
    correlationMatrix.Write("Corr");
    
  }
  else if(unstableStates.size() == 2){
    
    auto correlationEstimator = getCorrelation(*unstableStates.front(), *unstableStates.back(), epsilon, cauchyNumber);
    if(slope > 0) correlationEstimator.addSlopeMatrix(slope);
    if(verbose > 2) std::cout<<correlationEstimator<<std::endl;
    
    auto var1 = correlationEstimator.getVarianceEstimator1().getCovarianceMatrix();
    TMatrixD covarianceMatrix1(var1.rows(), var1.cols(), var1.data());
    auto corr1 = correlationEstimator.getVarianceEstimator1().getCorrelationMatrix();
    TMatrixD correlationMatrix1(corr1.rows(), corr1.cols(), corr1.data());
    auto mean1 = unstableStates.front()->getTemplateRealisation();//use a dummy realisation to get the right edges
    mean1.setBinContents(correlationEstimator.getMean1());
    mean1.setErrorsFrom(var1);

    auto var2 = correlationEstimator.getVarianceEstimator2().getCovarianceMatrix();
    TMatrixD covarianceMatrix2(var2.rows(), var2.cols(), var2.data());
    auto corr2 = correlationEstimator.getVarianceEstimator2().getCorrelationMatrix();
    TMatrixD correlationMatrix2(corr2.rows(), corr2.cols(), corr2.data());
    auto mean2 = unstableStates.back()->getTemplateRealisation();//use a dummy realisation to get the right edges
    mean2.setBinContents(correlationEstimator.getMean2());
    mean2.setErrorsFrom(var2);

    TMatrixD covarianceMatrix12(correlationEstimator.getCovarianceMatrix().rows(), correlationEstimator.getCovarianceMatrix().cols(), correlationEstimator.getCovarianceMatrix().transpose().data());
    auto corr12 = correlationEstimator.getCorrelationMatrix().transpose();
    TMatrixD correlationMatrix12(corr12.rows(), corr12.cols(), corr12.data());

    TFile outfile(outFileName.c_str(), "recreate");
    mean1.Write("Mean1");
    mean2.Write("Mean2");
    covarianceMatrix1.Write("Cov1");
    covarianceMatrix2.Write("Cov2");
    correlationMatrix1.Write("Corr1");
    correlationMatrix2.Write("Corr2");
    covarianceMatrix12.Write("Cov12");
    correlationMatrix12.Write("Corr12");
    
  }
  
  if(verbose > 0) std::cout<<"Results successfully saved in: \""<<outFileName<<"\"\n";
  
}

int main (int argc, char* argv[]){
  
  bpo::options_description optionDescription("Covariance Tool usage");
  optionDescription.add_options()
  ("help,h", "Display this help message")
  ("tree,t", bpo::value<std::vector<std::string>>(), "Decay trees to process in the form of XML files (1 or 2 files)")
  ("output,o", bpo::value<std::string>(), "Output file where to save the covariance matrices")
  ("accuracy,a", bpo::value<double>()->default_value(1e-4) ,"Demanded relative accuracy for the covariance matrix")
  ("consecutive,c", bpo::value<unsigned>()->default_value(3),"Number of close consecutive matrices demanded")
  ("slope,s", bpo::value<double>()->default_value(0) ,"Add a correlated slope matrix to the results")
  ("verbose,v", bpo::value<unsigned>()->default_value(2),"Display parsing at 1, complete parsing at 2, and display all results at 3");

  bpo::positional_options_description positionalOptions;//to use arguments without "--"
  positionalOptions.add("tree", -1);
  
  bpo::variables_map arguments;
  bpo::store(bpo::command_line_parser(argc, argv).options(optionDescription).positional(positionalOptions).run(), arguments);
  bpo::notify(arguments);//the arguments are ready to be used
  
  if(arguments.count("help")) std::cout<<optionDescription<<std::endl;
  else if(arguments.count("tree")){
    
    if(arguments.count("output")){
      
      std::vector<std::string> xmlFiles = arguments["tree"].as<std::vector<std::string>>();
      
      if(xmlFiles.size() != 1 && xmlFiles.size() != 2) std::cout<<"Error : wrong number of input files (1 or 2 needed)"<<std::endl;
      else{
	
	bool regularFiles = true;
	for (const auto& file : xmlFiles) if(!boost::filesystem::is_regular_file(file)){
	  
	    std::cout<<"Error: '"<<file<<"' is not a regular file"<<std::endl;
	    regularFiles = false;
	    
	}
	
	if(regularFiles) saveCovariance(xmlFiles,arguments["output"].as<std::string>(),arguments["accuracy"].as<double>(),arguments["consecutive"].as<unsigned>(),arguments["verbose"].as<unsigned>(),arguments["slope"].as<double>());
	  
      }
      
    }
    else std::cout<<"Output file missing (see help)"<<std::endl;
    
  }
  else std::cout<<"No trees to process were passed (see help)"<<std::endl;
  
  return 0;
  
}


