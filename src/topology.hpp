/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef TOPOLOGY_HPP
#define TOPOLOGY_HPP

#include "link.hpp"
#include "node.hpp"

#include <list>

class Topology
{
public:
  void
  add(const Node& node)
  {
    m_nodes.push_back(node);
  }

  void
  add(const Link& link)
  {
    m_links.push_back(link);
  }

  void
  print() const
  {
    for (const Node& node : m_nodes) {
      std::cout << "Node{ "
                << "name: " << node.getName()
                << ", angle: " << node.getAngle()
                << ", radius: " << node.getRadius()
                << "}" << std::endl;
    }

    for (const Link& link : m_links) {
      std::cout << "Link{ "
              << "src: " << link.getSrc()
              << ", dst: " << link.getDst()
              << ", delay: " << link.getDelay()
              << "}" << std::endl;
    }
  }

private:
  std::list<Node> m_nodes;
  std::list<Link> m_links;
};

#endif // TOPLOGY_HPP