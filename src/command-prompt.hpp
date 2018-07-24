/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef COMMAND_PROMPT_HPP
#define COMMAND_PROMPT_HPP

#include <functional>
#include <vector>
#include <map>
#include <string>

class Node;
class Topology;

class CommandPrompt
{
public:
  CommandPrompt(const Topology& topo);

  void
  run();

private:
  void
  calculatePaths();

  void
  getNextHopDifference();

  void
  getStretch();

  float
  getGeographicalDistance(float th1, float phi1, float th2, float phi2 );
 
  int 
  getUnderlayDelay(float th1, float phi1, float th2, float phi2 );

  /*
   This function computes UDS delay stretch, 

   Steps  
   1. Read the original file containing the latitude and longitude 
   2. Compute the geographical delay first 
   3. compute the hyperbolic delay
   4. (actual delay in the routing(GGR) / geographical delays between nodes(~underlay delay ) ) 
  
 **/
  void
  udsDelayStretch();

  void
  getTimeouts();

  void
  printHelp();

  void
  quit();

  // float 
  // computePercentile();

private:
  typedef std::map<std::pair<std::string, std::string>, float> m_UDS;
  typedef std::function<void(const Node&, const Node&)> NodePairCommand;

  void
  executeNodePairCommand(const NodePairCommand& command, const std::string& cmdName);

  float 
  computePercentile(m_UDS&);

  m_UDS m_links_UDS;


private:
  const Topology& m_topo;

  typedef std::function<void()> CommandCallback;
  typedef std::map<std::string, CommandCallback> CommandMap;
  CommandMap m_commands;

  bool m_isRunning;
  std::vector<std::string> m_args;
  // std::vector<std::tuple <std::string, std::string, float>> m_links_UDS;
  std::map<std::pair<std::string, std::string>, float> m_links_GGR;
  static const double MATH_PI;
};

 #endif // COMMAND_PROMPT_HPP