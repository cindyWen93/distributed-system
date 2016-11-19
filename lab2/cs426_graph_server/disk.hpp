#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include "graph.h"

class Disk{
private:
  uint32_t fd;
  uint32_t cur_log_page;
  uint32_t start_address;
  uint32_t block_num;
  uint32_t data_start_address;
  uint32_t generation;

public:
  void Open(uint32_t);
  void Init();
  void ReadLog(Graph &);
  void ReadCheckPoint(Graph &);
  bool isCheckPointValid();
  bool isLogValid();
  void Format();
  void Print();
  void FormatCheckPoint();
  void GetGeneration();
  uint32_t CheckPoint(Graph &);
  uint32_t WriteLog(uint32_t,uint64_t,uint64_t);

};

#endif
