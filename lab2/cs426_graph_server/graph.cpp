/* This is cpp file of class Graph*/

#include "graph.h"

Graph::Graph(){}

uint32_t Graph::add_node(uint64_t node_id) {

  /* if the node already exists */
  if(node_map.count(node_id)){
    return 204;
  }

  /* on success */
  else{
    uint64_t node_index;
    /* no recently deleted node */
    if(index_stack.empty()){
      uint32_t size = edge_vector.size();
      for(uint32_t i = 0; i < size; i ++){
        edge_vector[i].push_back(false);
      }
      edge_vector.push_back(vector<bool>(size+1,false));
      node_index = size;
    }
    else{
      node_index = index_stack.top();
      index_stack.pop();
    }
    node_map.insert(make_pair(node_id,node_index));
    return 200;
  }
}

uint32_t Graph::add_edge(uint64_t node_a_id, uint64_t node_b_id) {

  /* if either node doesn't exist, or if node_a_id is the same as node_b_id */
  if(node_a_id == node_b_id || node_map.count(node_a_id) == 0 || node_map.count(node_b_id) == 0){
    return 400;
  }

  /* if the edge already exists */
  else if(edge_vector[node_map[node_a_id]][node_map[node_b_id]] == true){
    return 204;
  }

  /* on success */
  else{
    edge_vector[node_map[node_a_id]][node_map[node_b_id]] = true;
    edge_vector[node_map[node_b_id]][node_map[node_a_id]] = true;
    return 200;
  }
}

uint32_t Graph::remove_node(uint64_t node_id) {

  /* if the node does not exist */
  if(!node_map.count(node_id)){
      return 400;
  }

  /* on success */
  else{
    int node_index = node_map[node_id];
    for ( auto it = node_map.begin(); it != node_map.end(); ++it )
      if(it->first == node_id)
      node_map.erase(it);

    int size = edge_vector.size();
    for(int i = 0; i < size; i++){
      edge_vector[node_index][i] = false;
      edge_vector[i][node_index] = false;
    }
    index_stack.push(node_index);
    return 200;
  }
}

uint32_t Graph::remove_edge(uint64_t node_a_id, uint64_t node_b_id) {

  /* if the edge does not exist */
  if(node_a_id == node_b_id || node_map.count(node_a_id) == 0 || node_map.count(node_b_id) == 0 || edge_vector[node_map[node_a_id]][node_map[node_b_id]] == false){
    return 400;
  }

  /* on success */
  else{
    edge_vector[node_map[node_a_id]][node_map[node_b_id]] = false;
    edge_vector[node_map[node_b_id]][node_map[node_a_id]] = false;
    return 200;
  }
}

uint32_t Graph::get_node(uint64_t node_id, bool & flag) {
  if(node_map.count(node_id)) flag = true;
  else flag = false;
  return 200;
}

uint32_t Graph::get_edge(uint64_t node_a_id, uint64_t node_b_id, bool & flag) {

  /* at least one of the vertices does not exist */
  if(node_a_id == node_b_id || node_map.count(node_a_id) == 0 || node_map.count(node_b_id) == 0){
    return 400;
  }

  /* on success*/
  else{
    if(edge_vector[node_map[node_a_id]][node_map[node_b_id]]){
      flag = true;
    }
    else flag = false;
  }
  return 200;
}

uint32_t Graph::get_neighbors(uint64_t node_id,vector<uint64_t> &neighbors) {

  /* if the node does not exist*/
  if(!node_map.count(node_id)){
    return 400;
  }

  /* on success*/
  else{
    for(uint32_t i = 0; i < edge_vector.size(); i++){
      if(edge_vector[node_map[node_id]][i]){
        for ( auto it = node_map.begin(); it != node_map.end(); ++it ){
          if(it->second == i){
            neighbors.push_back(it->first);
            break;
          }
        }
      }
    }
    return 200;
  }
}

uint32_t Graph::shortest_path(uint64_t node_a_id, uint64_t node_b_id, int &dist) {

  /* if either node does not exist */
  if(node_map.count(node_a_id) == 0 || node_map.count(node_b_id) == 0){
    return 400;
  }
  else{
    vector<bool> v(edge_vector.size(), false);
    queue<uint64_t> node_queue;
    vector<int> distance(edge_vector.size(),-1);

    distance[node_map[node_a_id]] = 0;
    node_queue.push(node_map[node_a_id]);

    while(!node_queue.empty()){
      uint64_t cur = node_queue.front();node_queue.pop();

      v[cur] = true;

      if(cur == node_map[node_b_id]){
        break;
      }

      for(uint32_t i = 0; i < edge_vector.size(); i ++){
        if(v[i] == false && edge_vector[i][cur] == true){
            node_queue.push(i);
            distance[i] = distance[cur] + 1;
            v[i] = true;
        }
      }
    }

    /* if there is no path */
    if(distance[node_map[node_b_id]] == -1){
      return 204;
    }

    /* on success*/
    else{
      dist = distance[node_map[node_b_id]];
      return 200;
    }
  }
}

void Graph::print(){
  for(auto m: node_map){
    cout << "Node: " << m.first << "  Neighbors: ";
    vector<uint64_t> neighbors;
    get_neighbors(m.first,neighbors);
    for(auto x : neighbors) cout << x << " ";
    cout << endl;
  }
}

vector<pair<uint64_t,uint64_t>> Graph::get_all_edge(){
  vector<pair<uint64_t,uint64_t>> res;
  unordered_map<uint64_t,uint64_t> m;
  for(auto x: node_map){m[x.second] = x.first;}
  for(uint32_t i = 0; i < edge_vector.size(); i++){
    for(uint32_t j = i+1; j < edge_vector.size(); j++){
      if(edge_vector[i][j] == 1){
        res.push_back(make_pair(m[i],m[j]));
      }
    }
    res.push_back(make_pair(m[i],m[i]));
  }
  return res;
}
