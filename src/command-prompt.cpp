/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "command-prompt.hpp"

#include "nexthop-difference-calculator.hpp"
#include "path.hpp"
#include "path-calculator.hpp"
#include "topology.hpp"
#include <fstream>
#include <boost/tokenizer.hpp>
#include <functional>
#include <iostream>
#include "hr-calculator.hpp"
#include <boost/math/constants/constants.hpp>

const double CommandPrompt::MATH_PI = boost::math::constants::pi<double>();

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


  //get uds delay 
  m_commands["u"] = std::bind(&CommandPrompt::udsDelayStretch, this);

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

float 
CommandPrompt::computePercentile(m_UDS& uds){

// create an iterator to access elements of map
  float _90_percentile, _75_percentile, _50_percentile, _25_percentile;
  std::map<std::pair<std::string, std::string>, float> :: iterator iterator_name;
  std::vector<float> v;
  int i = 0;
  for(iterator_name = uds.begin(); iterator_name != uds.end(); iterator_name++)
  {

    v.push_back(iterator_name->second);
  }
  std::sort(v.begin(), v.end());

  int size = v.size();
  _90_percentile = 0.90*size; 
  _75_percentile = 0.75*size; 
  _50_percentile = 0.50*size; 
  _25_percentile = 0.25*size; 
  std::cout << "25th percentile: " << v[(int)_25_percentile] << "\n"
            << "50th percentile: " << v[(int)_50_percentile] << "\n"
            << "75th percentile: " << v[(int)_75_percentile] << "\n"
            << "90th percentile: " << v[(int)_90_percentile]
            << std::endl;
  return 0.0;
}

float 
CommandPrompt::getGeographicalDistance(float th1, float phi1, float th2, float phi2 ){

    float R_earth = 6371.0; //radius of earth
    float dtheta = acos(cos(th1)*cos(th2) + sin(th1)*sin(th2)*cos(phi1 - phi2));
    float x_ij = R_earth * dtheta;
    return x_ij;
}

int 
CommandPrompt::getUnderlayDelay( float th1, float phi1, float th2, float phi2 ){

  return (int)(getGeographicalDistance(th1, phi1, th2, phi2) + 1164.87669839) / 49.4306893399 ;
}

void 
CommandPrompt::udsDelayStretch(){

  std::ifstream infile("current_test_hr");
  std::string line;
  std::vector<std::tuple <std::string, float, float>> node;
  PathCalculator calculator;
  const RoutingTable* routingTable;
  float ggr_delay, ggr_delay_inv, uds_stretch;

  while (std::getline(infile, line))
  {
    std::string delimiter = " ";
    size_t pos = 0;
    std::vector<std::string> v;
    float latitude, longitude, temp_longitude;
    while ((pos = line.find(delimiter)) != std::string::npos) {  
      v.push_back(line.substr(0, pos));
      line.erase(0, pos + delimiter.length());
    } 
    v.push_back(line);

    // calculate latitude and longitude in radians 
    latitude = (-std::stof(v[4]) + 90.0) * MATH_PI / 180.0;
    temp_longitude = std::stof(v[5]);

    if (temp_longitude < 0.0)
      longitude = 2 * MATH_PI - fabs(temp_longitude * MATH_PI / 180.0);
    else
      longitude = temp_longitude * MATH_PI / 180.0;
  
    node.push_back(std::make_tuple(v[0], latitude, longitude));
  }  
  for(std::vector<int>::size_type i = 0; i != node.size(); i++) {
    for(std::vector<int>::size_type j = i+1; j != node.size(); j++) {
      float geodelay = getUnderlayDelay(std::get<1>(node[i]), 
                                        std::get<2>(node[i]), 
                                        std::get<1>(node[j]), 
                                        std::get<2>(node[j]));
      m_links_GGR[std::make_pair(std::get<0>(node[i]),std::get<0>(node[j]))] = geodelay;
    }
  }
  int count = 0; 
  float averageUDSdelay = 0.0;
  if (m_args.size() == 0) {

    for (const auto& srcPair : m_topo.getNodes()) {
      const Node& src = srcPair.second;

      for (const auto& dstPair : m_topo.getNodes()) {
        const Node& dst = dstPair.second;
        
        //check if these pairs have links or not  - questionable (coz we need to compute UDS even if the link doesn't exists)

        //const Link* link = m_topo.findLink(src.getName(), dst.getName());
        //if(link){
          if (src.getName() != dst.getName()) {
            Path hr = calculator.getHyperbolicPath(m_topo, src, dst);

            ggr_delay = m_links_GGR[std::make_pair(src.getName(), dst.getName())];
            ggr_delay_inv = m_links_GGR[std::make_pair(dst.getName(), src.getName())];
      
            //compute udsDelay Stretch
            if (hr.getRtt() != Path::INFINITE_RTT){   
              if (ggr_delay != 0){
                uds_stretch = ((float)hr.getRtt()/2)/(float)ggr_delay;}
              else  
                uds_stretch = (float)hr.getRtt()/(float)ggr_delay_inv;
            }

            if(m_links_UDS.find(std::make_pair(dst.getName(), src.getName())) == m_links_UDS.end() && 
               m_links_UDS.find(std::make_pair(src.getName(), dst.getName())) == m_links_UDS.end()){
                m_links_UDS[std::make_pair(dst.getName(), src.getName())] = uds_stretch;

              averageUDSdelay+=uds_stretch;             
              // std::cout << "( " << src.getName() 
              //         <<  ", " << dst.getName() 
              //         << " ) "
              //         << " UDS stretch: "
              //         << uds_stretch 
              //         << std::endl;  
            count++;
            }            
          }
        //}
      }
    } 
  }
  std::cout << "Average UDS delay: " << (averageUDSdelay/count) << std::endl;
  computePercentile(m_links_UDS);

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
  NodePairCommand command =
    [this] (const Node& src, const Node& dst) {
      PathCalculator calculator;

      Path hr = calculator.getHyperbolicPath(m_topo, src, dst);
      Path ls = calculator.getLinkStatePath(m_topo, src, dst);
      std::cout << "stretch(" << src.getName() << ", " << dst.getName()
                << ") = " << computeStretch(hr, ls) << std::endl;
    };

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