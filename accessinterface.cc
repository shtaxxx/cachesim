#include "accessinterface.hh"

/******************************************************************************/
AccessInterface::AccessInterface()
{
  up_read = false;
  up_write = false;
  up_read_request.init();
  up_write_request.init();
  lo_read = false;
  lo_write = false;
  lo_read_request.init();
  lo_write_request.init();
}

/******************************************************************************/
void AccessInterface::set_up_read(Request request)
{
  up_read = true;
  up_read_request = request;
#ifdef DUMP
  cout << "UP READ: " << request.addr << endl;
#endif
}

void AccessInterface::set_up_write(Request request)
{
  up_write = true;
  up_write_request = request;
#ifdef DUMP
  cout << "UP WRITE: " << request.addr << endl;
#endif
}

bool AccessInterface::up_read_exists()
{
  return up_read;
}

bool AccessInterface::up_write_exists()
{
  return up_write;
}

Request AccessInterface::get_up_read()
{
  up_read = false;
  return up_read_request;
}

Request AccessInterface::get_up_write()
{
  up_write = false;
  return up_write_request;
}

/******************************************************************************/
void AccessInterface::set_lo_read(Request request)
{
  lo_read = true;
  lo_read_request = request;
#ifdef DUMP
  cout << "LO READ: " << request.addr << endl;
#endif
}

void AccessInterface::set_lo_write(Request request)
{
  lo_write = true;
  lo_write_request = request;
#ifdef DUMP
  cout << "LO WRITE: " << request.addr << endl;
#endif
}

bool AccessInterface::lo_read_exists()
{
  return lo_read;
}

bool AccessInterface::lo_write_exists()
{
  return lo_write;
}

Request AccessInterface::get_lo_read()
{
  lo_read = false;
  return lo_read_request;
}

Request AccessInterface::get_lo_write()
{
  lo_write = false;
  return lo_write_request;
}
