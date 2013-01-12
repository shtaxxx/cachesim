#ifndef VPI_MEMORY_HH
#define VPI_MEMORY_HH

#include "param.hh"
#include "memoryinterface.hh"

class VpiMemory
{
private:
  MemoryInterface* memory_interface;
  bool read_wait;
  bool write_wait;
  addr_t last_read_addr;
  addr_t last_write_addr;

public:
  VpiMemory();
  VpiMemory(uint size, int nways, uint nsets);

  bool read(addr_t addr, uint size, data_t* rdata);
  bool read_busy();
  bool write(addr_t addr, uint size, data_t& wdata);
  bool write_busy();
  void step();

  ulong get_access_cnt();
  ulong get_read_cnt();
  ulong get_write_cnt();
  ulong get_hit_cnt();
  ulong get_miss_cnt();
  ulong get_hit_read_cnt();
  ulong get_miss_read_cnt();
  ulong get_hit_write_cnt();
  ulong get_miss_write_cnt();  
};

#endif
