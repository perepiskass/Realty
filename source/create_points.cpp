#include <dlib/rand.h>
#include <fstream>
#include <algorithm>


dlib::rand rnd;
using point = std::pair<double,double>;
using points = std::vector<point>;

void get_circle(points& samples,const long num, const double radius,const double offset_x = 0,const double offset_y = 0);
void get_starsky(points& samples,const long num);
void create_file(const points& samples,const std::string& name);

  int main()
{
    points samples;
    const long num = 50;

    get_circle(samples,num,5,25,-25);
    get_circle(samples,num,1,-25,25);
    get_circle(samples,num,3,-25,-25);
    get_circle(samples,num,7,25,25);

    std::random_shuffle(std::begin(samples),std::end(samples));
    create_file(samples,"kkmeans.txt");

    samples.clear();

    get_starsky(samples,200);
    create_file(samples,"starsky.txt");

    return 0;
}


void get_circle(points& samples,const long num, const double radius,const double offset_x,const double offset_y)
{
    point p;

    for (long i = 0; i < num; ++i)
    {
        double sign = 1;
        if (rnd.get_random_double() < 0.5)
        sign = -1;
        p.first = 2*radius*rnd.get_random_double()-radius;
        p.second = sign*sqrt(radius*radius - p.first*p.first);

        p.first += offset_x;
        p.second += offset_y;

        samples.push_back(p);
    }
}

void get_starsky(points& samples,const long num)
{
    point p;
    for(long i = 0; i < num ; ++i)
    {
        p.first = rnd.get_integer_in_range(-100,100);
        p.second = rnd.get_integer_in_range(-100,100);
        samples.push_back(p);
    }
}


void create_file(const points& samples,const std::string& name)
{
    std::ofstream out(name,std::ios::trunc);
    if(!out.is_open()) std::cout << "ERR open file test.txt\n";

    for(const auto& point : samples)
    {
        out << point.first << ';' << point.second << '\n';
    }

    out.close();

}