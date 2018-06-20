/*******************************************************************************
 *
 *  BSD 2-Clause License
 *
 *  Copyright (c) 2018, Sandeep Prakash
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*******************************************************************************
 * Copyright (c) 2018, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   proc-stat.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Jun 18, 2018
 *
 * \brief
 *
 ******************************************************************************/

#include "ch-cpp-utils/proc-stat.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <chrono>
#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>

#include <glog/logging.h>

using std::ostringstream;
using std::lock_guard;

namespace ChCppUtils {

ProcStat::ProcStat() {
  mTimer = new Timer();
	struct timeval tv = {0};
	tv.tv_sec = 1;
	mTimerEvent = mTimer->create(&tv, ProcStat::_onTimerEvent, this);
}

ProcStat::~ProcStat() {

}

#define unlikely(x)     __builtin_expect(!!(x),0)
#define KLF "l"

void ProcStat::stat2proc(const char* S, proc_t *P) {
    unsigned num;

    /* fill in default values for older kernels */
    P->processor = 0;
    P->rtprio = -1;
    P->sched = -1;
    P->nlwp = 0;

    // LOG(INFO) << "S: " << S;

    S = strchr(S, '(') + 1;
    const char *tmp = strrchr(S, ')');
    num = tmp - S;
    if(unlikely(num >= sizeof P->cmd)) num = sizeof P->cmd - 1;
    memcpy(P->cmd, S, num);
    P->cmd[num] = '\0';
    S = tmp + 2;                 // skip ") "

    sscanf(S,
       "%c "
       "%d %d %d %d %d "
       "%lu %lu %lu %lu %lu "
       "%Lu %Lu %Lu %Lu "  /* utime stime cutime cstime */
       "%ld %ld "
       "%d "
       "%ld "
       "%Lu "  /* start_time */
       "%lu "
       "%ld "
       "%lu %lu %lu %lu %lu %lu "
       "%*s %*s %*s %*s " /* discard, no RT signals & Linux 2.1 used hex */
       "%lu %*lu %*lu "
       "%d %d "
       "%lu %lu",
       &P->state,
       &P->ppid, &P->pgrp, &P->session, &P->tty, &P->tpgid,
       &P->flags, &P->min_flt, &P->cmin_flt, &P->maj_flt, &P->cmaj_flt,
       &P->utime, &P->stime, &P->cutime, &P->cstime,
       &P->priority, &P->nice,
       &P->nlwp,
       &P->alarm,
       &P->start_time,
       &P->vsize,
       &P->rss,
       &P->rss_rlim, &P->start_code, &P->end_code, &P->start_stack, &P->kstk_esp, &P->kstk_eip,
/*     P->signal, P->blocked, P->sigignore, P->sigcatch,   */ /* can't use */
       &P->wchan, /* &P->nswap, &P->cnswap, */  /* nswap and cnswap dead for 2.4.xx and up */
/* -- Linux 2.0.35 ends here -- */
       &P->exit_signal, &P->processor,  /* 2.2.1 ends with "exit_signal" */
/* -- Linux 2.2.8 to 2.5.17 end here -- */
       &P->rtprio, &P->sched  /* both added to 2.5.18 */
    );

    if(!P->nlwp){
      P->nlwp = 1;
    }
}

void ProcStat::_onTimerEvent(TimerEvent *event, void *this_) {
	ProcStat *server = (ProcStat *) this_;
	server->onTimerEvent(event);
}

void ProcStat::onTimerEvent(TimerEvent *event) {
  memset(&proc, 0x00, sizeof(proc));
  ostringstream os;
  os << "/proc/" << getpid() << "/stat";
  std::ifstream t(os.str());
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());

  {
    lock_guard<mutex> lock(mMutex);
    stat2proc(str.c_str(), &proc);
  }
  

  LOG(INFO) << "RSS: " << proc.rss << " pages, Page Size: " << PAGESIZE << ", RSS: " << 
    ((proc.rss * PAGESIZE) / 1024) << " KB";

	mTimer->restart(event);
}

uint32_t ProcStat::getRSS() {
  lock_guard<mutex> lock(mMutex);
  return proc.rss * PAGESIZE;
}

}
