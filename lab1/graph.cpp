/* This is cpp file of class Graph*/

#include "graph.h"

Graph::Graph(){}

int Graph::add_node(int node_id) {

  /* if the node already exists */
  if(node_map.count(node_id)){
    return 204;
  }

  /* on success */
  else{
    int node_index;
    /* no recently deleted node */
    if(index_stack.empty()){
      int size = edge_vector.size();
      for(int i = 0; i < size; i ++){
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

int Graph::add_edge(int node_a_id, int node_b_id) {

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

int Graph::remove_node(int node_id) {

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

int Graph::remove_edge(int node_a_id, int node_b_id) {

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

int Graph::get_node(int node_id, bool & flag) {
  if(node_map.count(node_id)) flag = true;
  else flag = false;
  return 200;
}

int Graph::get_edge(int node_a_id, int node_b_id, bool & flag) {

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

int Graph::get_neighbors(int node_id,vector<int> &neighbors) {

  /* if the node does not exist*/
  if(!node_map.count(node_id)){
    return 400;
  }

  /* on success*/
  else{
    for(int i = 0; i < edge_vector.size(); i++){
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

int Graph::shortest_path(int node_a_id, int node_b_id, int &dist) {

  /* if either node does not exist */
  if(node_map.count(node_a_id) == 0 || node_map.count(node_b_id) == 0){
    return 400;
  }
  else{
    vector<bool> v(edge_vector.size(), false);
    queue<int> node_queue;
    vector<int> distance(edge_vector.size(),-1);
    
    distance[node_map[node_a_id]] = 0;
    node_queue.push(node_map[node_a_id]);
    
    while(!node_queue.empty()){
      int cur = node_queue.front();node_queue.pop();
      
      v[cur] = true;
      
      if(cur == node_map[node_b_id]){
        break;
      }
      
      for(int i = 0; i < edge_vector.size(); i ++){
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
