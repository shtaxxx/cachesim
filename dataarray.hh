#ifndef DATAARRAY_HH
#define DATAARRAY_HH

#include "param.hh"
#include <vector>

using namespace std;

class DataArray
{
private:
  vector<byte_t>* memory;
  ulong memorysize;
public:
  DataArray();
  DataArray(uint size);
  ~DataArray();
  data_t read(addr_t addr, uint size);
  void write(addr_t addr, uint size, data_t wdata);
  void step();
};

#endif
