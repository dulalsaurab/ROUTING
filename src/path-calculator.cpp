/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "path-calculator.hpp"

#include "node.hpp"
#include "routing-table.hpp"
#include "topology.hpp"

#include <iostream>

const double PathCalculator::INFINITE_RTT = -1.0;

std::string
PathCalculator::getPath(const Topology& topo, const Node& src, const Node& dst)
{
  std::cout << "Calculating path from " << src.getName() << " to " << dst.getName() << std::endl;

  PathAndRttPair hr = getPath(topo, src, dst, true);
  PathAndRttPair ls = getPath(topo, src, dst, false);

  std::string output;
  output += "HR: " + hr.path;

  if (hr.rtt != INFINITE_RTT) {
    output += " (RTT: " + std::to_string(int(hr.rtt)) + ")";
  }

  output += "\n";

  output += "LS: " + ls.path;

  if (ls.rtt != INFINITE_RTT) {
    output += " (RTT: " + std::to_string(int(ls.rtt)) + ")";
  }

  output += "\n";

  output += "Stretch: " + getStretch(hr, ls) + "\n";

  return output;
}

PathCalculator::PathAndRttPair
PathCalculator::getPath(const Topology& topo, const Node& src, const Node& dst, bool isHyperbolic)
{
  std::string path;
  const Node* current = &src;
  const Node* inFace = &src;
  double rtt = 0;

  std::set<std::string> visitedNodes;

  while (current != nullptr) {

    // Check for loop
    if (visitedNodes.count(current->getName()) > 0) {
      path += current->getName() + " (LOOP!) ";
      return PathAndRttPair{path, INFINITE_RTT};
    }
    else {
      path += current->getName();
    }

    // Arrived at destination?
    if (current->getName() == dst.getName()) {
      return PathAndRttPair{path, 2*rtt};
    }

    path += " -> ";

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
      path += "No Nexthop";
      return PathAndRttPair{path, INFINITE_RTT};
    }
  }

  return PathAndRttPair{path, INFINITE_RTT};
}

std::string
PathCalculator::getStretch(const PathAndRttPair& hr, const PathAndRttPair& ls) const
{
  if (hr.rtt == INFINITE_RTT && ls.rtt != INFINITE_RTT) {
    return "infinity";
  }
  else if (hr.rtt != INFINITE_RTT && ls.rtt == INFINITE_RTT) {
    return "-infinity";
  }
  else {
    return std::to_string(hr.rtt/ls.rtt);
  }
}
