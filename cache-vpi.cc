#include <vpi_user.h>
#include "cache-vpi.hh"

/******************************************************************************/
static int cache_initialize_compiletf(char*user_data)
{
  return 0;
}

static int cache_initialize_calltf(char*user_data)
{
  cout << "# Cache is initialized." << endl;
  return 0;
}

void cache_initialize_register()
{
  s_vpi_systf_data tf_data;
  tf_data.type      = vpiSysTask;
  tf_data.tfname    = "$cache_initialize";
  tf_data.calltf    = cache_initialize_calltf;
  tf_data.compiletf = cache_initialize_compiletf;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

/******************************************************************************/
static int cache_finalize_compiletf(char*user_data)
{
  return 0;
}

static int cache_finalize_calltf(char*user_data)
{
  cout << "# Cache is finalized." << endl;
  cout << "Access     : " << vpi_memory.get_access_cnt() << endl;
  cout << "Read       : " << vpi_memory.get_read_cnt() << endl;
  cout << "Write      : " << vpi_memory.get_write_cnt() << endl;
  cout << "Hit        : " << vpi_memory.get_hit_cnt() << endl;
  cout << "Miss       : " << vpi_memory.get_miss_cnt() << endl;
  cout << "Read Hit   : " << vpi_memory.get_hit_read_cnt() << endl;
  cout << "Read Miss  : " << vpi_memory.get_miss_read_cnt() << endl;
  cout << "Write Hit  : " << vpi_memory.get_hit_write_cnt() << endl;
  cout << "Write Miss : " << vpi_memory.get_miss_write_cnt() << endl;
  return 0;
}

void cache_finalize_register()
{
  s_vpi_systf_data tf_data;
  tf_data.type      = vpiSysTask;
  tf_data.tfname    = "$cache_finalize";
  tf_data.calltf    = cache_finalize_calltf;
  tf_data.compiletf = cache_finalize_compiletf;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

/******************************************************************************/
static int cache_read_compiletf(char*user_data)
{
  return 0;
}

static int cache_read_calltf(char*user_data)
{
  vpiHandle vhandle, iterator;

  vpiHandle addr_handle;
  s_vpi_value addr_val;
  addr_t addr;

  vpiHandle size_handle;
  s_vpi_value size_val;
  uint size;

  data_t readdata;
  vpiHandle readdata_handle;
  s_vpi_value readdata_val;

  bool valid;
  int valid_int;
  vpiHandle valid_handle;
  s_vpi_value valid_val;

  vhandle = vpi_handle(vpiSysTfCall, NULL);
  iterator = vpi_iterate(vpiArgument, vhandle);

  addr_handle = vpi_scan(iterator);
  addr_val.format = vpiIntVal;
  vpi_get_value(addr_handle, &addr_val);
  addr = (addr_t) addr_val.value.integer;

  size_handle = vpi_scan(iterator);
  size_val.format = vpiIntVal;
  vpi_get_value(size_handle, &size_val);
  size = (uint) size_val.value.integer;

  //cout << "############" << endl;
  valid = vpi_memory.read(addr, size, &readdata);
  //cout << "## VPI readdata" << readdata << endl;
  valid_int = (valid)? 1 : 0;

  readdata_handle = vpi_scan(iterator);
  readdata_val.format = vpiIntVal;
  readdata_val.value.integer = (uint) readdata;
  vpi_put_value(readdata_handle, &readdata_val, NULL, vpiNoDelay);

  valid_handle = vpi_scan(iterator);
  valid_val.format = vpiIntVal;
  valid_val.value.integer = (uint) valid_int;
  vpi_put_value(valid_handle, &valid_val, NULL, vpiNoDelay);

  return 0;
}

void cache_read_register()
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysTask;
  tf_data.tfname    = "$cache_read";
  tf_data.calltf    = cache_read_calltf;
  tf_data.compiletf = cache_read_compiletf;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

/******************************************************************************/
static int cache_read_busy_compiletf(char*user_data)
{
  return 0;
}

static int cache_read_busy_calltf(char*user_data)
{
  vpiHandle vhandle, iterator;

  bool valid;
  int valid_int;
  vpiHandle valid_handle;
  s_vpi_value valid_val;

  vhandle = vpi_handle(vpiSysTfCall, NULL);
  iterator = vpi_iterate(vpiArgument, vhandle);

  valid = vpi_memory.read_busy();
  valid_int = (valid)? 1 : 0;

  valid_handle = vpi_scan(iterator);
  valid_val.format = vpiIntVal;
  valid_val.value.integer = (uint) valid_int;
  vpi_put_value(valid_handle, &valid_val, NULL, vpiNoDelay);

  return 0;
}

void cache_read_busy_register()
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysTask;
  tf_data.tfname    = "$cache_read_busy";
  tf_data.calltf    = cache_read_busy_calltf;
  tf_data.compiletf = cache_read_busy_compiletf;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

/******************************************************************************/
static int cache_write_compiletf(char*user_data)
{
  return 0;
}

static int cache_write_calltf(char*user_data)
{
  vpiHandle vhandle, iterator;

  vpiHandle addr_handle;
  s_vpi_value addr_val;
  addr_t addr;

  vpiHandle size_handle;
  s_vpi_value size_val;
  uint size;

  vpiHandle writedata_handle;
  s_vpi_value writedata_val;
  uint writedata_int;
  data_t writedata;

  bool valid;
  int valid_int;
  vpiHandle valid_handle;
  s_vpi_value valid_val;

  vhandle = vpi_handle(vpiSysTfCall, NULL);
  iterator = vpi_iterate(vpiArgument, vhandle);

  addr_handle = vpi_scan(iterator);
  addr_val.format = vpiIntVal;
  vpi_get_value(addr_handle, &addr_val);
  addr = (addr_t) addr_val.value.integer;

  size_handle = vpi_scan(iterator);
  size_val.format = vpiIntVal;
  vpi_get_value(size_handle, &size_val);
  size = (uint) size_val.value.integer;

  writedata_handle = vpi_scan(iterator);
  writedata_val.format = vpiIntVal;
  vpi_get_value(writedata_handle, &writedata_val);
  writedata_int = (uint) writedata_val.value.integer;
  writedata = (data_t) writedata_int;

  valid = vpi_memory.write(addr, size, writedata);
  valid_int = (valid)? 1 : 0;

  valid_handle = vpi_scan(iterator);
  valid_val.format = vpiIntVal;
  valid_val.value.integer = (uint) valid_int;
  vpi_put_value(valid_handle, &valid_val, NULL, vpiNoDelay);

  return 0;
}

void cache_write_register()
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysTask;
  tf_data.tfname    = "$cache_write";
  tf_data.calltf    = cache_write_calltf;
  tf_data.compiletf = cache_write_compiletf;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

/******************************************************************************/
static int cache_write_busy_compiletf(char*user_data)
{
  return 0;
}

static int cache_write_busy_calltf(char*user_data)
{
  vpiHandle vhandle, iterator;

  bool valid;
  int valid_int;
  vpiHandle valid_handle;
  s_vpi_value valid_val;

  vhandle = vpi_handle(vpiSysTfCall, NULL);
  iterator = vpi_iterate(vpiArgument, vhandle);

  valid = vpi_memory.write_busy();
  valid_int = (valid)? 1 : 0;

  valid_handle = vpi_scan(iterator);
  valid_val.format = vpiIntVal;
  valid_val.value.integer = (uint) valid_int;
  vpi_put_value(valid_handle, &valid_val, NULL, vpiNoDelay);

  return 0;
}

void cache_write_busy_register()
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysTask;
  tf_data.tfname    = "$cache_write_busy";
  tf_data.calltf    = cache_write_busy_calltf;
  tf_data.compiletf = cache_write_busy_compiletf;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

/******************************************************************************/
static int cache_step_compiletf(char*user_data)
{
  return 0;
}

static int cache_step_calltf(char*user_data)
{
  vpi_memory.step();
  return 0;
}

void cache_step_register()
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysTask;
  tf_data.tfname    = "$cache_step";
  tf_data.calltf    = cache_step_calltf;
  tf_data.compiletf = cache_step_compiletf;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

/******************************************************************************/
void (*vlog_startup_routines[])() = {
  cache_initialize_register,
  cache_finalize_register,
  cache_read_register,
  cache_read_busy_register,
  cache_write_register,
  cache_write_busy_register,
  cache_step_register,
  0
};

