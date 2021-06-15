#include "guest_thread.h"

#include <fmt/core.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <gsl/gsl>
#include <iostream>

#include "common/logging.h"
#include "common/support/fs.h"
#include "common/support/io.h"
#include "common/support/thread.h"

namespace ava {

thread_local GuestThread *GuestThread::current_{nullptr};

int GuestThread::GetGuestStatsFd() {
  if (guest_stats_fd_ == -1) {
    auto stat_fname = fmt::format("{}/{}_{}", guest_stats_path_, guest_stats_prefix_, tid());
    auto stat_path = ava::support::GetRealPath(stat_fname);
    if (auto fd = ava::support::Create(stat_path)) {
      guest_stats_fd_ = *fd;
    } else {
      guest_stats_fd_ = STDOUT_FILENO;
    }
  }
  return guest_stats_fd_;
}

namespace {
static GuestThread main_guest_thread{ava::kGuestMainThreadName, nullptr};
}

void GuestThread::Start() {
  if (name_ == kMainThreadName) {
    std::cerr << "Cannot call Start() on the guest main thread" << std::endl;
    abort();
  }
  if (!fn_) {
    std::cerr << fmt::format("Empty entry function for thread {}", name_);
    abort();
  }
  state_.store(kStarting);
  CHECK_EQ(pthread_create(&pthread_, nullptr, &GuestThread::StartRoutine, this), 0);
  started_.WaitForNotification();
  CHECK(state_.load() == kRunning);
}

void GuestThread::RegisterMainThread(std::string guest_stats_path, std::string guest_stats_prefix) {
  GuestThread *thread = &main_guest_thread;
  thread->state_.store(kRunning);
  thread->tid_ = gsl::narrow_cast<int>(syscall(SYS_gettid));
  thread->pthread_ = pthread_self();
  thread->guest_stats_path_ = guest_stats_path;
  thread->guest_stats_prefix_ = guest_stats_prefix;
  current_ = thread;
  AVA_LOG_F(INFO, "Register guestlib main thread: tid={}", thread->tid_);
}

void *GuestThread::StartRoutine(void *arg) {
  std::cerr << "enter guest thread start routine" << std::endl;
  GuestThread *self = reinterpret_cast<GuestThread *>(arg);
  std::cerr << "after cast" << std::endl;
  current_ = self;
  std::cerr << "after assign" << std::endl;
  self->Run();
  return nullptr;
}

void guest_write_stats(std::string stats) {
  auto gthread = (GuestThread *)GuestThread::current();
  int guest_stats_fd = gthread->GetGuestStatsFd();
  ava::support::WriteData(guest_stats_fd, stats.data(), stats.size());
}

int get_guest_stats_fd() {
  auto gthread = (GuestThread *)GuestThread::current();
  return gthread->GetGuestStatsFd();
}

void register_guestlib_main_thread(std::string guest_stats_path, std::string guest_stats_prefix) {
  GuestThread::RegisterMainThread(guest_stats_path, guest_stats_prefix);
}
}  // namespace ava
