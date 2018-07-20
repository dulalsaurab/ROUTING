/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "command-prompt.hpp"

#include "nexthop-difference-calculator.hpp"
#include "path.hpp"
#include "path-calculator.hpp"
#include "topology.hpp"

#include <boost/tokenizer.hpp>
#include <functional>
#include <iostream>
#include <fstream>

CommandPrompt::CommandPrompt(const Topology& topo)
  : m_topo(topo)
  , m_isRunning(true)
{
  // Calculate all paths
  m_commands["p"] = std::bind(&CommandPrompt::calculatePaths, this);
  m_commands["path"] = std::bind(&CommandPrompt::calculatePaths, this);

  // Get NextHop difference
  m_commands["d"] = std::bind(&CommandPrompt::getNextHopDifference, this);
  m_commands["diff"] = std::bind(&CommandPrompt::getNextHopDifference, this);

  // Get stretch
  m_commands["s"] = std::bind(&CommandPrompt::getStretch, this);
  m_commands["stretch"] = std::bind(&CommandPrompt::getStretch, this);
  
  // Get node pairs that should timeout
  m_commands["t"] = std::bind(&CommandPrompt::getTimeouts, this);
  m_commands["timeout"] = std::bind(&CommandPrompt::getTimeouts, this);

  // Get node pairs that should timeout
  m_commands["help"] = std::bind(&CommandPrompt::printHelp, this);

  // Quit
  m_commands["exit"] = std::bind(&CommandPrompt::quit, this);
  m_commands["q"] = std::bind(&CommandPrompt::quit, this);
  m_commands["quit"] = std::bind(&CommandPrompt::quit, this);
}

void
CommandPrompt::run()
{
  std::string input;

  typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;
  boost::char_separator<char> sep(" ");

  while (m_isRunning) {
    std::cout << "> ";

    std::getline(std::cin, input);

    Tokenizer tokens(input, sep);

    // Empty input?
    if (tokens.begin() == tokens.end()) {
      continue;
    }

    std::string command = *(tokens.begin());

    Tokenizer::iterator arg = tokens.begin();
    for (++arg; arg != tokens.end(); ++arg) {
      m_args.push_back(*arg);
    }

    CommandMap::const_iterator it = m_commands.find(command);

    if (it != m_commands.end()) {
      it->second();
    }
    else {
      std::cout << "Unknown command: " << command << std::endl;
    }

    m_args.clear();
  }
}

void
CommandPrompt::calculatePaths()
{
  NodePairCommand command =
    [this] (const Node& src, const Node& dst) {
      PathCalculator calculator;

      Path hr = calculator.getHyperbolicPath(m_topo, src, dst);
      std::cout << "HR: " << hr.toString() << std::endl;

      Path ls = calculator.getLinkStatePath(m_topo, src, dst);
      std::cout << "LS: " << ls.toString() << std::endl;
    };

  executeNodePairCommand(command, "path");
}

void
CommandPrompt::getNextHopDifference()
{
  NodePairCommand command =
    [] (const Node& src, const Node& dst) {
      const NextHopSet& lsNextHops = src.getLinkStateRoutingTable().getNextHops(dst.getName());
      const NextHopSet& hrNextHops = src.getHyperbolicRoutingTable().getNextHops(dst.getName());

      double difference = NextHopDifferenceCalculator::getDifference(lsNextHops, hrNextHops);

      std::cout << "nexthop-diff(" << src.getName() << ", " << dst.getName()
                << ") = " << difference << std::endl;
    };

  executeNodePairCommand(command, "diff");
}

std::string
computeStretch(const Path& hr, const Path& ls)
{
  if (hr.getRtt() == Path::INFINITE_RTT && ls.getRtt() != Path::INFINITE_RTT) {
    return "infinity";
  }
  else if (hr.getRtt() != Path::INFINITE_RTT && ls.getRtt() == Path::INFINITE_RTT) {
    return "-infinity";
  }
  else {
    return std::to_string(hr.getRtt()/ls.getRtt());
  }
}

void
CommandPrompt::getStretch()
{
  //std::ofstream myfile ("test.txt");
  NodePairCommand command =
    [this] (const Node& src, const Node& dst) {
      PathCalculator calculator;
      std::ofstream myfile ("test.txt", std::ios_base::app);
      Path hr = calculator.getHyperbolicPath(m_topo, src, dst);
      Path ls = calculator.getLinkStatePath(m_topo, src, dst);
      myfile << "stretch(" << src.getName() << ", " << dst.getName()
                << ") = " << computeStretch(hr, ls) <<std::endl;
   myfile.close();
   };
   // myfile.close();
  executeNodePairCommand(command, "stretch");
}

void
CommandPrompt::getTimeouts()
{
  PathCalculator calculator;
  int nTimeouts = 0;

  NodePairCommand command =
    [this, &nTimeouts] (const Node& src, const Node& dst) {
      PathCalculator calculator;

      Path hr = calculator.getHyperbolicPath(m_topo, src, dst);

      if (hr.getError() != Path::ERROR_NONE) {
        std::cout << src.getName() << " to " << dst.getName()
                  << " should timeout in HR" << std::endl;
        ++nTimeouts;
      }

      Path ls = calculator.getLinkStatePath(m_topo, src, dst);

      if (ls.getError() != Path::ERROR_NONE) {
        std::cout << src.getName() << " to " << dst.getName()
                  << " should timeout in LS" << std::endl;
        ++nTimeouts;
      }
    };

  executeNodePairCommand(command, "timeout");

  if (nTimeouts == 0) {
    std::cout << "No timeouts expected" << std::endl;
  }
}

void
CommandPrompt::printHelp()
{
  std::cout << "Help - commands:" << std::endl;

  for (const auto& cmd : m_commands) {
    std::cout << "  " << cmd.first << std::endl;
  }
}

void
CommandPrompt::quit()
{
  m_isRunning = false;
}

void
CommandPrompt::executeNodePairCommand(const NodePairCommand& command, const std::string& cmdName)
{
  if (m_args.size() == 0) {
    for (const auto& srcPair : m_topo.getNodes()) {
      const Node& src = srcPair.second;

      for (const auto& dstPair : m_topo.getNodes()) {
        const Node& dst = dstPair.second;

        if (src.getName() != dst.getName()) {
          command(src, dst);
        }
      }
    }
  }
  else if (m_args.size() == 2) {
    const Node* src = m_topo.getNode(m_args[0]);
    const Node* dst = m_topo.getNode(m_args[1]);

    if (src == nullptr) {
      std::cout << "ERROR: Unknown node '" << m_args[0] << "'" << std::endl;
      return;
    }
    else if (dst == nullptr) {
      std::cout << "ERROR: Unknown node '" << m_args[1] << "'" << std::endl;
      return;
    }

    if (dst == src) {
      std::cout << "ERROR: source and destination are the same node" << std::endl;
      return;
    }

    command(*src, *dst);
  }
  else {
    std::cout << "Usage: " << cmdName << " [src dst]" << std::endl;
  }
}
