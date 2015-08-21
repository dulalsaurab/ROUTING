/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "path-calculator.hpp"

#include "node.hpp"
#include "routing-table.hpp"
#include "topology.hpp"

#include <iostream>

std::string
PathCalculator::getPath(Topology& topo, const Node& src, const Node& dst)
{
  std::cout << "Calculating path from " << src.getName() << " to " << dst.getName() << std::endl;

  std::string path;
  const Node* current = &src;
  double rtt = 0;

  std::set<std::string> visitedNodes;

  while (current != nullptr) {

    // Check for loop
    if (visitedNodes.count(current->getName()) > 0) {
      path += current->getName() + " (LOOP!) ";
      return path;
    }
    else {
      path += current->getName();
    }

    // Arrived at destination?
    if (current->getName() == dst.getName()) {
      path += " (RTT: " + std::to_string(int(2*rtt)) + "ms)";
      return path;
    }

    path += " -> ";

    visitedNodes.insert(current->getName());

    const NextHop* hop = current->getHyperbolicRoutingTable().getBestRoute(dst.getName());

    if (hop != nullptr) {
      const Node* next = topo.getNode(hop->face);

      // Find link to add to RTT
      const Link* link = topo.findLink(current->getName(), next->getName());
      rtt += link->getDelay();

      current = next;
    }
    else {
      path += "No Nexthop";
      return path;
    }
  }

  return path;
}