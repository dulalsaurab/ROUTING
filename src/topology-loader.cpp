/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "topology-loader.hpp"

#include "link.hpp"
#include "node.hpp"
#include "topology.hpp"

#include <iostream>

std::string
TopologyLoader::getValueFromTokens(const Tokenizer& tokens, const std::string key)
{
  Tokenizer::iterator it = std::find_if(tokens.begin(), tokens.end(),
    [key] (const std::string& s) {
      return s.find(key) != std::string::npos;
    });

  std::string value = *it;

  // Separate value from key=
  return value.substr(value.find('=') + 1);
}

bool
TopologyLoader::load(const std::string& filename, Topology& topo)
{
  std::cout << "Loading " << filename << "..." << std::endl;

  std::ifstream file;
  file.open(filename, std::ifstream::in);

  if (file.is_open()) {

    while (file.good()) {
      std::string line;
      std::getline(file, line);

      if (line == "[nodes]") {
        loadNodes(file, topo);
        loadLinks(file, topo);
      }
    }

    file.close();
    std::cout << "...Done" << std::endl;
    return true;
  }
  else {
    std::cout << "ERROR: Could not open " << filename << std::endl;
    return false;
  }
}

void
TopologyLoader::loadNodes(std::ifstream& file, Topology& topo)
{
  while (file.good()) {
    std::string line;
    std::getline(file, line);

    if (line == "[links]") {
      // Done loading nodes
      return;
    }
    else {
      boost::char_separator<char> sep(" ");
      Tokenizer tokens(line, sep);

      std::string name = getNodeName(tokens);
      std::string angle = getNodeAngle(tokens);
      std::string radius = getNodeRadius(tokens);

      topo.add(Node(name, angle, radius));
    }
  }
}

std::string
TopologyLoader::getNodeName(const Tokenizer& tokens)
{
  std::string name = *tokens.begin();

  // Remove colon from end of name
  name = name.substr(0, name.size() - 1);

  return name;
}

std::string
TopologyLoader::getNodeAngle(const Tokenizer& tokens)
{
  return getValueFromTokens(tokens, "angle");
}

std::string
TopologyLoader::getNodeRadius(const Tokenizer& tokens)
{
  return getValueFromTokens(tokens, "radius");
}

void
TopologyLoader::loadLinks(std::ifstream& file, Topology& topo)
{
  while (file.good()) {
    std::string line;
    std::getline(file, line);

    if (line.empty()) {
      continue;
    }

    boost::char_separator<char> sep(" ");
    Tokenizer tokens(line, sep);

    std::pair<std::string, std::string> nodes = getNodesFromLink(tokens);
    std::string delay = getDelay(tokens);

    // Remove "ms" from delay
    delay = delay.substr(0, delay.find('m'));

    topo.add(Link(nodes.first, nodes.second, delay));
  }
}

std::pair<std::string, std::string>
TopologyLoader::getNodesFromLink(const Tokenizer& tokens)
{
  std::string nodes = *tokens.begin();

  std::string src = nodes.substr(0, nodes.find(':'));
  std::string dst = nodes.substr(nodes.find(':') + 1);

  return std::pair<std::string, std::string>(src, dst);
}

std::string
TopologyLoader::getDelay(const Tokenizer& tokens)
{
  return getValueFromTokens(tokens, "delay");
}
