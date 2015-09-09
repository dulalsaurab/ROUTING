/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "nexthop-difference-calculator.hpp"

#include <map>
#include <cmath>

int
computeDifferenceSum(int lhsIndex, int rhsIndex)
{
  return std::abs(double(lhsIndex - rhsIndex));
}

int
computeDifferenceSquare(int lhsIndex, int rhsIndex)
{
  return std::abs(double(lhsIndex - rhsIndex));
}

int
NextHopDifferenceCalculator::getDifference(const NextHopSet& lhs, const NextHopSet& rhs)
{
  std::map<std::string /* next hop name */, int /* index */> lookup;

  int lhsIndex = 0;
  for (const NextHop& hop : lhs) {
    lookup[hop.face] = lhsIndex++;
  }

  int rhsIndex = 0;
  int difference = 0;

  for (const NextHop& hop : rhs) {
    difference += computeDifferenceSum(lookup[hop.face], rhsIndex);
    ++rhsIndex;
  }

  return difference;
}