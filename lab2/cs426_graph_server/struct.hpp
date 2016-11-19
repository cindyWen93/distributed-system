#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <stdint.h>

struct SupLog{
  uint32_t generation;
  uint64_t checksum;
  uint32_t start_address;
  uint32_t block_num;
};

struct Entry{
  uint32_t opcode;
  uint64_t node_a_id;
  uint64_t node_b_id;
};

struct Log{
  uint32_t generation;
  uint32_t entry_num;
  uint64_t checksum;
  struct Entry entry[170];
};

struct SupData{
  uint64_t checksum;
  uint64_t page_num;
};

struct Edge{
  uint64_t node_a_id;
  uint64_t node_b_id;
};

struct Data{
  uint32_t edge_num;
  struct Edge edge[255];
};

#endif
