/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef ROUTING_TABLE_HPP
#define ROUTING_TABLE_HPP

#include <list>
#include <map>
#include <string>

class NextHop
{
public:
  NextHop(const std::string& face, double cost)
    : face(face)
    , cost(cost)
  {
  }

public:
  const std::string face;
  const double cost;
};

class RoutingTable
{
public:
  void
  addNextHop(const std::string& dst, const NextHop& hop)
  {
    m_table[dst].push_back(hop);
  }

private:
  std::map<std::string, std::list<NextHop>> m_table;
};

#endif // ROUTING_TABLE_HPP
