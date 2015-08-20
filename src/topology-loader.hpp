/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef TOPOLOGY_LOADER_HPP
#define TOPOLOGY_LOADER_HPP

#include <boost/tokenizer.hpp>
#include <fstream>
#include <string>

class TopologyLoader
{
public:
  typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;

  bool
  load(const std::string& filename);

private:
  void
  processFile(std::ifstream& file);

  std::string
  getValueFromTokens(const Tokenizer& tokens, const std::string key);

private:
  void
  loadNodes(std::ifstream& file);

  std::string
  getNodeName(const Tokenizer& tokens);

  std::string
  getNodeAngle(const Tokenizer& tokens);

  std::string
  getNodeRadius(const Tokenizer& tokens);

private:
  void
  loadLinks(std::ifstream& file);

  std::pair<std::string, std::string>
  getNodesFromLink(const Tokenizer& tokens);

  std::string
  getDelay(const Tokenizer& tokens);
};

#endif
