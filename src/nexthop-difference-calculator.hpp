/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef NEXTHOP_DIFFERENCE_CALCULATOR_HPP
#define NEXTHOP_DIFFERENCE_CALCULATOR_HPP

#include "routing-table.hpp"

class NextHopDifferenceCalculator
{
public:
  static int
  getDifference(const NextHopSet& lhs, const NextHopSet& rhs);
};

#endif // NEXTHOP_DIFFERENCE_CALCULATOR_HPP
