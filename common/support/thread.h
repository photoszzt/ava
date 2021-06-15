#ifndef _AVA_SUPPORT_THREAD_H_
#define _AVA_SUPPORT_THREAD_H_
#include <absl/strings/string_view.h>
#include <absl/synchronization/notification.h>
#include <pthread.h>

#include <atomic>
#include <functional>

#include "common/declaration.h"
#include "common/logging.h"

namespace ava {
namespace support {

class Thread {
 public:
  static constexpr const char *kMainThreadName = "Main";

  Thread(absl::string_view name, std::function<void()> fn);
  ~Thread();

  virtual void Start();
  void Join();

  void Cancel();

  // Set cpu affinity and nice based on AVA_<category>_THREAD_CPUSET and
  // AVA_<category>_THREAD_NICE
  void MarkThreadCategory(absl::string_view category);

  const char *name() const { return name_.c_str(); }
  int tid() const { return tid_; }

  static Thread *current() {
#ifdef __CAVA__
    return current_;
#else
    return DCHECK_NOTNULL(current_);
#endif
  }

  static void RegisterMainThread();

 protected:
  enum State { kCreated, kStarting, kRunning, kFinished };

  std::atomic<State> state_;
  std::string name_;
  std::function<void()> fn_;
  int tid_;

  absl::Notification started_;
  pthread_t pthread_;

  static thread_local Thread *current_;

  void Run();
  static void *StartRoutine(void *arg);

  DISALLOW_COPY_AND_ASSIGN(Thread);
};

}  // namespace support
}  // namespace ava

#endif  // _AVA_SUPPORT_THREAD_H_
