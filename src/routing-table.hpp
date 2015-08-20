/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef ROUTING_TABLE_HPP
#define ROUTING_TABLE_HPP

#include <map>
#include <set>
#include <string>

class NextHop
{
public:
  NextHop(const std::string& face, double cost)
    : face(face)
    , cost(cost)
  {
  }

  bool
  operator<(const NextHop& other) const
  {
    if (this->cost < other.cost) {
      return true;
    }
    else if (this->cost == other.cost) {
      return this->face < other.face;
    }
    else {
      return false;
    }
  }

public:
  const std::string face;
  const double cost;
};

class RoutingTable
{
public:
  typedef std::map<std::string, std::set<NextHop>> NextHopTable;

  void
  addNextHop(const std::string& dst, const NextHop& hop)
  {
    m_table[dst].insert(hop);
  }

  const NextHop*
  getBestRoute(const std::string& dst) const
  {
    NextHopTable::const_iterator it = m_table.find(dst);

    if (it != m_table.end() && !it->second.empty()) {
      return &(*(it->second.begin()));
    }
    else {
      return nullptr;
    }
  }

private:
  std::map<std::string, std::set<NextHop>> m_table;
};

#endif // ROUTING_TABLE_HPP
