/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef PATH_HPP
#define PATH_HPP

#include <list>
#include <string>

class Node;
class Topology;

class Path
{
public:
  enum Error
  {
    ERROR_NONE,
    ERROR_LOOP,
    ERROR_NO_NEXTHOP
  };

  Path()
    : m_rtt(INFINITE_RTT)
    , m_error(ERROR_NONE)
  {

  }

  void
  addNode(const std::string& node)
  {
    m_nodes.push_back(node);
  }

  void
  setRtt(double rtt)
  {
    m_rtt = rtt;
  }

  void
  setError(Error error)
  {
    m_error = error;
  }

public:
  std::string
  toString() const
  {
    std::string output;

    bool isFirstNode = true;

    for (const std::string& node : m_nodes) {
      if (!isFirstNode) {
        output += " -> ";
      }

      output += node;
      isFirstNode = false;
    }

    if (m_error == ERROR_NONE) {
      output += " (RTT: " + std::to_string(int(m_rtt)) + "ms)";
    }
    else {
      output += " (ERROR: ";

      if (m_error == ERROR_LOOP) {
        output += "LOOP";
      }
      else if (m_error == ERROR_NO_NEXTHOP) {
        output += "NO NEXTHOP";
      }

      output += ")";
    }

    return output;
  }

  double
  getRtt() const
  {
    return m_rtt;
  }

  Error
  getError() const
  {
    return m_error;
  }

public:
  static const double INFINITE_RTT;

private:
  std::list<std::string> m_nodes;
  double m_rtt;
  Error m_error;
};

#endif // PATH_HPP
