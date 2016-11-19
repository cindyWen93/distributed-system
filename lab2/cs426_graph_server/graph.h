#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <map>
#include <stack>
#include <vector>
#include <queue>
#include <string.h>
#include <unordered_map>
#include <utility>

using namespace std;

class Graph{
private:
  map<uint64_t,uint64_t> node_map; /* hash input node_id to a stored id */
  vector<vector<bool>> edge_vector; /* edges of nodes */
  stack<uint64_t> index_stack; /* deleted node id */

public:
  Graph();
  uint32_t add_node(uint64_t); /* this function is used to add a node to the graph */
  uint32_t add_edge(uint64_t, uint64_t); /* this function is used to add an edge to the graph */
  uint32_t remove_node(uint64_t); /* this function is used to remove a node from the graph */
  uint32_t remove_edge(uint64_t,uint64_t); /* this function is used to remove an edge from the graph */
  uint32_t get_node(uint64_t,bool&); /* this function is used to check whether the node in the graph */
  uint32_t get_edge(uint64_t,uint64_t,bool&); /* this function is used to check whether the edge is in the graph */
  uint32_t get_neighbors(uint64_t,vector<uint64_t>&); /* this function is used to get the neighbors of certain node in the graph */
  uint32_t shortest_path(uint64_t,uint64_t,int&); /* this function is used to cal shortest path of given pair of nodes */
  void print();
  vector<pair<uint64_t,uint64_t>> get_all_edge();

};

#endif
