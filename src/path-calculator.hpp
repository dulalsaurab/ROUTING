/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef PATH_CALCULATOR_HPP
#define PATH_CALCULATOR_HPP

#include <string>

class Node;
class Path;
class Topology;

class PathCalculator
{
public:
  Path
  getHyperbolicPath(const Topology& topo, const Node& src, const Node& dst);

  Path
  getLinkStatePath(const Topology& topo, const Node& src, const Node& dst);

private:
  Path
  getPath(const Topology& topo, const Node& src, const Node& dst, bool isHyperbolic);

public:
  static const std::string NO_PATH_DISCONNECTED;
  static const std::string NO_PATH_LOOP;
};

#endif // PATH_CALCULATOR_HPP
