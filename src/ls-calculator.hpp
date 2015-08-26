/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef LS_CALCULATOR_HPP
#define LS_CALCULATOR_HPP

#include "routing-calculator.hpp"

class LinkStateRoutingCalculator : public RoutingCalculator
{
public:
  void
  calculatePaths(Topology& topo, Node& thisNode) override;
};

#endif // ROUTING_CALCULATOR_HPP
