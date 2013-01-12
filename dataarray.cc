#include "dataarray.hh"
#include <iostream>
using namespace std;

DataArray::DataArray() :
  memorysize(MEMSIZE)
{
  memory = new vector<byte_t>(MEMSIZE, 0);
}

DataArray::DataArray(uint size) :
  memorysize(size)
{
  memory = new vector<byte_t>(size, 0);
}

DataArray::~DataArray()
{
  delete memory;
}

data_t DataArray::read(addr_t addr, uint size)
{
  data_t read_data = 0;
  if(addr >= memorysize - size) return read_data;
  uint _size = size;
  if(size > 8) _size = 8;
  for(uint i=0; i<_size; i++){
    read_data |= ((data_t)memory->at(addr+i) & 0xff) << (i*8);
  }
  //cout << "### read_data" << read_data << endl;
  return read_data;
}

void DataArray::write(addr_t addr, uint size, data_t wdata)
{
  if(addr >= memorysize - size) return;
  uint _size = size;
  if(size > 8) _size = 8;
  for(uint i=0; i<_size; i++){
    memory->at(addr+i) = (byte_t)((wdata >> (i*8)) & 0xff);
  }
}
