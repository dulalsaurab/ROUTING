/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef ROUTING_CALCULATOR_HPP
#define ROUTING_CALCULATOR_HPP

#include "routing-table.hpp"

#include <string>

class Node;
class Topology;

class RoutingCalculator
{
public:
  virtual void
  calculatePaths(Topology& topo, Node& thisNode) = 0;

protected:
  void
  addNextHop(const std::string& face,
             const std::string& dst,
             double cost,
             RoutingTable& routingTable)
  {
    NextHop hop(face, cost);
    routingTable.addNextHop(dst, hop);
  }

};

#endif // ROUTING_CALCULATOR_HPP
