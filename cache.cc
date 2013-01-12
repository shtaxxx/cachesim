#include "cache.hh"

#include <cassert>

/******************************************************************************/
CacheLine::CacheLine() :
  valid(false), dirty(false), access_time(0), tag(0)
{
}

bool CacheLine::is_valid()
{
  return valid;
}

bool CacheLine::is_dirty()
{
  return valid && dirty;
}

ulong CacheLine::get_access_time()
{
  return access_time;
}

tag_t CacheLine::get_tag()
{
  return tag;
}

addr_t CacheLine::get_next_addr()
{
  return next_addr;
}

void CacheLine::set_dirty()
{
  this->dirty = true;
}

void CacheLine::set_access_time(ulong time)
{
  this->access_time = time;
}

void CacheLine::set_next_addr(addr_t addr)
{
  this->next_addr = addr;
}

bool CacheLine::is_hit(tag_t _tag)
{
  if(! this->valid) return false;
  //cout << "this->tag" << this->tag << "_tag:" << _tag << endl;
  if(this->tag == _tag) return true;
  return false;
}

void CacheLine::invalidate()
{
  this->valid = false;
  this->dirty = false;
}

void CacheLine::fill(tag_t _tag)
{
  this->tag = _tag;
  this->valid = true;
  this->dirty = false;
}

/******************************************************************************/
Cache::Cache() :
  nways(DEFAULT_NWAYS), nsets(DEFAULT_NSETS),
  nlines(DEFAULT_NWAYS * DEFAULT_NSETS),
  current_time(0), outstanding_misses(0),
  access_cnt(0), read_cnt(0), write_cnt(0),
  hit_cnt(0), miss_cnt(0),
  hit_read_cnt(0), miss_read_cnt(0),
  hit_write_cnt(0), miss_write_cnt(0)
{
  initialize();
}

Cache::Cache(int _nways, uint _nsets) :
  nways(_nways), nsets(_nsets), nlines(nways*nsets), 
  current_time(0), outstanding_misses(0),
  access_cnt(0), read_cnt(0), write_cnt(0),
  hit_cnt(0), miss_cnt(0),
  hit_read_cnt(0), miss_read_cnt(0),
  hit_write_cnt(0), miss_write_cnt(0)
{
  initialize();
}

void Cache::initialize()
{
  cache_lines = new vector<CacheLine*>(nlines);
  state = new vector<CacheLineState>(nlines);
  read_request = new map<addr_t, Request>();
  write_request = new map<addr_t, Request>();
  read_address_map = new map<addr_t, addr_t>();
  write_address_map = new map<addr_t, addr_t>();
  for(uint i=0; i<nlines; i++){
    cache_lines->at(i) = new CacheLine();
  }
  for(uint i=0; i<nlines; i++){
    state->at(i) = IDLE;
  }
}

Cache::~Cache()
{
  finalize();
}

void Cache::finalize()
{
  for(uint i=0; i<nlines; i++){
    delete cache_lines->at(i);
  }
  delete cache_lines;
  delete state;
  delete read_request;
  delete write_request;
  delete read_address_map;
  delete write_address_map;
}

/******************************************************************************/
inline uint Cache::to_index(addr_t addr)
{
  return (addr / LINE_SIZE) % nsets;
}

inline tag_t Cache::to_tag(addr_t addr){
  return (addr / LINE_SIZE) / nsets;
}

inline addr_t Cache::index_tag_to_addr(uint index, tag_t tag)
{
  return (tag * nsets + index) * LINE_SIZE;
}

inline addr_t Cache::masked_addr(addr_t addr)
{
  return index_tag_to_addr(to_index(addr), to_tag(addr));
}

inline uint Cache::to_listindex(uint index, int way)
{
  return index * (uint)nways + (uint) way;
}

/******************************************************************************/
int Cache::find_hit_way(uint index, tag_t tag)
{
  for(int way=0; way<nways; way++){
    if(cache_lines->at(to_listindex(index, way))->is_hit(tag)){
      return way;
    }
  }
  return -1;
}

int Cache::find_replaced_way(uint index) // LRU
{
  ulong min_time;
  int ret_way = 0;
  for(int way=0; way<nways; way++){
    uint listindex = to_listindex(index, way);
    if(! cache_lines->at(listindex)->is_valid()){
      return way;
    }
    ulong time = cache_lines->at(listindex)->get_access_time();
    if(way==0){ min_time = time; ret_way = 0; }
    if(min_time > time){ min_time = time; ret_way = way; }
  }
  return ret_way;
}

/******************************************************************************/
void Cache::set_state_idle(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  state->at(listindex) = IDLE;
}

