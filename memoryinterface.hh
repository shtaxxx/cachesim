#ifndef MEMORYINTERFACE_HH
#define MEMORYINTERFACE_HH

#include "param.hh"
#include "dataarray.hh"
#include "accessinterface.hh"
#include "cache.hh"
#include "mainmemory.hh"
#include <list>
#include <algorithm>

using namespace std;

class MemoryInterface : public AccessInterface
{
private:
  ulong current_time;
  int cache_nways;
  uint cache_nsets;

  DataArray* data_array;
  Cache* cache;
  MainMemory* main_memory;

  list<addr_t>* read_request;
  list<addr_t>* write_request;

  inline ulong to_index(addr_t addr);

  void add_read_request(addr_t addr);
  void add_write_request(addr_t addr);
  bool read_request_exists(addr_t addr);
  bool write_request_exists(addr_t addr);
  void remove_read_request(addr_t addr);
  void remove_write_request(addr_t addr);

public:
  MemoryInterface();
  MemoryInterface(uint size, int nways, uint nsets);

  void vpi_read_request(addr_t addr);
  bool vpi_read_busy(addr_t addr);
  void vpi_get_read_data(addr_t addr, uint size, data_t* rdata);
  void vpi_write_request(addr_t addr, uint size, data_t& wdata);
  bool vpi_write_busy(addr_t addr);

  bool busy();
  void read(Request req);
  void write(Request req);
  void read_done(Request req);
  void write_done(Request req);
  void step();

  void step_all();

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
