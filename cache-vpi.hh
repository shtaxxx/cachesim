#ifndef CACHE_VPI_HH
#define CACHE_VPI_HH

#include "param.hh"
#include "vpi_memory.hh"

VpiMemory vpi_memory(MEMSIZE, DEFAULT_NWAYS, DEFAULT_NSETS);

#endif
