#ifndef PARAM_HH
#define PARAM_HH

//#define DUMP
//#define DUMP_MISS
//#define DUMP_MEM

#define MEMORY_LATENCY (32)
#define LINE_SIZE (64)
#define MAX_OUTSTANDING_MISSES (4)
#define DEFAULT_NWAYS (4)
#define DEFAULT_NSETS (256)
#define MEMSIZE (1024 * 1024 * 512)

typedef unsigned int uint;
typedef unsigned long int ulong;
typedef unsigned long long addr_t;
typedef unsigned long long tag_t;
typedef unsigned long long data_t;
typedef unsigned char byte_t;

enum RequestType { REQ_NONE, REQ_READ, REQ_WRITE };
enum CmdType { CMD_NONE, CMD_READ, CMD_WRITE };

/******************************************************************************/
struct Request
{
  ulong time;
  addr_t addr;
  RequestType type;
  ulong latency;
  int way;;
  Request()
  {
    this->time = 0;
    this->addr = 0;
    this->type = REQ_NONE;
    this->latency = 0;
    this->way = 0;
  }
  Request(ulong _time, addr_t _addr, RequestType _type)
  {
    this->time = _time;
    this->addr = _addr;
    this->type = _type;
    this->latency = 0;
    this->way = 0;
  }
  void init()
  {
    this->time = 0;
    this->addr = 0;
    this->type = REQ_NONE;
    this->latency = 0;
    this->way = 0;
  }
  void step()
  {
    this->latency++;
  }
};

/******************************************************************************/
struct DramCmdState
{
  CmdType cmd;
  uint cycle;
  Request request;
  DramCmdState(){
    this->cmd = CMD_NONE;
    this->cycle = MEMORY_LATENCY;
    this->request.init();
  }
  bool done()
  {
    return (this->cycle == 0);
  }
  void step()
  {
    if(cycle > 0) cycle--;
  }
};

/******************************************************************************/
#endif
