/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, The University of Memphis.
 **/

#include "ls-calculator.hpp"

#include "topology.hpp"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <exception>
#include <map>
#include <vector>

using namespace boost;

typedef adjacency_list<listS, vecS, directedS, no_property, property<edge_weight_t, int>> AdjacencyList;
typedef graph_traits<AdjacencyList>::vertex_descriptor Vertex;
typedef graph_traits<AdjacencyList>::vertex_iterator VertexIterator;
typedef std::pair<int, int> Edge;
typedef std::map<std::string, size_t> NodeMap;
typedef std::map<size_t, std::string> NameMap;

class Graph
{
public:
  Graph(const Topology& topo)
    : m_nNodes(0)
    , m_nLinks(0)
  {
    m_nLinks = 2*topo.getLinks().size();

    // Populate node ID and node name maps
    for (const auto& pair : topo.getNodes()) {
      const Node& node = pair.second;

      m_nodeMap[node.getName()] = m_nNodes;
      m_nameMap[m_nNodes] = node.getName();

      ++m_nNodes;
    }

    // Create edge and weight arrays
    m_edgeArray = new Edge[m_nLinks];
    m_weights = new int[m_nLinks];
    int i = 0;

    for (const Link& link : topo.getLinks()) {
      const size_t src = m_nodeMap[link.getSrc()];
      const size_t dst = m_nodeMap[link.getDst()];

      m_edgeArray[i] = Edge(src, dst);
      m_weights[i] = link.getDelay();
      ++i;

      m_edgeArray[i] = Edge(dst, src);
      m_weights[i] = link.getDelay();
      ++i;
    }

    m_graph = AdjacencyList(m_edgeArray, m_edgeArray + m_nLinks, m_weights, m_nNodes);

    property_map<AdjacencyList, edge_weight_t>::type weightmap = get(edge_weight, m_graph);
  }

  size_t
  getNodeId(const std::string& name) const
  {
    NodeMap::const_iterator it = m_nodeMap.find(name);

    if (it != m_nodeMap.end()) {
      return it->second;
    }
    else {
      throw std::runtime_error("Node not found: " + name);
    }
  }

  const std::string&
  getNodeName(size_t nodeId) const
  {
    NameMap::const_iterator it = m_nameMap.find(nodeId);

    if (it != m_nameMap.end()) {
      return it->second;
    }
    else {
      throw std::runtime_error("Node not found: " + std::to_string(nodeId));
    }
  }

  const Vertex
  getVertex(const std::string& name) const
  {
    size_t id = getNodeId(name);

    return vertex(id, m_graph);
  }

  size_t
  getNumVertices() const
  {
    return num_vertices(m_graph);
  }

  const AdjacencyList&
  toBoost() const
  {
    return m_graph;
  }

  std::pair<VertexIterator, VertexIterator>
  getVertexIterators()
  {
    return vertices(m_graph);
  }

  void
  onlyUseThisLinkFromSource(const Vertex& src, const Link& link)
  {
    size_t side1 = getNodeId(link.getSrc());
    size_t side2 = getNodeId(link.getDst());

    for (int i = 0; i < m_nLinks; ++i) {
      Edge& edge = m_edgeArray[i];

      // If the edge is connected to the src node
      if (edge.first == src || edge.second == src) {
        // If the edge is the one to use
        if ((edge.first == side1 && edge.second == side2) || (edge.first == side2 || edge.second == side1)) {
          continue;
        }
        else {
          // This graph should not use this edge
          m_edgeArray[i] = Edge(0, 0);
        }
      }
    }

    // Reset graph
    m_graph = AdjacencyList(m_edgeArray, m_edgeArray + m_nLinks, m_weights, m_nNodes);
  }

private:
  NodeMap m_nodeMap;
  NameMap m_nameMap;

  AdjacencyList m_graph;

  size_t m_nNodes;
  size_t m_nLinks;

  int* m_weights;
  Edge* m_edgeArray;
};

class DijkstraAlgorithm
{
public:
  DijkstraAlgorithm(const Graph& graph, const Vertex& src)
    : m_graph(graph)
    , m_src(src)
    , m_predecessors(m_graph.getNumVertices())
    , m_distances(m_graph.getNumVertices())
  {

  }

  void
  computeShortestPaths()
  {
    dijkstra_shortest_paths(m_graph.toBoost(),
                            m_src,
                            predecessor_map(&m_predecessors[0]).distance_map(&m_distances[0]));
  }

  int
  getDistance(size_t nodeId)
  {
    return m_distances[nodeId];
  }

  Vertex
  getPredecessor(size_t nodeId)
  {
    return m_predecessors[nodeId];
  }

private:
  const Graph& m_graph;
  const Vertex& m_src;

  std::vector<Vertex> m_predecessors;
  std::vector<int> m_distances;
};

void
LinkStateRoutingCalculator::calculatePaths(Topology& topo, Node& thisNode)
{
  // Put all links connected to source node into linksFromSource
  const std::list<Link>& links = topo.getLinks();

  std::set<const Link*> linksFromSource;
  for (const Link& link : links) {
    if (link.getSrc() == thisNode.getName() || link.getDst() == thisNode.getName()) {
      linksFromSource.insert(&link);
    }
  }

  // Compute shortest path using each link from the source node
  for (const Link* link : linksFromSource) {
    Graph graph(topo);

    Vertex src = graph.getVertex(thisNode.getName());
    graph.onlyUseThisLinkFromSource(src, *link);


    DijkstraAlgorithm dijkstra(graph, src);
    dijkstra.computeShortestPaths();

    VertexIterator vi, vend;
    size_t currentIndex = 0;
    const size_t srcIndex = graph.getNodeId(thisNode.getName());

    for (boost::tie(vi, vend) = graph.getVertexIterators(); vi != vend; ++vi) {
      // Don't add nexthops to get to self
      if (*vi == srcIndex) {
        continue;
      }

      currentIndex = *vi;
      std::list<size_t> path;

      while (currentIndex != srcIndex) {
        size_t predecessor = dijkstra.getPredecessor(currentIndex);

        if (predecessor != currentIndex) {
          path.push_front(currentIndex);
        }
        else {
          // No way to reach this node
          break;
        }

        currentIndex = predecessor;
      }

      // No route
      if (path.empty()) {
        continue;
      }

      const std::string& adj = graph.getNodeName(path.front());
      const std::string& dstRouterName = graph.getNodeName(*vi);
      double cost = dijkstra.getDistance(*vi);

      addNextHop(adj, dstRouterName, cost, thisNode.getLinkStateRoutingTable());
    }
  }
}
