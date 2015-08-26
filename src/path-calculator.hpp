/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef PATH_CALCULATOR_HPP
#define PATH_CALCULATOR_HPP

#include <string>

class Node;
class Topology;

class PathCalculator
{
public:
  std::string
  getPath(const Topology& topo, const Node& src, const Node& dst);

private:
  struct PathAndRttPair
  {
    const std::string path;
    const double rtt;

    std::string
    toString() const
    {
      std::string output;

      output += path;

      if (rtt != PathCalculator::INFINITE_RTT) {
        output += " (RTT: " + std::to_string(int(rtt)) + ")";
      }

      output += "\n";

      return output;
    }
  };

  PathAndRttPair
  getPath(const Topology& topo, const Node& src, const Node& dst, bool isHyperbolic);

  std::string
  getStretch(const PathAndRttPair& hr, const PathAndRttPair& ls) const;

public:
  static const std::string NO_PATH_DISCONNECTED;
  static const std::string NO_PATH_LOOP;

private:
  static const double INFINITE_RTT;
};

#endif // PATH_CALCULATOR_HPP
