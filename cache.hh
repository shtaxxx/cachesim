#ifndef CACHE_HH
#define CACHE_HH

#include "param.hh"
#include "accessinterface.hh"
#include <vector>
#include <map>

using namespace std;

enum CacheLineState { IDLE, WRITING, READING };

/******************************************************************************/
class CacheLine
{
private:
  bool valid;
  bool dirty;
  ulong access_time;
  tag_t tag;
  addr_t next_addr;
public:
  CacheLine();

  bool is_valid();
  bool is_dirty();
  ulong get_access_time();
  tag_t get_tag();
  addr_t get_next_addr();

  void set_dirty();
  void set_access_time(ulong time);
  void set_next_addr(addr_t addr);

  bool is_hit(tag_t _tag);
  void invalidate();
  void fill(tag_t _tag);

  void step();
};

/******************************************************************************/
class Cache : public AccessInterface
{
private:
  int nways;
  uint nsets;
  unsigned int nlines;

  ulong current_time;
  uint outstanding_misses;

  vector<CacheLine*>* cache_lines;
  vector<CacheLineState>* state;

  map<addr_t, Request>* read_request;
  map<addr_t, Request>* write_request;

  map<addr_t, addr_t>* read_address_map;
  map<addr_t, addr_t>* write_address_map;

  void initialize();
  void finalize();

  inline uint to_index(addr_t addr);
  inline tag_t to_tag(addr_t addr);
  inline addr_t index_tag_to_addr(uint index, tag_t tag);
  inline addr_t masked_addr(addr_t addr);
  inline uint to_listindex(uint index, int way);

  int find_hit_way(uint index, tag_t tag);
  int find_replaced_way(uint index);

  void set_state_idle(uint index, int way);
  void set_state_read(uint index, int way);
  void set_state_write(uint index, int way);
  bool is_state_idle(uint index, int way);
  bool is_state_read(uint index, int way);
  bool is_state_write(uint index, int way);

  bool is_line_busy(uint index);

  bool is_valid(uint index, int way);
  bool is_dirty(uint index, int way);
  tag_t get_tag(uint index, int way);
  addr_t get_next_addr(uint index, int way);
  ulong get_access_time(uint index, int way);

  void set_dirty(uint index, int way);
  void set_next_addr(uint index, int way, addr_t addr);
  void set_access_time(uint index, int way, ulong access_time);
  void invalidate(uint index, int way);
  void fill(uint index, int way, tag_t tag);

  void add_read_request(addr_t addr, Request req);
  void add_write_request(addr_t addr, Request req);
  void remove_read_request(addr_t addr);
  void remove_write_request(addr_t addr); 

  void set_read_address_map(addr_t src, addr_t dst);
  void set_write_address_map(addr_t src, addr_t dst);
  addr_t get_read_address_map(addr_t src);
  addr_t get_write_address_map(addr_t src);

  ulong access_cnt;
  ulong read_cnt;
  ulong write_cnt;
  ulong hit_cnt;
  ulong miss_cnt;
  ulong hit_read_cnt;
  ulong miss_read_cnt;
  ulong hit_write_cnt;
  ulong miss_write_cnt;  

public:
  Cache();
  Cache(int _nways, uint _nsets);
  ~Cache();
  bool busy();
  void read(Request req);
  void write(Request req);
  void read_done(Request req);
  void write_done(Request req);
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
