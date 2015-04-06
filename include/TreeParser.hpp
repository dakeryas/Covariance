#ifndef TREEPARSER_H
#define TREEPARSER_H

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "UnstableState.hpp"
#include "GaussianRatio.hpp"
#include "FinalState.hpp"
#include "FileReader.hpp"

template <class T> 
class TreeParser{ //class to parse xml files containing decay trees and build an unstable state from the root state of the xml file

  boost::filesystem::path databasePath;
  bool normaliseData;//normalise the data found the in databasePath
  void parse(const boost::property_tree::ptree& tree, State<T>& state);//parse the children of tree and append them to state
  
public:
  TreeParser()= default;
  std::unique_ptr<UnstableState<T>> read(std::istream& stream);//read the tree defined in "stream" and normalise the data found if demanded
  
};

template <class T>
void TreeParser<T>::parse(const boost::property_tree::ptree& tree, State<T>& state){

  for(const auto& node : tree){
    
    auto ratio = node.second.get_optional<double>("<xmlattr>.ratio");
    auto error = node.second.get_optional<double>("<xmlattr>.error");

    if(node.first == "US"){
      
      if(ratio && error) state.addDaughter(new UnstableState<T>, new GaussianRatio(*ratio, *error));
      else state.addDaughter(new UnstableState<T>);//if we have no ratios, a default uniform ratio U(0,1) is built
      parse(node.second, *(state.getDaughters().back()));//if the state is unstable, it means we have to parse deeper into the decay tree add append to the newly added unstable state
      
    }
    else if(node.first == "FS"){
      
      auto spectrum = FileReader::read(databasePath/node.second.get<boost::filesystem::path>("<xmlattr>.datafile"));//retrieve the datafile field as boost::fs::path
      if(normaliseData) normalise(spectrum);
      if(ratio && error) state.addDaughter(new FinalState<T>(spectrum), new GaussianRatio(*ratio, *error));
      else state.addDaughter(new FinalState<T>(spectrum));//may have ratio and error attributes !
      
    }
    
  }

}

template <class T>
std::unique_ptr<UnstableState<T>> TreeParser<T>::read(std::istream& stream){
  
  boost::property_tree::ptree tree;
  boost::property_tree::read_xml(stream, tree);
  
  tree = tree.get_child("configuration");//enter the actual tree
  databasePath = tree.get_child("database").get<boost::filesystem::path>("<xmlattr>.path");//fill the databasePath
  std::cout<<"Using database: "<<databasePath<<std::endl;
  
  auto normaliseChild = tree.get_child_optional("normalise");
  if(normaliseChild) normaliseData = normaliseChild->get<bool>("<xmlattr>.true");
  else normaliseData = false;
  std::cout<<"Normalise the data read: "<<std::boolalpha<<normaliseData<< std::endl;
  
  std::unique_ptr<UnstableState<T>> state(new UnstableState<T>);
  parse(tree.get_child("US"), *state);//fill the state
  
  std::cout<<"Read tree:\n"<<*state<<std::endl;
  
  return state;
  
}


#endif