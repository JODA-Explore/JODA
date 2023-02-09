#!/bin/sh

#define parameters which are passed in.
NAME=$1


mkdir ${NAME}
mkdir ${NAME}/src
mkdir ${NAME}/include
mkdir ${NAME}/include/joda
mkdir ${NAME}/include/joda/${NAME}


#define the template.
(
cat  << EOF
project(JODA-Core-${NAME} CXX)

set(joda-core-${NAME}-SOURCES

        )

set(JODA-CORE-SOURCE-FILES \$\{JODA-CORE-SOURCE-FILES\} \$\{joda-core-${NAME}-SOURCES\} PARENT_SCOPE )

add_library(joda-core-${NAME}-lib INTERFACE )

target_include_directories(joda-core-${NAME}-lib
        INTERFACE ./include/
        )


#target_link_libraries(joda-core-${NAME}-lib PUBLIC )
#########################################################
#
# GLOG
#
#########################################################
target_link_libraries(joda-core-${NAME}-lib INTERFACE glog::glog)
EOF
) > ${NAME}/CMakeLists.txt
