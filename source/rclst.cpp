#include <iostream>
#include <boost/filesystem.hpp>
#include "utility.h"
// #include <dlib/svm_threaded.h>


std::pair<size_t,std::string> checkArg(int argc,char** argv);

int main(int argc, char *argv[]) 
{
  try
  {
    auto args = checkArg(argc,argv);
    auto num = args.first;
    auto name = args.second;
    
    kcentroid<linear_kernel_t> kc(std::move(linear_kernel_t()));
    kkmeans<linear_kernel_t> test(kc);

    std::vector<sample_type> samples;
    std::vector<full_sample_type> full_samples;
    std::vector<sample_type> initial_centers;

    // full_sample_type f;
    std::string str;

    // get data from string
    while(std::getline(std::cin, str))
    {
      auto f = split_data(str);
      full_samples.push_back(f);
      samples.push_back(normolize::convert_to_sample(f));
    }

    //get class for samples for nex classificator train
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

    // using poly_kernel = polynomial_kernel<sample_type>;
    // using rbf_kernel = radial_basis_kernel<sample_type>;
    krr_trainer<rbf_kernel> rbf_trainer;
    svm_nu_trainer<poly_kernel> poly_trainer;
    
    poly_trainer.set_kernel(poly_kernel(0.1, 1, 2));
    // poly_trainer.set_kernel(poly_kernel());
    rbf_trainer.set_kernel(rbf_kernel(0.1));
    // rbf_trainer.set_kernel(rbf_kernel());
    trainer.set_trainer(rbf_trainer);
    trainer.set_trainer(poly_trainer, 1, 2);

    std::cout <<"START TRAIN multiclass_classificator...\n";
    one_vs_one_decision_function<ovo_trainer,decision_function<poly_kernel>,decision_function<rbf_kernel> > df = trainer.train(samples, labels);

    if(labels.size() != full_samples.size()) std::cout << "ERROR SIZE label- " << labels.size() << "full_s- " << full_samples.size() << std::endl;
    std::map<size_t,size_t> label_count;
    for(size_t i = 0;i < labels.size();++i)
    {
      size_t l = df(normolize::convert_to_sample(full_samples[i]));
      label_count[l] ++;
    }
    std::cout << "LABEL:\tCOUNT:\n";
    for(const auto& i : label_count)
    {
      std::cout << i.first << '\t' << i.second << std::endl;
    }



    // write classificator and data to files
    namespace fs = boost::filesystem;
    if(fs::exists(fs::path(name+".clas"))) fs::remove(fs::path(name+".clas"));
    serialize(name + ".clas") << df;
    std::cout << "SERIALIZE classificator done\n";

    std::map<size_t,std::vector<full_sample_type>> type_samples;

    for(const auto& s : full_samples)
    {
      type_samples[df(normolize::convert_to_sample(s))].push_back(s);
    }
    if(fs::exists(fs::path(name+".map"))) fs::remove(fs::path(name+".map"));
    serialize(name + ".map") << type_samples;
    std::cout << "SERIALIZE map-data done\n";

  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  return 0;
}



