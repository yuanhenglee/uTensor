#include <cstdlib>
#include <unordered_map>

#include "uTensor.h"
#include "uTensor/core/errorHandler.hpp"

using std::free;
using std::malloc;
using uTensor::AllocatorInterface;
using uTensor::Context;
using uTensor::Handle;

DECLARE_ERROR(InvalidBoundError);
DEFINE_ERROR(InvalidBoundError);

class MallocAlloactor : public AllocatorInterface {
 protected:
  void _bind(void *ptr, Handle *hndl) override {
    if (ptrs_map.find(hndl) != ptrs_map.end()) {
      Context::get_default_context()->throwError(new InvalidBoundError);
    }
    ptrs_map[hndl] = ptr;
  }
  void _unbind(void *ptr, Handle *hndl) override {}
  bool _is_bound(void *ptr, Handle *hndl) override {}
  bool _has_handle(Handle *hndl) override {
    return ptrs_map.find(hndl) != ptrs_map.end();
  }
  void *_allocate(size_t sz) override {
    void *addr = malloc(sz);
    return addr;
  }
  void _deallocate(void *ptr) override { free(ptr); }

 private:
  std::unordered_map<Handle *, void *> ptrs_map;
};