void Cache::set_state_read(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  state->at(listindex) = READING;
}

void Cache::set_state_write(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  state->at(listindex) = WRITING;
}

bool Cache::is_state_idle(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  return state->at(listindex);
}

bool Cache::is_state_read(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  return state->at(listindex);
}

bool Cache::is_state_write(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  return state->at(listindex);
}

bool Cache::is_line_busy(uint index)
{
  for(int way=0; way<nways; way++){
    if(! is_state_idle(index, way)) return true;
  }
  return false;
}

/******************************************************************************/
bool Cache::is_valid(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  return cache_lines->at(listindex)->is_valid();
}

bool Cache::is_dirty(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  return cache_lines->at(listindex)->is_dirty();
}

tag_t Cache::get_tag(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  return cache_lines->at(listindex)->get_tag();
}

addr_t Cache::get_next_addr(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  return cache_lines->at(listindex)->get_next_addr();
}

ulong Cache::get_access_time(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  return cache_lines->at(listindex)->get_access_time();
}

void Cache::set_dirty(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  cache_lines->at(listindex)->set_dirty();
}

void Cache::set_next_addr(uint index, int way, addr_t addr)
{
  uint listindex = to_listindex(index, way);
  cache_lines->at(listindex)->set_next_addr(addr);
}

void Cache::set_access_time(uint index, int way, ulong access_time)
{
  uint listindex = to_listindex(index, way);
  cache_lines->at(listindex)->set_access_time(access_time);
}

void Cache::invalidate(uint index, int way)
{
  uint listindex = to_listindex(index, way);
  cache_lines->at(listindex)->invalidate();
}

void Cache::fill(uint index, int way, tag_t tag)
{
  //cout << "index:" << index << "way:" << way << "tag:" << tag << endl;
  uint listindex = to_listindex(index, way);
  cache_lines->at(listindex)->fill(tag);
}

/******************************************************************************/
void Cache::add_read_request(addr_t addr, Request req)
{
  (*read_request)[addr] = req;
}

void Cache::add_write_request(addr_t addr, Request req)
{
  (*write_request)[addr] = req;
}

void Cache::remove_read_request(addr_t addr)
{
  map<addr_t, Request>::iterator it = read_request->find(addr);
  if(it == read_request->end()) return;
  read_request->erase(it);
}

void Cache::remove_write_request(addr_t addr)
{
  map<addr_t, Request>::iterator it = write_request->find(addr);
  if(it == write_request->end()) return;
  write_request->erase(it);
}

/******************************************************************************/
void Cache::set_read_address_map(addr_t src, addr_t dst)
{
  (*read_address_map)[src] = dst;
}

void Cache::set_write_address_map(addr_t src, addr_t dst)
{
  (*write_address_map)[src] = dst;
}

addr_t Cache::get_read_address_map(addr_t src)
{
  return read_address_map->at(src);
}

addr_t Cache::get_write_address_map(addr_t src)
{
  return write_address_map->at(src);
}

/******************************************************************************/
bool Cache::busy()
{
  if(outstanding_misses >= MAX_OUTSTANDING_MISSES) return true;
  return false;
}

void Cache::read(Request req)
{
  access_cnt++;
  read_cnt++;
  uint index = to_index(req.addr);
  tag_t tag = to_tag(req.addr);
  int way = find_hit_way(index, tag);
  if(way < 0){
    miss_cnt++;
    miss_read_cnt++;
    int replaced_way = find_replaced_way(index);
    if(is_dirty(index, replaced_way)){ //dirty to write
      set_state_write(index, replaced_way);
      set_next_addr(index, replaced_way, index_tag_to_addr(index, tag));
      Request new_req;
      new_req.init();
      new_req.type = REQ_READ;
      new_req.time = current_time;
      addr_t hash_addr = index_tag_to_addr(index, get_tag(index, replaced_way));
      new_req.addr = hash_addr;
      new_req.way = replaced_way;
      set_lo_write(new_req);
      add_write_request(hash_addr, new_req);
      set_read_address_map(index_tag_to_addr(index, tag), req.addr);
#ifdef DUMP_MISS
      cout << "Read Dirty:" << req.addr << ":" << hash_addr << endl;
#endif
    }
    else{ //clean to read
      set_state_read(index, replaced_way);
      Request new_req;
      new_req.init();
      new_req.type = REQ_READ;
      new_req.time = current_time;
      addr_t hash_addr = index_tag_to_addr(index, tag);
      new_req.addr = hash_addr;
      new_req.way = replaced_way;
      set_lo_read(new_req);
      add_read_request(hash_addr, new_req);
      set_read_address_map(hash_addr, req.addr);
#ifdef DUMP_MISS
      cout << "Read Clean:" << req.addr << ":" << hash_addr << endl;
#endif
    }
    invalidate(index, replaced_way);
    outstanding_misses++;
  }
  else{
    hit_cnt++;
    hit_read_cnt++;
    set_access_time(index, way, current_time);
    set_up_read(req);
  }
}

