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

using namespace std;

class Graph{
private:
  map<int,int> node_map; /* hash input node_id to a stored id */
  vector<vector<bool>> edge_vector; /* edges of nodes */
  stack<int> index_stack; /* deleted node id */

public:
  Graph();
  int add_node(int); /* this function is used to add a node to the graph */
  int add_edge(int, int); /* this function is used to add an edge to the graph */
  int remove_node(int); /* this function is used to remove a node from the graph */
  int remove_edge(int,int); /* this function is used to remove an edge from the graph */
  int get_node(int,bool&); /* this function is used to check whether the node in the graph */
  int get_edge(int,int,bool&); /* this function is used to check whether the edge is in the graph */
  int get_neighbors(int,vector<int>&); /* this function is used to get the neighbors of certain node in the graph */
  int shortest_path(int,int,int&); /* this function is used to cal shortest path of given pair of nodes */
};

#endif
