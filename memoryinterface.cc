#include "memoryinterface.hh"

using namespace std;

/******************************************************************************/
MemoryInterface::MemoryInterface() :
  current_time(0), cache_nways(DEFAULT_NWAYS), cache_nsets(DEFAULT_NSETS)
{
  data_array = new DataArray();
  cache = new Cache();
  main_memory = new MainMemory();
  read_request = new list<addr_t>();
  write_request = new list<addr_t>();
}

MemoryInterface::MemoryInterface(uint size, int nways, uint nsets) :
  current_time(0),  cache_nways(nways), cache_nsets(nsets)
{
  data_array = new DataArray(size);
  cache = new Cache(nways, nsets);
  main_memory = new MainMemory();
  read_request = new list<addr_t>();
  write_request = new list<addr_t>();
}

/******************************************************************************/
inline ulong MemoryInterface::to_index(addr_t addr)
{
  return (addr / LINE_SIZE) % cache_nsets;
}

/******************************************************************************/
void MemoryInterface::add_read_request(addr_t addr)
{
  list<addr_t>::iterator it = find(read_request->begin(), read_request->end(), addr);
  if(it == read_request->end()){
    read_request->push_back(addr);
  }
}

void MemoryInterface::add_write_request(addr_t addr)
{
  list<addr_t>::iterator it = find(write_request->begin(), write_request->end(), addr);
  if(it == write_request->end()){
    write_request->push_back(addr);
  }
}

bool MemoryInterface::read_request_exists(addr_t addr)
{
  //list<addr_t>::iterator it = find(read_request->begin(), read_request->end(), addr);
  //if(it != read_request->end()) return true;
  //return false;

  for(list<addr_t>::iterator it = read_request->begin(); it != read_request->end(); it++){
    if(to_index(*it) == to_index(addr)) return true;
  }
  return false;

  //return !read_request->empty();
}

bool MemoryInterface::write_request_exists(addr_t addr)
{
  //list<addr_t>::iterator it = find(write_request->begin(), write_request->end(), addr);
  //if(it != write_request->end()) return true;
  //return false;
  
  for(list<addr_t>::iterator it = write_request->begin(); it != write_request->end(); it++){
    if(to_index(*it) == to_index(addr)) return true;
  }
  return false;

  //return !write_request->empty();
}

void MemoryInterface::remove_read_request(addr_t addr)
{
  list<addr_t>::iterator it = find(read_request->begin(), read_request->end(), addr);
  if(it != read_request->end()){
    read_request->erase(it);
  }
}

void MemoryInterface::remove_write_request(addr_t addr)
{
  list<addr_t>::iterator it = find(write_request->begin(), write_request->end(), addr);
  if(it != write_request->end()){
    write_request->erase(it);
  }
}

/******************************************************************************/
void MemoryInterface::vpi_read_request(addr_t addr)
{
  Request req;
  req.init();
  req.addr = addr;
  req.time = current_time;
  req.type = REQ_READ;
  read(req);
}

bool MemoryInterface::vpi_read_busy(addr_t addr)
{
  if(read_request_exists(addr)) return true;
  if(write_request_exists(addr)) return true;
  return false;
}

void MemoryInterface::vpi_get_read_data(addr_t addr, uint size, data_t* rdata)
{
  *rdata = data_array->read(addr, size);
  //cout << "## MI read_data" << *rdata << endl;
}

void MemoryInterface::vpi_write_request(addr_t addr, uint size, data_t& wdata)
{
  data_array->write(addr, size, wdata);
  Request req;
  req.init();
  req.addr = addr;
  req.time = current_time;
  req.type = REQ_WRITE;
  write(req);
}

bool MemoryInterface::vpi_write_busy(addr_t addr)
{
  if(read_request_exists(addr)) return true;
  if(write_request_exists(addr)) return true;
  return false;
}

/******************************************************************************/
bool MemoryInterface::busy()
{
  return cache->busy();
}

void MemoryInterface::read(Request req)
{
  add_read_request(req.addr);
  set_lo_read(req);
}

void MemoryInterface::write(Request req)
{
  add_write_request(req.addr);
  set_lo_write(req);
}

void MemoryInterface::read_done(Request req)
{
  addr_t addr = req.addr;
  remove_read_request(addr);
}

void MemoryInterface::write_done(Request req)
{
  addr_t addr = req.addr;
  remove_write_request(addr);
}

void MemoryInterface::step()
{
  current_time++;
}

/******************************************************************************/
void MemoryInterface::step_all()
{
  if(lo_read_exists()){
    cache->read(get_lo_read());
  }else if(lo_write_exists()){
    cache->write(get_lo_write());
  }

  if(cache->lo_read_exists()){
    main_memory->read(cache->get_lo_read());
  }else if(cache->lo_write_exists()){
    main_memory->write(cache->get_lo_write());
  }

  if(main_memory->up_read_exists()){
    cache->read_done(main_memory->get_up_read());
  }else if(main_memory->up_write_exists()){
    cache->write_done(main_memory->get_up_write());
  }

  if(cache->up_read_exists()){
    read_done(cache->get_up_read());
  }else if(cache->up_write_exists()){
    write_done(cache->get_up_write());
  }

  step();
  cache->step();
  main_memory->step();
}

ulong MemoryInterface::get_access_cnt()
{
  return cache->get_access_cnt();
}
ulong MemoryInterface::get_read_cnt()
{
  return cache->get_read_cnt();
}
ulong MemoryInterface::get_write_cnt()
{
  return cache->get_write_cnt();
}
ulong MemoryInterface::get_hit_cnt()
{
  return cache->get_hit_cnt();
}
ulong MemoryInterface::get_miss_cnt()
{
  return cache->get_miss_cnt();
}
ulong MemoryInterface::get_hit_read_cnt()
{
  return cache->get_hit_read_cnt();
}
ulong MemoryInterface::get_miss_read_cnt()
{
  return cache->get_miss_read_cnt();
}
ulong MemoryInterface::get_hit_write_cnt()
{
  return cache->get_hit_write_cnt();
}
ulong MemoryInterface::get_miss_write_cnt()
{
  return cache->get_miss_write_cnt();
}

