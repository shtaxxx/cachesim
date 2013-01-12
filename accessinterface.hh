#ifndef ACCESSINTERFACE_HH
#define ACCESSINTERFACE_HH

#include "param.hh"
#include <iostream>

using namespace std;

class AccessInterface
{
private:
  //Upper Module
  bool up_read;
  bool up_write;
  Request up_read_request;
  Request up_write_request;
  //Lower Module
  bool lo_read;
  bool lo_write;
  Request lo_read_request;
  Request lo_write_request;

protected:
  //Upper Module
  void set_up_read(Request request);
  void set_up_write(Request request);
  //Lower Module
  void set_lo_read(Request request);
  void set_lo_write(Request request);

public:
  AccessInterface();
  bool up_read_exists();
  bool up_write_exists();
  Request get_up_read();
  Request get_up_write();

  bool lo_read_exists();
  bool lo_write_exists();
  Request get_lo_read();
  Request get_lo_write();

  virtual bool busy() = 0;
  //add a request
  virtual void read(Request req) = 0;
  virtual void write(Request req) = 0;
  //notification of request completion
  virtual void read_done(Request req) = 0;
  virtual void write_done(Request req) = 0;
  virtual void step() = 0;
};

#endif
