#pragma once
#include <map>
#include <type_traits>
#include <regex>

#include <dlib/clustering.h>
#include <dlib/rand.h>
#include <dlib/svm_threaded.h>

#include <ostream>



using namespace dlib;

using sample_type = matrix<double,7,1>;
using full_sample_type = matrix<double,8,1>;

using ovo_trainer = one_vs_one_trainer<any_trainer<sample_type> >;
using poly_kernel = polynomial_kernel<sample_type>;
using rbf_kernel = radial_basis_kernel<sample_type>;
using linear_kernel_t = linear_kernel<sample_type>; /// The most basic kernel is the linear_kernel which is simply a normal dot product.

namespace normolize
{

    struct max
    {
        static constexpr double longitude = 180;
        static constexpr double latitude = 90;
        static constexpr double room_count = 10;
        static constexpr double price = 100'000'000;
        static constexpr double area = 1000;
        static constexpr double kitchen = 100;
    };

    struct min
    {
        static constexpr double longitude = -180;
        static constexpr double latitude = -90;
        static constexpr double room_count = 1;
        static constexpr double price = 0;
        static constexpr double area = 1;
        static constexpr double kitchen = 0;
        static constexpr double floor = 1;
    };

    double replace_missed_data(size_t i,double val)
    {
        switch (i)
        {
        case 2:
            if(!val) val = min::room_count; // кол-во комнат
            break;
        case 4:
            if(!val) val = min::area; // общая площадь
            break;
        case 5:
            if(!val) val = min::kitchen; // площадь кухни
            break;
        case 6:
            if(!val) val = min::floor; // этажей
            break;
        case 7:
            if(!val) val = min::floor; // этажность
            break;
        default:
            break;
        }
        return val;
    }

    double norm (const double min,const double max,double val)
    {
        if(val < min) val = min;
        else if (val > max) val = max;
        return double((val - min)/(max - min));
    }

    sample_type convert_to_sample (const full_sample_type& f)
    {
        sample_type m;
        for(std::size_t i = 0;i<6;++i)
        {
            if(i == 0) m(i)= norm(min::longitude,max::longitude,f(i));
            else if(i == 1) m(i)= norm(min::latitude,max::latitude,f(i));
            else if(i == 2) m(i)= norm(min::room_count,max::room_count,f(i));
            else if(i == 3) m(i)= norm(min::price,max::price,f(i));
            else if(i == 4) m(i)= norm(min::area,max::area,f(i));
            else if(i == 5) m(i)= norm(min::kitchen,max::kitchen,f(i));
        }
        if(f(6) == f(7) || f(6) == 1) m(6) = 0;
        else m(6) = 1;
        return m;
    }
}


full_sample_type split_data(const std::string& str)
{
    full_sample_type f;
    std::size_t pos = 0;
    auto delim = str.find(';');
    for(size_t i = 0;i < 7; ++i)
    {
        auto tmp = std::atof(str.substr(pos, delim++).c_str());
        f(i) = normolize::replace_missed_data(i,tmp);
        pos = delim;
        delim = str.find(';',pos);
    }
    f(7) = normolize::replace_missed_data(7, std::atof(str.substr(pos,str.length()).c_str()));

    return f;
}

double get_distance(const sample_type& lv,const sample_type& rv)
{
    double tmp = 0;
    auto lv_first = lv.begin();
    auto rv_first = rv.begin();
    while(lv_first != lv.end())
    {
        double dist = (*lv_first++) -(*rv_first++);
        tmp += dist*dist;
    }
    return tmp > 0 ? sqrt(tmp) : 0;
}

template<typename ST>
typename std::enable_if<std::is_same<ST,full_sample_type>::value || std::is_same<ST,sample_type>::value,std::string>::type
convert_to_string(const ST& sample)
{
    std::string str;
    for(const auto& s : sample)
    {
        str += std::to_string(s) + ' ';
    }
    return str;
}

void provide_result (const full_sample_type& f,const std::vector<full_sample_type>& full_t)
{
    std::multimap<double,full_sample_type> dis_fsample;
    auto sample_quary = normolize::convert_to_sample(f);
    
    for(const auto& f_sample : full_t)
    {
        double dist = get_distance(sample_quary,normolize::convert_to_sample(f_sample));
        dis_fsample.emplace(std::make_pair(dist,f_sample));
    }
    std::ofstream fs(convert_to_string(f)+".result",std::ios_base::out | std::ios_base::trunc);
    if(!fs.is_open()) std::cout << "ERR open file for result\n";

    auto query = convert_to_string(f);
    fs << "QUERY: ";
    std::cout << "QUERY: ";
    fs << query << std::endl;
    std::cout << query << std::endl;

    fs << "RESULT: " << std::endl;
    std::cout << "RESULT: " << std::endl;
    for(const auto& sample : dis_fsample)
    {
        std::string res = "dist - " + std::to_string(sample.first)+ '\t' + convert_to_string(sample.second)+'\n';
        std::cout << res;
        fs << res;
    }
    fs.close();
}
