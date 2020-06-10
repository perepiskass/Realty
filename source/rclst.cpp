#include <iostream>
#include <boost/filesystem.hpp>
#include "utility.h"
#include <dlib/svm_threaded.h>

std::pair<size_t,std::string> checkArg(int argc,char** argv);

int main(int argc, char *argv[]) 
{
  try
  {
    auto args = checkArg(argc,argv);
    auto num = args.first;
    auto name = args.second;
    
    using kernel_type = linear_kernel<sample_type>; /// The most basic kernel is the linear_kernel which is simply a normal dot product.

    kcentroid<kernel_type> kc(std::move(kernel_type()));
    kkmeans<kernel_type> test(kc);

    std::vector<sample_type> samples;
    std::vector<full_sample_type> full_samples;
    std::vector<sample_type> initial_centers;

    full_sample_type f;
    std::string str;

    // get data from string
    while(std::getline(std::cin, str))
    {
      std::size_t pos = 0;
      auto delim = str.find(';');
      for(size_t i = 0;i < 7; ++i)
      {
        f(i) = std::atof(str.substr(pos, delim++).c_str());
        pos = delim;
        delim = str.find(';',pos);
      }
      f(7) = std::atof(str.substr(pos,str.length()).c_str());

      full_samples.push_back(f);
      samples.push_back(convert_to_sample(f));
    }

    //get class for samples for nex classificator training
    test.set_number_of_centers(num);
    pick_initial_centers(num, initial_centers, samples, test.get_kernel());

    std::cout <<"START TRAIN claster_classificator...\n";
    if(samples.size())     test.train(samples,initial_centers);
    else throw std::length_error ("no data to sample");

    // train multiclass_classificator
    std::vector<double> labels;
    for(const auto& m : samples)
    {
      labels.push_back(test(m));
    }

    using ovo_trainer = one_vs_one_trainer<any_trainer<sample_type> >;
    ovo_trainer trainer;

    using poly_kernel = polynomial_kernel<sample_type>;
    using rbf_kernel = radial_basis_kernel<sample_type>;
    krr_trainer<rbf_kernel> rbf_trainer;
    svm_nu_trainer<poly_kernel> poly_trainer;
    
    poly_trainer.set_kernel(poly_kernel(0.1, 1, 2));
    rbf_trainer.set_kernel(rbf_kernel(0.1));
    trainer.set_trainer(rbf_trainer);
    trainer.set_trainer(poly_trainer, 1, 2);

    std::cout <<"START TRAIN multiclass_classificator...\n";
    one_vs_one_decision_function<ovo_trainer,decision_function<poly_kernel>,decision_function<rbf_kernel>> df = trainer.train(samples, labels);
    std::cout << "TEST multiclass_classificator\n";
    std::cout << "predicted label: "<< df(samples[0])  << ", true label: "<< labels[0] << std::endl;
    std::cout << "predicted label: "<< df(samples[90]) << ", true label: "<< labels[90] << std::endl;

    // write classificator and data to files
    namespace fs = boost::filesystem;
    if(fs::exists(fs::path(name+".clas"))) fs::remove(fs::path(name+".clas"));
    serialize(name + ".clas") << df;
    std::cout << "SERIALIZE classificator done\n";

    if(fs::exists(fs::path(name+".data"))) fs::remove(fs::path(name+".clas"));
    serialize(name + ".data") << full_samples;
    std::cout << "SERIALIZE data done\n";
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  return 0;
}



