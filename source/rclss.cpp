#include <iostream>
#include <boost/filesystem.hpp>
#include "utility.h"


std::string checkArg(int argc,char** argv);

int main(int argc, char *argv[]) 
{
  try
  {
    auto name = checkArg(argc,argv);

    one_vs_one_decision_function<ovo_trainer,decision_function<poly_kernel>,decision_function<rbf_kernel> > df;
    std::map<size_t,std::vector<full_sample_type>> map_samples;

    deserialize(name + ".clas") >> df;
    deserialize(name + ".map") >> map_samples;

    std::string str;
    while(std::getline(std::cin, str))
    {
      auto f = split_data(str);
      auto label = df(normolize::convert_to_sample(f));
      std::cout << "Query is labe - " << label << std::endl;
      std::cout << "Result have - " << map_samples.at(label).size() << std::endl;
      provide_result(f,map_samples.at(label));
    }
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

return 0;
}