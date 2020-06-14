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
            std::string result = argv[1];
            if (result.find('.')==std::string::npos)
            {
                return result;
            } 
            else  
            {
                std::cout << "Enter correct classificator name (without suffix)"<< std::endl;
                exit(1);
            }
        }
    }
    else 
    {
      std::cout << "Usage: rclsы <name classificator> or -version" << std::endl;
      exit(1);
    }
    
}