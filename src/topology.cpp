/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "topology.hpp"

void
Topology::build()
{
  // Assign neighbors
  for (const Link& link : m_links) {
    NodeMap::iterator srcIt = m_nodes.find(link.getSrc());
    NodeMap::iterator dstIt = m_nodes.find(link.getDst());

    Node& src = srcIt->second;
    Node& dst = dstIt->second;

    src.addNeighbor(dst);
    dst.addNeighbor(src);
  }
}