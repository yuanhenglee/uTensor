#include <cstdlib>
#include <unordered_map>

#include "uTensor.h"
#include "uTensor/core/errorHandler.hpp"

using std::free;
using std::malloc;

namespace uTensor {

DECLARE_ERROR(InvalidBoundError);
DEFINE_ERROR(InvalidBoundError);

class MallocAlloactor : public AllocatorInterface {
 public:
  MallocAlloactor(size_t capability) : capability_(capability) {}
  size_t available() override { return capability_; }
  bool rebalance() override { return true; }

 protected:
  void _bind(void *ptr, Handle *hndl) override {
    if (ptrs_map_.find(hndl) != ptrs_map_.end()) {
      Context::get_default_context()->throwError(new InvalidBoundError());
    }
    ptrs_map_[hndl] = ptr;
  }
  void _unbind(void *ptr, Handle *hndl) override {
    if (ptrs_map_.find(hndl) == ptrs_map_.end()) {
      return;
    }
    ptrs_map_.erase(hndl);
  }
  bool _is_bound(void *ptr, Handle *hndl) override {
    if (ptrs_map_.find(hndl) == ptrs_map_.end()) {
      return false;
    }
    return ptrs_map_[hndl] == ptr;
  }
  bool _has_handle(Handle *hndl) override {
    return ptrs_map_.find(hndl) != ptrs_map_.end();
  }
  void *_allocate(size_t sz) override {
    void *addr = malloc(sz);
    return addr;
  }
  void _deallocate(void *ptr) override { free(ptr); }

 private:
  std::unordered_map<Handle *, void *> ptrs_map_;
  size_t capability_;
};
}  // namespace uTensor

int main(int argc, const char **argv) {
  uTensor::MallocAlloactor alloc(10000);
  return 0;
}
