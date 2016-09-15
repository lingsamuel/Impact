cmake_minimum_required(VERSION 3.3)
project(AnotherC__)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
set(linq references/linq.cpp
        references/linq.h)
set(forColor tests/OverloadOperator.cpp
        )
add_executable(colorer ${forColor})
set(CjhkagsghCCC MagicCompare.cpp)
#set(Magic6 magic1.cpp)
add_executable(ifOperatorReload ifOperatorReload.cpp)
add_executable(BigNumber BigNumber.cpp)
add_executable(magic2 magic2.cpp)
add_executable(Assignment Assignment.cpp)
add_executable(MagicCompare MagicCompare.cpp)
add_executable(Lisp WriteCppLikeLisp.cpp)
add_executable(tuguan_regex TemplateExpand.cpp)
add_executable(test_template TemplateTest.cpp)
add_executable(V VorNotV.cpp)
add_executable(linq ${linq})
#add_executable(Magic6 ${Magic6})