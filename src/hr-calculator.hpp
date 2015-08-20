/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef HR_CALCULATOR_HPP
#define HR_CALCULATOR_HPP

#include <string>

class Node;
class RoutingTable;
class Topology;

class HyperbolicRoutingCalculator
{
public:
  void
  calculatePaths(Topology& topo, Node& thisNode);

private:
  double
  getHyperbolicDistance(Topology& topo, const Node& src, const Node& dst);

  void
  addNextHop(const std::string& face,
             const std::string& dst,
             double cost,
             RoutingTable& routingTable);

private:
  static const double MATH_PI;
  static const double UNKNOWN_DISTANCE;
  static const double UNKNOWN_RADIUS;
  static const int32_t ROUTER_NOT_FOUND;
};

#endif // HR_CALCULATOR_HPP
