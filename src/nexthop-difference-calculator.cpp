/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "nexthop-difference-calculator.hpp"

#include <map>
#include <cmath>

double
computeDifferenceSum(const NextHopSet& lhs, const NextHopSet& rhs)
{
  std::map<std::string /* next hop name */, int /* index */> lookup;

  int lhsIndex = 0;
  for (const NextHop& hop : lhs) {
    lookup[hop.face] = lhsIndex++;
  }

  int rhsIndex = 0;
  double difference = 0;

  for (const NextHop& hop : rhs) {
    difference += std::abs(double(lookup[hop.face] - rhsIndex));
    ++rhsIndex;
  }

  return difference;
}

double
computeDifferenceSquare(const NextHopSet& lhs, const NextHopSet& rhs)
{
  std::map<std::string /* next hop name */, int /* index */> lookup;

  int lhsIndex = 0;
  for (const NextHop& hop : lhs) {
    lookup[hop.face] = lhsIndex++;
  }

  int rhsIndex = 0;
  double difference = 0;

  for (const NextHop& hop : rhs) {
    difference += std::pow((double(lookup[hop.face] - rhsIndex)), 2);
    ++rhsIndex;
  }

  return std::sqrt(difference);
}

double
NextHopDifferenceCalculator::getDifference(const NextHopSet& lhs, const NextHopSet& rhs)
{
  return computeDifferenceSquare(lhs, rhs);
}