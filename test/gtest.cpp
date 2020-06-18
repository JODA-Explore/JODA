//
// Created by nico on 3/20/17.
//
#include <glog/logging.h>
#include "gtest/gtest.h"

int main(int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_minloglevel = 3;//Disable Info/Warning/Error Logging
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}