/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "hr-calculator.hpp"

#include "node.hpp"
#include "routing-table.hpp"
#include "topology.hpp"

#include <boost/math/constants/constants.hpp>

const double HyperbolicRoutingCalculator::MATH_PI = boost::math::constants::pi<double>();

const double HyperbolicRoutingCalculator::UNKNOWN_DISTANCE = -1.0;
const double HyperbolicRoutingCalculator::UNKNOWN_RADIUS   = -1.0;

const int32_t HyperbolicRoutingCalculator::ROUTER_NOT_FOUND = -1.0;

void
HyperbolicRoutingCalculator::calculatePaths(Topology& topo, Node& thisNode)
{
  // Iterate over directly connected neighbors
  for (const Node* adj : thisNode.getNeighbors()) {

    std::string srcRouterName = adj->getName();

    // Don't calculate nexthops for this router to other routers
    if (srcRouterName == thisNode.getName()) {
      continue;
    }

    // Install nexthops for this router to the neighbor; direct neighbors have a 0 cost link
    addNextHop(srcRouterName, srcRouterName, 0, thisNode.getHyperbolicRoutingTable());

    // Get hyperbolic distance from direct neighbor to every other router
    for (const auto& pair : topo.getNodes()) {
      const Node& dst = pair.second;

      // Don't calculate nexthops to this router or from a router to itself
      if (dst.getName() != thisNode.getName() && dst.getName() != adj->getName()) {

        std::string dstRouterName = dst.getName();

        double distance = getHyperbolicDistance(topo, *adj, dst);

        // Could not compute distance
        if (distance == UNKNOWN_DISTANCE) {
          std::cout << "Could not calculate hyperbolic distance from "
                    << srcRouterName << " to " << dstRouterName << std::endl;
          continue;
        }

        addNextHop(adj->getName(), dstRouterName, distance, thisNode.getHyperbolicRoutingTable());
      }
    }
  }
}

double
HyperbolicRoutingCalculator::getHyperbolicDistance(Topology& topo, const Node& src, const Node& dst)
{
  //std::cout << "Calculating hyperbolic distance from " << src.getName()
  //          << " to " << dst.getName() << std::endl;

  double distance = UNKNOWN_DISTANCE;

  double srcTheta = src.getAngle();
  double dstTheta = dst.getAngle();

  double diffTheta = fabs(srcTheta - dstTheta);

  if (diffTheta > MATH_PI) {
    diffTheta = 2 * MATH_PI - diffTheta;
  }

  double srcRadius = src.getRadius();
  double dstRadius = dst.getRadius();

  if (srcRadius == UNKNOWN_RADIUS && dstRadius == UNKNOWN_RADIUS) {
    return UNKNOWN_DISTANCE;
  }

  if (diffTheta == 0) {
    distance = fabs(srcRadius - dstRadius);
  }
  else {
    distance = acosh((cosh(srcRadius) * cosh(dstRadius)) -
                     (sinh(srcRadius) * sinh(dstRadius) * cos(diffTheta)));
  }

  //std::cout << "Distance from " << src.getName() << " to " << dst.getName()
  //          << " is " << distance << std::endl;

  return distance;
}
