#include "thread.h"

#include <absl/strings/str_split.h>
#include <errno.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <sched.h>
#include <sys/syscall.h>

#include <gsl/gsl>
#include <iostream>

#include "common/logging.h"
#include "env_variables.h"
#include "fmt.h"

namespace ava {
namespace support {

namespace {
static int __gettid() { return gsl::narrow_cast<int>(syscall(SYS_gettid)); }
}  // namespace

thread_local Thread *Thread::current_{nullptr};

Thread::Thread(absl::string_view name, std::function<void()> fn)
    : state_(kCreated), name_(std::string(name)), fn_(fn), tid_(-1) {}

Thread::~Thread() {
  if (name_ != kMainThreadName) {
    State state = state_.load();
    CHECK(state == kCreated || state == kFinished);
  }
}

void Thread::Start() {
  if (name_ == kMainThreadName) {
    std::cerr << "Cannot call Start() on the main thread" << std::endl;
    abort();
  }
  if (!fn_) {
    std::cerr << fmt::format("Empty entry function for thread {}", name_);
    abort();
  }
  state_.store(kStarting);
  CHECK_EQ(pthread_create(&pthread_, nullptr, &Thread::StartRoutine, this), 0);
  started_.WaitForNotification();
  CHECK(state_.load() == kRunning);
}

void Thread::Join() {
  if (name_ == kMainThreadName) {
    AVA_LOG(FATAL) << "Cannot call Join() on the main thread";
    abort();
  }
  State state = state_.load();
  if (state == kFinished) {
    return;
  }
  CHECK(state == kRunning);
  CHECK_EQ(pthread_join(pthread_, nullptr), 0);
}

void Thread::Cancel() {
  if (pthread_cancel(pthread_)) {
    AVA_LOG(ERROR) << "pthread_cancel failed " << strerror(errno);
  }
}

void Thread::Run() {
  tid_ = __gettid();
  state_.store(kRunning);
  started_.Notify();
  AVA_LOG_F(INFO, "Start thread: {} (tid={})", name_, tid_);
  fn_();
  state_.store(kFinished);
}

void Thread::MarkThreadCategory(absl::string_view category) {
  CHECK(current_ == this);
  // Set cpuset
  std::string cpuset_var_name = fmt::format("AVA_{}_THREAD_CPUSET", category);
  std::string cpuset_str(support::GetEnvVariable(cpuset_var_name));
  if (!cpuset_str.empty()) {
    cpu_set_t set;
    CPU_ZERO(&set);
    for (const absl::string_view &cpu_str : absl::StrSplit(cpuset_str, ",")) {
      int cpu;
      CHECK(absl::SimpleAtoi(cpu_str, &cpu));
      CPU_SET(cpu, &set);
    }
    if (sched_setaffinity(0, sizeof(set), &set) != 0) {
      AVA_LOG_F(FATAL, "Failed to set CPU affinity to {}", cpuset_str);
      abort();
    } else {
      AVA_LOG_F(INFO, "Successfully set CPU affinity of current thread to {}", cpuset_str);
    }
  } else {
    AVA_LOG_F(INFO, "Does not find cpuset setting for {} threads (can be set by {})", category, cpuset_var_name);
  }
  // Set nice
  std::string nice_var_name = fmt::format("AVA_{}_THREAD_NICE", category);
  std::string nice_str(support::GetEnvVariable(nice_var_name));
  if (!nice_str.empty()) {
    int nice_value;
    CHECK(absl::SimpleAtoi(nice_str, &nice_value));
    int current_nice = nice(0);
    errno = 0;
    if (nice(nice_value - current_nice) == -1 && errno != 0) {
      AVA_LOG_F(FATAL, "Failed to set nice to {}", nice_value);
      abort();
    } else {
      CHECK_EQ(nice(0), nice_value);
      AVA_LOG_F(INFO, "Successfully set nice of current thread to {}", nice_value);
    }
  } else {
    AVA_LOG_F(INFO, "Does not find nice setting for {} threads (can be set by {})", category, nice_var_name);
  }
}

void *Thread::StartRoutine(void *arg) {
  Thread *self = reinterpret_cast<Thread *>(arg);
  current_ = self;
  self->Run();
  return nullptr;
}

namespace {
static Thread main_thread{Thread::kMainThreadName, nullptr};
}

void Thread::RegisterMainThread() {
  Thread *thread = &main_thread;
  thread->state_.store(kRunning);
  thread->tid_ = __gettid();
  thread->pthread_ = pthread_self();
  current_ = thread;
  AVA_LOG_F(INFO, "Register main thread: tid={}", thread->tid_);
}

}  // namespace support
}  // namespace ava
