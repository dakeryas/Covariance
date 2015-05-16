#include "Ratio.hpp"

std::ostream& operator<<(std::ostream& output, const Ratio& ratio){
  
  return ratio.print(output);
  
}
