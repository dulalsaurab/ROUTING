/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "command-prompt.hpp"

#include "path-calculator.hpp"
#include "topology.hpp"

#include <boost/tokenizer.hpp>
#include <iostream>

CommandPrompt::CommandPrompt(const Topology& topo)
  : m_topo(topo)
  , m_isRunning(true)
{
  // Calculate all paths
  m_commands["c"] = std::bind(&CommandPrompt::calculatePaths, this);
  m_commands["calc"] = std::bind(&CommandPrompt::calculatePaths, this);
  m_commands["calculate"] = std::bind(&CommandPrompt::calculatePaths, this);

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
  PathCalculator calculator;

  if (m_args.size() == 0) {
    for (const auto& srcPair : m_topo.getNodes()) {
      const Node& src = srcPair.second;

      for (const auto& dstPair : m_topo.getNodes()) {
        const Node& dst = dstPair.second;

        if (src.getName() != dst.getName()) {
          std::cout << calculator.getPath(m_topo, src, dst) << std::endl;
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

    std::cout << calculator.getPath(m_topo, *src, *dst) << std::endl;
  }
  else {
    std::cout << "Usage: calculate [src dst]" << std::endl;
  }
}

void
CommandPrompt::quit()
{
  m_isRunning = false;
}