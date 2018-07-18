/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "path-calculator.hpp"

#include "node.hpp"
#include "path.hpp"
#include "routing-table.hpp"
#include "topology.hpp"

#include <iostream>

const double Path::INFINITE_RTT = -1.0;

Path
PathCalculator::getHyperbolicPath(const Topology& topo, const Node& src, const Node& dst)
{
  return getPath(topo, src, dst, true);
}

Path
PathCalculator::getLinkStatePath(const Topology& topo, const Node& src, const Node& dst)
{
  return getPath(topo, src, dst, false);
}

Path
PathCalculator::getPath(const Topology& topo, const Node& src, const Node& dst, bool isHyperbolic)
{
  Path path;
  const Node* current = &src;
  const Node* inFace = &src;
  double rtt = 0;

  std::set<std::string> visitedNodes;

  while (current != nullptr) {

    path.addNode(current->getName());

    // Check for loop
    if (visitedNodes.count(current->getName()) > 0) {
      path.setError(Path::ERROR_LOOP);
      return path;
    }

    // Arrived at destination?
    if (current->getName() == dst.getName()) {
      path.setRtt(2*rtt);
      return path;
    }

    visitedNodes.insert(current->getName());

    const RoutingTable* routingTable;

    if (isHyperbolic) {
      routingTable = &(current->getHyperbolicRoutingTable());
    }
    else {
      routingTable = &(current->getLinkStateRoutingTable());
    }

    const NextHop* hop = routingTable->getBestRoute(dst.getName(), inFace->getName());

    if (hop != nullptr) {
      const Node* next = topo.getNode(hop->face);

      // Find link to add to RTT
      const Link* link = topo.findLink(current->getName(), next->getName());
      rtt += link->getDelay();

      inFace = current;
      current = next;
    }
    else {
      path.setError(Path::ERROR_NO_NEXTHOP);
      return path;
    }
  }
  throw std::runtime_error("PathCalculator::getPath did not return a path before exiting loop");
}
