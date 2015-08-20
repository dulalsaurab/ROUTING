/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "topology-loader.hpp"

#include <iostream>

static const int ERROR_ARGS = 1;
static const int ERROR_LOAD = 2;

int
main(int argc, char* argv[])
{
  const std::string programName = argv[0];

  if (argc != 2) {
    std::cout << "Usage: " << programName << " TOPOLOGY" << std::endl;
    exit(ERROR_ARGS);
  }

  const std::string filename = argv[1];

  TopologyLoader loader;

  if (loader.load(filename)) {

  }
  else {
    exit(ERROR_LOAD);
  }
}