void Cache::write(Request req)
{
  access_cnt++;
  write_cnt++;
  uint index = to_index(req.addr);
  tag_t tag = to_tag(req.addr);
  int way = find_hit_way(index, tag);
  if(way < 0){
    miss_cnt++;
    miss_write_cnt++;
    int replaced_way = find_replaced_way(index);
    if(is_dirty(index, replaced_way)){ //dirty to write
      set_state_write(index, replaced_way);
      set_next_addr(index, replaced_way, index_tag_to_addr(index, tag));
      Request new_req;
      new_req.init();
      new_req.type = REQ_WRITE;
      new_req.time = current_time;
      addr_t hash_addr = index_tag_to_addr(index, get_tag(index, replaced_way));
      new_req.addr = hash_addr;
      new_req.way = replaced_way;
      set_lo_write(new_req);
      add_write_request(hash_addr, new_req);
      set_write_address_map(index_tag_to_addr(index, tag), req.addr);
#ifdef DUMP_MISS
      cout << "Write Dirty:" << req.addr << ":" << hash_addr << endl;
#endif
    }
    else{ //clean to read
      set_state_read(index, replaced_way);
      Request new_req;
      new_req.init();
      new_req.type = REQ_WRITE;
      new_req.time = current_time;
      addr_t hash_addr = index_tag_to_addr(index, tag);
      new_req.addr = hash_addr;
      new_req.way = replaced_way;
      set_lo_read(new_req);
      add_read_request(hash_addr, new_req);
      set_write_address_map(hash_addr, req.addr);
#ifdef DUMP_MISS
      cout << "Write Clean:" << req.addr << ":" << hash_addr << endl;
#endif
    }
    invalidate(index, replaced_way);
    outstanding_misses++;
  }
  else{
    hit_cnt++;
    hit_write_cnt++;
    set_access_time(index, way, current_time);
    set_dirty(index, way);
    set_up_write(req);
  }
}

void Cache::read_done(Request req)
{
  addr_t addr = req.addr;
  int way = req.way;
  uint index = to_index(addr);
  tag_t tag = to_tag(addr);
  fill(index, way, tag);
  if(req.type == REQ_READ){
    Request new_req = req;
    new_req.addr = get_read_address_map(req.addr);
    set_up_read(new_req);
#ifdef DUMP_MISS
    cout << "Read Done:" << new_req.addr << endl;
#endif
  }else if(req.type == REQ_WRITE){
    Request new_req = req;
    new_req.addr = get_write_address_map(req.addr);
    set_up_write(new_req);
    set_dirty(index, way);
#ifdef DUMP_MISS
    cout << "Write Done:" << new_req.addr << endl;
#endif
  }
  set_access_time(index, way, current_time);
  remove_read_request(addr);
  outstanding_misses--;
}

void Cache::write_done(Request req)
{
  addr_t addr = req.addr;
  int way = req.way;
  uint index = to_index(addr);
  addr_t next_addr = get_next_addr(index, way);
  remove_write_request(addr);
  Request new_req = req;
  new_req.time = req.time;
  new_req.addr = next_addr;
  new_req.way = way;
  set_lo_read(new_req);
  addr_t hash_addr = index_tag_to_addr(index, get_tag(index, way));
  add_read_request(hash_addr, new_req);
#ifdef DUMP_MISS
  cout << "Next Read:" << new_req.addr << endl;
#endif
}

void Cache::step()
{
  current_time++;
}

ulong Cache::get_access_cnt()
{
  return access_cnt;
}
ulong Cache::get_read_cnt()
{
  return read_cnt;
}
ulong Cache::get_write_cnt()
{
  return write_cnt;
}

ulong Cache::get_hit_cnt()
{
  return hit_cnt;
}
ulong Cache::get_miss_cnt()
{
  return miss_cnt;
}
ulong Cache::get_hit_read_cnt()
{
  return hit_read_cnt;
}
ulong Cache::get_miss_read_cnt()
{
  return miss_read_cnt;
}
ulong Cache::get_hit_write_cnt()
{
  return hit_write_cnt;
}
ulong Cache::get_miss_write_cnt()
{
  return miss_write_cnt;
}
