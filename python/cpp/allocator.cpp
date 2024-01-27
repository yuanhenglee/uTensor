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
  MallocAlloactor(size_t capability) : total_(capability), accum_(0) {}
  size_t available() override {
    size_t rem = static_cast<size_t>(total_ - accum_);
    return rem;
  }
  bool rebalance() override { return true; }
  void set_total(size_t v) { total_ = v; }

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
    if (total_ < accum_ + sz) {
      return nullptr;
    }
    void *addr = malloc(sz);
    accum_ += sz;
    size_map_[addr] = sz;
    return addr;
  }
  void _deallocate(void *ptr) override {
    if (size_map_.find(ptr) == size_map_.end()) {
      // throw error?
      return;
    }
    size_t sz = size_map_[ptr];
    size_map_.erase(ptr);
    accum_ -= sz;
    free(ptr);
  }

 private:
  std::unordered_map<Handle *, void *> ptrs_map_;
  std::unordered_map<void *, size_t> size_map_;
  unsigned long long total_;
  unsigned long long accum_;
};
}  // namespace uTensor

int main(int argc, const char **argv) {
  uTensor::MallocAlloactor alloc(10000);
  return 0;
}
