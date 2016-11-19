#include "disk.hpp"
#include "struct.hpp"
#include <iostream>
#include <sys/mman.h>
#include <vector>

using namespace std;

void Disk::Open(uint32_t fd){
  this->fd = fd;
}

void Disk::GetGeneration(){
  cout << "Current Generation: " << generation << endl;
}

void Disk::Init(){
  SupLog * spl = (SupLog*)mmap(NULL,sizeof(4096),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  generation = spl -> generation;
  start_address = spl -> start_address;
  block_num = spl -> block_num;
  data_start_address = start_address + block_num;
  cur_log_page = 1;
}

void Disk::Format(){
  SupLog * spl = (SupLog*)mmap(NULL,sizeof(4096),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  spl -> start_address = 1;
  spl -> block_num = 512 * 1024;

  if(isLogValid()){
    spl -> generation ++;
    spl -> checksum = ((spl -> start_address ) ^ (spl -> generation) ^ (spl -> block_num) ^ 1771);
  }
  else{
    spl -> generation = 0;
    spl -> checksum = ((spl -> start_address ) ^ (spl -> generation) ^ (spl -> block_num) ^ 1771);
  }

  Log *lg = (Log*)mmap(NULL,sizeof(Log),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 4096);
  lg -> generation = -1;
  lg -> entry_num = 0;

  Init();
}

void Disk::FormatCheckPoint(){
  SupData *sd = (SupData*)mmap(NULL,sizeof(SupData),PROT_READ|PROT_WRITE, MAP_SHARED, fd, data_start_address * 4096);
  sd->page_num = 0;
}

uint32_t Disk::WriteLog(uint32_t opcode, uint64_t node_a_id, uint64_t node_b_id){

  Log *lg = (Log*)mmap(NULL,sizeof(Log),PROT_READ|PROT_WRITE, MAP_SHARED, fd, cur_log_page*4096);

  uint32_t index = lg->entry_num;
  lg->entry[index].opcode = opcode;
  lg->entry[index].node_a_id = node_a_id;
  lg->entry[index].node_b_id = node_b_id;
  lg->generation = generation;
  lg -> entry_num ++;

  cout << "Writing to log, P: " << cur_log_page << " E: "  << lg->entry_num;
  cout << " Opt:" ;
  if(opcode == 0) cout << " add_node: ";
  else if(opcode == 1) cout << " add_edge: ";
  else if(opcode == 2) cout << " remove_node: ";
  else cout << " remove_edge: ";
  cout << node_a_id << " and " << node_b_id << endl;

  // If the block is full, then get next block, and set the entry_num 0.
  if(lg -> entry_num == 170){
    cur_log_page ++;

    if(cur_log_page == 1024*511) {
        //full 507//
        return 507;
    }

    Log *nlg = (Log*)mmap(NULL,sizeof(Log),PROT_READ|PROT_WRITE, MAP_SHARED, fd, cur_log_page*4096);
    nlg -> entry_num = 0;
    nlg -> generation = lg -> generation;

  }
  return 200;
}

void Disk::ReadLog(Graph & graph){

  cout << "Recovering from Log.\n";

  SupLog *slg = (SupLog*)mmap(NULL,sizeof(SupLog),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  while(true){
    Log *lg = (Log*)mmap(NULL,sizeof(Log),PROT_READ|PROT_WRITE, MAP_SHARED, fd, cur_log_page * 4096);

    if(lg -> generation != slg -> generation){
      cur_log_page --;
      if(cur_log_page == 0) cur_log_page = 1;
      return;
    }
    else{
      for(uint32_t i = 0 ; i < lg -> entry_num; i++){

        cout << "Recovery from log, P: " << cur_log_page << " E: "  << i+1;
        cout << " Opt:" ;
        if(lg->entry[i].opcode == 0) cout << " add_node: ";
        else if(lg->entry[i].opcode == 1) cout << " add_edge: ";
        else if(lg->entry[i].opcode == 2) cout << " remove_node: ";
        else cout << " remove_edge: ";
        cout << lg -> entry[i].node_a_id << " and " << lg -> entry[i].node_b_id << endl;

        if(lg -> entry[i].opcode == 0){
            graph.add_node(lg -> entry[i].node_a_id);
        }
        else if(lg -> entry[i].opcode == 1){
            graph.add_edge(lg -> entry[i].node_a_id, lg -> entry[i].node_b_id);
        }
        else if(lg -> entry[i].opcode == 2){
            graph.remove_node(lg -> entry[i].node_a_id);
        }
        else if(lg -> entry[i].opcode == 3){
            graph.remove_edge(lg -> entry[i].node_a_id, lg -> entry[i].node_b_id);
        }
      }
      cur_log_page ++;
    }
  }
}

void Disk::Print(){
  Log *lg = (Log*)mmap(NULL,sizeof(Log),PROT_READ|PROT_WRITE, MAP_SHARED, fd, cur_log_page*4096);

  for(uint32_t i = 0; i < lg->entry_num; i++){
    cout << lg->entry[i].opcode << " " << lg->entry[i].node_a_id << " " << lg->entry[i].node_b_id << endl;
  }

}

void Disk::ReadCheckPoint(Graph &graph){

  cout << "Recovering from CheckPoint. \n" << endl;
  SupData *sd = (SupData*)mmap(NULL,sizeof(SupData),PROT_READ|PROT_WRITE, MAP_SHARED, fd, data_start_address * 4096);

  for(uint32_t i = 1; i <= sd -> page_num ; i++){
    Data* data = (Data*)mmap(NULL,sizeof(Data),PROT_READ|PROT_WRITE, MAP_SHARED, fd, (data_start_address + i) * 4096);
    for(uint32_t j = 0 ; j <= data -> edge_num ; j++){

      cout << "Recovering Edge : " << "< " << data -> edge[j].node_a_id << " , "
      << data -> edge[j].node_b_id << " >" << endl;

      uint64_t node_a_id = data -> edge[j].node_a_id;
      uint64_t node_b_id = data -> edge[j].node_b_id;
      graph.add_node(node_a_id);
      graph.add_node(node_b_id);
      graph.add_edge(node_a_id, node_b_id);

    }
  }
}

uint32_t Disk::CheckPoint(Graph &graph){
  cout << "Storing to CheckPoint. \nCurrent generation: " << generation << endl;

  vector<pair<uint64_t, uint64_t>> all_edge = graph.get_all_edge();
  uint64_t edge_size = all_edge.size();
  uint32_t k = 0;
  SupData *sd = (SupData*)mmap(NULL,sizeof(SupData),PROT_READ|PROT_WRITE, MAP_SHARED, fd, data_start_address*4096);

  if(edge_size >= (2097150*255)){
    //Full
    return 507;
  }

  for(uint32_t i = 1 ; i < 2 * 1024 * 10; i++){
    Data* data = (Data*)mmap(NULL,sizeof(Data),PROT_READ|PROT_WRITE, MAP_SHARED, fd, (data_start_address + i) * 4096);
    for(uint32_t j = 0 ; j < 255 ; j++){
      if(k == edge_size){
        data -> edge_num = j-1;
        Format();
        sd -> page_num = i;
        sd -> checksum = ((sd -> page_num) ^ 1771);
        return 200;
      }
      cout << "Storing Edge : " << "< " << all_edge[k].first<< " , " <<
      all_edge[k].second << " >" << endl;

      data -> edge[j].node_a_id = all_edge[k].first;
      data -> edge[j].node_b_id = all_edge[k].second;
      k++;
    }
    data -> edge_num = 255;
  }
  return 507;
}

bool Disk::isCheckPointValid(){
  Init();
  SupData *sd = (SupData*)mmap(NULL,sizeof(SupData),PROT_READ|PROT_WRITE, MAP_SHARED, fd, data_start_address * 4096);
  if(sd -> checksum == ((sd -> page_num) ^ 1771)){
    return true;
  }
  else return false;
}

bool Disk::isLogValid(){
  Init();
  SupLog *sg = (SupLog*)mmap(NULL,sizeof(SupLog),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  generation = sg -> generation;
  if(sg -> checksum == ((sg -> start_address ) ^ (sg -> generation) ^ (sg -> block_num) ^ 1771)){
    return true;
  }
  else return false;
}
