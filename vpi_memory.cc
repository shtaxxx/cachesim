#include "vpi_memory.hh"

VpiMemory::VpiMemory() :
  read_wait(false), write_wait(false),
  last_read_addr(0), last_write_addr(0)
{
  memory_interface = new MemoryInterface();
}

VpiMemory::VpiMemory(uint size, int nways, uint nsets) :
  read_wait(false), write_wait(false),
  last_read_addr(0), last_write_addr(0)
{
  memory_interface = new MemoryInterface(size, nways, nsets);
}

bool VpiMemory::read(addr_t addr, uint size, data_t* rdata)
{
  if(!read_wait && !memory_interface->vpi_read_busy(addr)){
    memory_interface->vpi_read_request(addr);
    read_wait = true;
    last_read_addr = addr;
    return false;
  }
  else if(!memory_interface->vpi_read_busy(addr)){
    memory_interface->vpi_get_read_data(addr, size, rdata);
    //cout << "## vpi_memory readdata" << *rdata << endl;
    read_wait = false;
    return true;
  }
  return false;
}

bool VpiMemory::read_busy()
{
  if(!read_wait) return false;
  if(!memory_interface->vpi_read_busy(last_read_addr)) return false;
  return true;
}

bool VpiMemory::write(addr_t addr, uint size, data_t& wdata)
{
  if(!write_wait && !memory_interface->vpi_write_busy(addr)){
    memory_interface->vpi_write_request(addr, size, wdata);
    write_wait = true;
    last_write_addr = addr;
    return false;
  }
  else if(!memory_interface->vpi_write_busy(addr)){
    write_wait = false;
    return true;
  }
  return false;
}

bool VpiMemory::write_busy()
{
  if(!write_wait) return false;
  if(!memory_interface->vpi_write_busy(last_write_addr)) return false;
  return true;
}

void VpiMemory::step()
{
#ifdef DUMP
  //cout << "step" << endl;
#endif
  memory_interface->step_all();
}

ulong VpiMemory::get_access_cnt()
{
  return memory_interface->get_access_cnt();
}
ulong VpiMemory::get_read_cnt()
{
  return memory_interface->get_read_cnt();
}
ulong VpiMemory::get_write_cnt()
{
  return memory_interface->get_write_cnt();
}
ulong VpiMemory::get_hit_cnt()
{
  return memory_interface->get_hit_cnt();
}
ulong VpiMemory::get_miss_cnt()
{
  return memory_interface->get_miss_cnt();
}
ulong VpiMemory::get_hit_read_cnt()
{
  return memory_interface->get_hit_read_cnt();
}
ulong VpiMemory::get_miss_read_cnt()
{
  return memory_interface->get_miss_read_cnt();
}
ulong VpiMemory::get_hit_write_cnt()
{
  return memory_interface->get_hit_write_cnt();
}
ulong VpiMemory::get_miss_write_cnt()
{
  return memory_interface->get_miss_write_cnt();
}
