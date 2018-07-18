/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef ROUTING_TABLE_HPP
#define ROUTING_TABLE_HPP

#include <exception>
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

typedef std::set<NextHop> NextHopSet;

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
  getBestRoute(const std::string& dst, const std::string& inFace) const
  {
    NextHopTable::const_iterator it = m_table.find(dst);

    if (it != m_table.end() && !it->second.empty()) {
      const NextHopSet hops = it->second;
      NextHopSet::const_iterator hop = hops.begin();

      // Is the nexthop not the incoming Face?
      if (hop->face != inFace) {
        return &(*hop);
      }
      else {
        // Do not use the incoming face, try to use the second best face
        ++hop;

        if (hop != hops.end()) {
          return &(*hop);
        }
        else {
          return nullptr;
        }
      }
    }
    else {
      return nullptr;
    }
  }

  const NextHopSet&
  getNextHops(const std::string& dst) const
  {
    std::map<std::string, NextHopSet>::const_iterator it = m_table.find(dst);

    if (it != m_table.end()) {
      return it->second;
    }
    else {
      throw std::runtime_error("Tried to get NextHops for non-existent dst: " + dst);
    }
  }

private:
  std::map<std::string, NextHopSet> m_table;
};

#endif // ROUTING_TABLE_HPP
