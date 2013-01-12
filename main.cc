#include "param.hh"
#include "vpi_memory.hh"

#include <cassert>

int main(int argc, char* argv[])
{
  VpiMemory* vpi_memory = new VpiMemory();
  
  //const int max_cycle = 1024 * 128;
  const int size = 16 * 1024;
  int inst_cnt = 0;
  addr_t read_addr = 1024 * 8;
  addr_t write_addr = 1024 * 16;
  bool mode = false;
  data_t wd = 0;

  int same_mode_cnt = 0;
  
  int cycle = 0;
  //for(int cycle=0; cycle<max_cycle; cycle++){
  while(inst_cnt < size){
    cout << "cycle: " << cycle << endl;
    if(mode){ //read
      bool busy = vpi_memory->read_busy();
      if(!busy){
        data_t d;
        bool done = vpi_memory->read(read_addr, 4, &d);
        cout << "# READ REQ: " << read_addr << endl;
        if(done){
          cout << "# READ DONE: " << d << endl;
          mode = false;
          //read_addr += LINE_SIZE * 16;
          read_addr += 4;
          inst_cnt ++;
          same_mode_cnt = 0;
        }
      }
      same_mode_cnt++;
    }else{ //write
      bool busy = vpi_memory->write_busy();
      if(!busy){
        bool done = vpi_memory->write(write_addr, 4, wd);
        cout << "# WRITE REQ: " << write_addr << ": " << wd << endl;
        if(done){
          cout << "# WRITE DONE" << endl;
          mode = true;
          //write_addr += LINE_SIZE * 16;
          write_addr += 4;
          wd = (data_t) cycle;
          inst_cnt ++;
          same_mode_cnt = 0;
        }
      }
      same_mode_cnt++;
    }
    assert(same_mode_cnt < 80);
    vpi_memory->step();
    cycle++;
  }

  cout << "Access     : " << vpi_memory->get_access_cnt() << endl;
  cout << "Read       : " << vpi_memory->get_read_cnt() << endl;
  cout << "Write      : " << vpi_memory->get_write_cnt() << endl;
  cout << "Hit        : " << vpi_memory->get_hit_cnt() << endl;
  cout << "Miss       : " << vpi_memory->get_miss_cnt() << endl;
  cout << "Read Hit   : " << vpi_memory->get_hit_read_cnt() << endl;
  cout << "Read Miss  : " << vpi_memory->get_miss_read_cnt() << endl;
  cout << "Write Hit  : " << vpi_memory->get_hit_write_cnt() << endl;
  cout << "Write Miss : " << vpi_memory->get_miss_write_cnt() << endl;

  delete vpi_memory;

  return 0;
}
