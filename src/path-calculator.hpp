/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef PATH_CALCULATOR_HPP
#define PATH_CALCULATOR_HPP

#include <string>

class Node;
class Topology;

class PathCalculator
{
public:
  std::string
  getPath(Topology& topo, const Node& src, const Node& dst);

public:
  static const std::string NO_PATH_DISCONNECTED;
  static const std::string NO_PATH_LOOP;
};

#endif // PATH_CALCULATOR_HPP
