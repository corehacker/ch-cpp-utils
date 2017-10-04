/*******************************************************************************
 *
 *  BSD 2-Clause License
 *
 *  Copyright (c) 2017, Sandeep Prakash
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
 * Copyright (c) 2017, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   logger.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Apr 7, 2017
 *
 * \brief
 *
 ******************************************************************************/

// #include <pthread.h>
#include <libgen.h>
#include <unistd.h>

#include <type_traits>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <ctime>
#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <condition_variable>

#ifndef __SRC_UTILS_LOGGER_HPP__
#define __SRC_UTILS_LOGGER_HPP__

#define LOG log << basename ((char *) __FILE__) << ":" << __FUNCTION__ << ":" << __LINE__ << " | 0x"\
   << std::hex << std::this_thread::get_id () << " | "

namespace ChCppUtils {

class Logger
{
   private:
      std::ostream &m_file;
      std::mutex mQMutex;
      std::mutex mLogMutex;
      std::condition_variable mQCondition;
      std::condition_variable mShutdownSignal;
	   std::thread       *mThread;
      std::deque<std::ostringstream *> mLogQueue;
      std::unordered_map<std::thread::id, std::ostringstream *> mLogMap;
      bool shutdown;

      static void *threadFunc (void *this_) {
         Logger *logger = (Logger *) this_;
         logger->threadFunc_ ();
         return NULL;
      }

      void threadFunc_ () {
         printf ("Running logger thread routine\n");
         while (!shutdown)
         {
            if (!mLogQueue.empty ())
            {
               mQMutex.lock ();
               std::ostringstream *log = mLogQueue.at (0);
               mLogQueue.pop_front ();
               mQMutex.unlock ();
               std::cout << log->str ();
               delete log;
               fflush (stdout);
            }
            else
            {
               std::unique_lock < std::mutex > lk (mQMutex);
               mQCondition.wait (lk);
            }
         }
         mThread->detach();
         printf ("Exiting logger thread routine\n");
         mShutdownSignal.notify_one ();
      }

   public:
      static Logger& getInstance()
      {
         static Logger instance; // Guaranteed to be destroyed.
                                // Instantiated on first use.
         return instance;
      }

      Logger (std::ostream &o = std::cout) :
            m_file (o)
      {
         shutdown = false;
         printf ("Creating logger thread\n");
         mThread = new std::thread(Logger::threadFunc, this);
      }

      template<typename T>
      Logger &operator<< (const T &a)
      {
         std::thread::id threadId = std::this_thread::get_id ();
         mLogMutex.lock ();
         if (shutdown) {
            return *this;
         }
         auto threadEntry = mLogMap.find(threadId);
         if(threadEntry == mLogMap.end()) {
            mLogMap.insert(std::make_pair (threadId, new std::ostringstream ()));
         }

         threadEntry = mLogMap.find(threadId);
         (*threadEntry->second) << a;
         mLogMutex.unlock ();

         return *this;
      }

      Logger &operator<< (std::ostream& (*pf) (std::ostream&))
      {
         std::thread::id threadId = std::this_thread::get_id ();

         mLogMutex.lock ();
         if (shutdown) {
            return *this;
         }
         auto threadEntry = mLogMap.find(threadId);

         if(threadEntry == mLogMap.end()) {
            mLogMap.insert(std::make_pair (threadId, new std::ostringstream ()));
         }

         threadEntry = mLogMap.find(threadId);
         (*threadEntry->second) << pf;

         std::ostringstream *log = new std::ostringstream (threadEntry->second->str());
         threadEntry->second->str("");
         threadEntry->second->clear();
         mLogMutex.unlock ();

         std::lock_guard < std::mutex > lock (mQMutex);
         mLogQueue.push_back (log);
         mQCondition.notify_one ();
         return *this;
      }

      ~Logger() {
         printf("\n\n*****************~Logger\n\n");
         mLogMutex.lock ();
         shutdown = true;
         mLogMutex.unlock();
         std::unique_lock < std::mutex > lk (mQMutex);

         printf ("Waiting for logger thread to exit.\n");
         mShutdownSignal.wait (lk);
         printf ("Logger thread to exited.\n");

         for( const auto& logEntry : mLogMap) {
            delete logEntry.second;
         }
         delete mThread;
      }
};

}

#endif /* __SRC_UTILS_LOGGER_HPP__ */
