/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef NODE_HPP
#define NODE_HPP

#include <boost/lexical_cast.hpp>
#include <list>
#include <string>

#include "routing-table.hpp"

class Node
{
public:
  Node(const std::string& name, const std::string& angle, const std::string& radius)
    : m_name(name)
  {
    m_angle = boost::lexical_cast<double>(angle);
    m_radius = boost::lexical_cast<double>(radius);
  }

  const std::string&
  getName() const
  {
    return m_name;
  }

  double
  getAngle() const
  {
    return m_angle;
  }

  double
  getRadius() const
  {
    return m_radius;
  }

  void
  addNeighbor(const Node& node)
  {
    m_neighbors.push_back(node);
  }

  const std::list<Node>&
  getNeighbors() const
  {
    return m_neighbors;
  }

  RoutingTable&
  getHyperbolicRoutingTable()
  {
    return m_hyperbolicRoutingTable;
  }

private:
  std::string m_name;
  double m_angle;
  double m_radius;

  std::list<Node> m_neighbors;
  RoutingTable m_hyperbolicRoutingTable;
};

#endif // NODE_HPP
