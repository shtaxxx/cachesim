#ifndef MAINMEMORY_HH
#define MAINMEMORY_HH

#include "param.hh"
#include "accessinterface.hh"
#include <list>
#include <map>

using namespace std;

class MainMemory : public AccessInterface
{
private:
  list<Request>* read_queue;
  list<Request>* write_queue;
  DramCmdState cmd_state;
  void consume_next_request();
public:
  MainMemory();
  bool busy();
  void read(Request req);
  void write(Request req);
  void read_done(Request req);
  void write_done(Request req);
  void step();
};

#endif
