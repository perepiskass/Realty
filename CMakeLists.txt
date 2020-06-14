# определяем минимальную версию для компиляции
cmake_minimum_required(VERSION 3.5)

# НАЗНАЧАЕМ ИМЯ ПРОЕКТУ И УСТАНАВЛИВАЕМ НОМЕР ВЕРСИИ------------------------
# если определена среда сборки Travis то выставляем номер сборки из Travis
if($ENV{TRAVIS_BUILD_NUMBER})
project(realty VERSION 1.2.$ENV{TRAVIS_BUILD_NUMBER})

# если среда не найдена то ставим номер сборки указанный ниже
else()
project(realty VERSION 1.1.5)
endif()

find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(dlib REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads)

# include_directories(${dlib_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})

# Генерируем файл version.h при сборке из файла с уловиями version.h.in
configure_file(${CMAKE_SOURCE_DIR}/include/version.h.in ${CMAKE_SOURCE_DIR}/include/version.h)


add_executable(rclst ${CMAKE_SOURCE_DIR}/source/rclst.cpp ${CMAKE_SOURCE_DIR}/source/arg_rclst.cpp)
add_executable(rclss ${CMAKE_SOURCE_DIR}/source/rclss.cpp ${CMAKE_SOURCE_DIR}/source/arg_rclss.cpp)

add_library(versionlib ${CMAKE_SOURCE_DIR}/source/version_lib.cpp)
add_executable(gtest  ${CMAKE_SOURCE_DIR}/source/gtest.cpp)


set_target_properties(rclst rclss versionlib gtest PROPERTIES
CXX_STANDARD 17
CXX_STANDARD_REQUIRED ON
)

target_include_directories(rclst
    PRIVATE "${CMAKE_SOURCE_DIR}/include" ${Boost_INCLUDE_DIRS}
)
target_include_directories(rclss
    PRIVATE "${CMAKE_SOURCE_DIR}/include" ${Boost_INCLUDE_DIRS}
)

target_include_directories(versionlib
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
)
target_include_directories(gtest
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
)


target_link_libraries(rclst PRIVATE
${Boost_LIBRARIES}
dlib::dlib
versionlib
)
target_link_libraries(rclss PRIVATE
${Boost_LIBRARIES}
dlib::dlib
versionlib
)
target_link_libraries(gtest
${GTEST_LIBRARIES}
${CMAKE_THREAD_LIBS_INIT}
dlib::dlib
versionlib
)

# СВОЙСТВА ДЛЯ КОМПИЛЯТОРОВ (ЛЮБЫЕ ПРЕДУПРЕЖДЕНИЯ ТРАКТОВАТЬ КАК ОШИБКИ)
if (MSVC)
target_compile_options(rclst PRIVATE
        /W4
)
target_compile_options(rclss PRIVATE
/W4
)

target_compile_options(versionlib PRIVATE
        /W4
)
target_compile_options(gtest PRIVATE
        /W4
)
else ()
    target_compile_options(rclst PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(rclss PRIVATE
    -Wall -Wextra -pedantic -Werror
    )

    target_compile_options(versionlib PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(gtest PRIVATE
            -Wall -Wextra -pedantic -Werror
    )
endif()

# ----------------------------------------------------------------------

install(TARGETS rclst rclss RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT sise07@gmail.com)

include(CPack)

enable_testing()

add_test(version_test_case ${CMAKE_PROJECT_NAME})