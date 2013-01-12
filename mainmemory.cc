#include "mainmemory.hh"

MainMemory::MainMemory()
{
  this->read_queue = new list<Request>();
  this->write_queue = new list<Request>();
}

void MainMemory::consume_next_request()
{
  if(cmd_state.cmd == CMD_READ && ! cmd_state.done()){
  }else if(cmd_state.cmd == CMD_WRITE && ! cmd_state.done()){
  }else if(cmd_state.cmd == CMD_READ && cmd_state.done()){
    cmd_state.cmd = CMD_NONE;
    set_up_read(cmd_state.request);
  }else if(cmd_state.cmd == CMD_WRITE && cmd_state.done()){
    cmd_state.cmd = CMD_NONE;
    set_up_write(cmd_state.request);
  }else if(!up_read_exists() && !up_write_exists()){
    if(! read_queue->empty()){
      cmd_state.request = *(read_queue->begin());
      cmd_state.cycle = MEMORY_LATENCY;
      cmd_state.cmd = CMD_READ;
      read_queue->pop_front();
#ifdef DUMP_MEM
      cout << "memory read:" << cmd_state.request.addr << endl;
#endif
    }else if(! write_queue->empty()){
      cmd_state.request = *(write_queue->begin());
      cmd_state.cycle = MEMORY_LATENCY;
      cmd_state.cmd = CMD_WRITE;
      write_queue->pop_front();
#ifdef DUMP_MEM
      cout << "memory write:" << cmd_state.request.addr << endl;
#endif
    }
  }
}

/******************************************************************************/
bool MainMemory::busy()
{
  return false;
}

void MainMemory::read(Request req)
{
  read_queue->push_back(req);
}

void MainMemory::write(Request req)
{
  write_queue->push_back(req);
}

void MainMemory::read_done(Request req)
{
}

void MainMemory::write_done(Request req)
{
}

void MainMemory::step()
{
  cmd_state.step();
  consume_next_request();
}
