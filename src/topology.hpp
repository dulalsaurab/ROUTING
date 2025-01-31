/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef TOPOLOGY_HPP
#define TOPOLOGY_HPP

#include "link.hpp"
#include "node.hpp"
#include <iostream>
#include <list>
#include <map>

class Topology
{
public:
  typedef std::map<std::string, Node> NodeMap;

  void
  add(const Node& node)
  {
    m_nodes.insert(std::pair<std::string, Node>(node.getName(), node));
  }

  void
  add(const Link& link)
  {
    m_links.push_back(link);
  }

  void
  print() const
  {
    for (const auto& pair : m_nodes) {
      const Node& node = pair.second;

      std::cout << "Node{ "
                << "name: " << node.getName()
                << ", angle: " << node.getAngle()
                << ", radius: " << node.getRadius()
                << " Neighbors[ ";

      for (const Node* neighbor : node.getNeighbors()) {
        std::cout << neighbor->getName() << ", ";
      }

      std::cout << "] }" << std::endl;
    }

    for (const Link& link : m_links) {
      std::cout << "Link{ "
                << "src: " << link.getSrc()
                << ", dst: " << link.getDst()
                << ", delay: " << link.getDelay()
                << "}" << std::endl;
    }
  }

  void
  build();

  const NodeMap&
  getNodes() const
  {
    return m_nodes;
  }

  const std::list<Link>&
  getLinks() const
  {
    return m_links;
  }

  const Node*
  getNode(std::string name) const
  {
    NodeMap::const_iterator it = m_nodes.find(name);

    if (it != m_nodes.end()) {
      return &(it->second);
    }
    else {
      return nullptr;
    }
  }

  const Link*
  findLink(const std::string& src, const std::string& dst) const
  {
    std::list<Link>::const_iterator it = std::find_if(m_links.begin(), m_links.end(),
      [src, dst] (const Link& link) {
        // Links are bidirectional
        if ((link.getSrc() == src && link.getDst() == dst) ||
            (link.getSrc() == dst && link.getDst() == src))
        {
          return true;
        }
        else {
          return false;
        }
      }
    );

    if (it != m_links.end()) {
      return &(*it);
    }
    else {
      return nullptr;
    }
  }

public:
  NodeMap m_nodes;
  std::list<Link> m_links;
};

#endif // TOPLOGY_HPP