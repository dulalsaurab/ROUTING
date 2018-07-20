/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#ifndef LINK_HPP
#define LINK_HPP

#include <boost/lexical_cast.hpp>
#include <string>

class Link
{
public:
  Link(const std::string& src, const std::string& dst, const std::string& delay)
    : m_src(src)
    , m_dst(dst)
  {
    m_delay = boost::lexical_cast<double>(delay);
  }

  const std::string&
  getSrc() const
  {
    return m_src;
  }

  const std::string&
  getDst() const
  {
    return m_dst;
  }

  double
  getDelay() const
  {
    return m_delay;
  }

private:
  const std::string m_src;
  const std::string m_dst;
  double m_delay;
};

#endif // LINK_HPP
