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

  void
  getTimeouts();

  void
  printHelp();

  void
  quit();

private:
  typedef std::function<void(const Node&, const Node&)> NodePairCommand;

  void
  executeNodePairCommand(const NodePairCommand& command, const std::string& cmdName);

private:
  const Topology& m_topo;

  typedef std::function<void()> CommandCallback;
  typedef std::map<std::string, CommandCallback> CommandMap;
  CommandMap m_commands;

  bool m_isRunning;
  std::vector<std::string> m_args;
};

 #endif // COMMAND_PROMPT_HPP