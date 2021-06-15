#ifndef _AVA_GUESTLIB_COMMAND_WORKER_H_
#define _AVA_GUESTLIB_COMMAND_WORKER_H_

#include <glib.h>

#include "common/cmd_channel.hpp"
#include "guestlib/guest_thread.h"

namespace ava {

class CmdBatchingWorker {
 public:
  CmdBatchingWorker();
  ~CmdBatchingWorker();

  void Start();
  void enqueue_cmd(::command_base *cmd, ::command_channel *chan, bool is_async);

 private:
  bool running_;
  GuestThread cmd_batching_thread_;
  GAsyncQueue *pending_cmds_;
  GAsyncQueue *active_cmds_;
  void CmdBatchingThreadMain();
  DISALLOW_COPY_AND_ASSIGN(CmdBatchingWorker);
};

}  // namespace ava
#endif  // _AVA_GUESTLIB_COMMAND_WORKER_H_
