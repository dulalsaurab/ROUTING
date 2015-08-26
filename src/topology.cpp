/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "topology.hpp"

#include "hr-calculator.hpp"
#include "ls-calculator.hpp"

void
Topology::build()
{
  std::cout << "Building topology..." << std::endl;

  // Assign neighbors
  for (const Link& link : m_links) {
    NodeMap::iterator srcIt = m_nodes.find(link.getSrc());
    NodeMap::iterator dstIt = m_nodes.find(link.getDst());

    Node& src = srcIt->second;
    Node& dst = dstIt->second;

    src.addNeighbor(dst);
    dst.addNeighbor(src);
  }

  std::cout << "...Done" << std::endl;

  // Calculate HR routing tables for each node
  HyperbolicRoutingCalculator hrCalculator;
  LinkStateRoutingCalculator lsCalculator;

  std::cout << "Calculating paths..." << std::endl;

  for (auto& pair : m_nodes) {
    hrCalculator.calculatePaths(*this, pair.second);
    lsCalculator.calculatePaths(*this, pair.second);
  }

  std::cout << "...Done" << std::endl;
}