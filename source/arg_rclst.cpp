#include "version_lib.h"
#include <iostream>
#include <string>
#include <utility>

/**
 * @brief Функция для проверки введенных аргументов программы колличества классов и имя файла для хранения классификатора.
 */
auto checkArg(int argc,char** argv)
{
    if ( argc > 1)
    {
        std::string version = "-version";
        if (argv[1] == version)
        {
            std::cout << "version: " << version_major()<< '.'<< version_minor() << '.' << version_patch() << std::endl;
            exit(0);
        }
        else
        {
            std::pair<size_t,std::string> result;
            result.first = atoi(argv[1]);
            result.second = argv[2];
            if (result.first > 0 && result.second.find('.')==std::string::npos)
            {
                return result;
            } 
            else  
            {
                std::cout << "Enter correct num class or classificator name (without sufficks)"<< std::endl;
                exit(1);
            }
        }
    }
    else 
    {
      std::cout << "Usage: rclst <num class> <name classificator> or -version" << std::endl;
      exit(1);
    }
    
}