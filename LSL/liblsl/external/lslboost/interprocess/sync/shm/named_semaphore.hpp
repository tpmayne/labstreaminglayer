 //////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_SHM_NAMED_SEMAPHORE_HPP
#define BOOST_INTERPROCESS_SHM_NAMED_SEMAPHORE_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>
#include <lslboost/interprocess/creation_tags.hpp>
#include <lslboost/interprocess/exceptions.hpp>
#include <lslboost/interprocess/permissions.hpp>
#include <lslboost/interprocess/detail/interprocess_tester.hpp>
#include <lslboost/interprocess/detail/posix_time_types_wrk.hpp>
#include <lslboost/interprocess/shared_memory_object.hpp>
#include <lslboost/interprocess/detail/managed_open_or_create_impl.hpp>
#include <lslboost/interprocess/sync/interprocess_semaphore.hpp>
#include <lslboost/interprocess/sync/shm/named_creation_functor.hpp>

namespace lslboost {
namespace interprocess {
namespace ipcdetail {

class shm_named_semaphore
{
   /// @cond

   //Non-copyable
   shm_named_semaphore();
   shm_named_semaphore(const shm_named_semaphore &);
   shm_named_semaphore &operator=(const shm_named_semaphore &);
   /// @endcond

   public:
   shm_named_semaphore(create_only_t, const char *name, unsigned int initialCount, const permissions &perm = permissions());

   shm_named_semaphore(open_or_create_t, const char *name, unsigned int initialCount, const permissions &perm = permissions());

   shm_named_semaphore(open_only_t, const char *name);

   ~shm_named_semaphore();

   void post();
   void wait();
   bool try_wait();
   bool timed_wait(const lslboost::posix_time::ptime &abs_time);

   static bool remove(const char *name);

   /// @cond
   private:
   friend class interprocess_tester;
   void dont_close_on_destruction();

   interprocess_semaphore *semaphore() const
   {  return static_cast<interprocess_semaphore*>(m_shmem.get_user_address()); }

   managed_open_or_create_impl<shared_memory_object> m_shmem;
   typedef named_creation_functor<interprocess_semaphore, int> construct_func_t;
   /// @endcond
};

inline shm_named_semaphore::~shm_named_semaphore()
{}

inline void shm_named_semaphore::dont_close_on_destruction()
{  interprocess_tester::dont_close_on_destruction(m_shmem);  }

inline shm_named_semaphore::shm_named_semaphore
   (create_only_t, const char *name, unsigned int initialCount, const permissions &perm)
   :  m_shmem  (create_only
               ,name
               ,sizeof(interprocess_semaphore) +
                  managed_open_or_create_impl<shared_memory_object>::
                     ManagedOpenOrCreateUserOffset
               ,read_write
               ,0
               ,construct_func_t(DoCreate, initialCount)
               ,perm)
{}

inline shm_named_semaphore::shm_named_semaphore
   (open_or_create_t, const char *name, unsigned int initialCount, const permissions &perm)
   :  m_shmem  (open_or_create
               ,name
               ,sizeof(interprocess_semaphore) +
                  managed_open_or_create_impl<shared_memory_object>::
                     ManagedOpenOrCreateUserOffset
               ,read_write
               ,0
               ,construct_func_t(DoOpenOrCreate, initialCount)
               ,perm)
{}

inline shm_named_semaphore::shm_named_semaphore
   (open_only_t, const char *name)
   :  m_shmem  (open_only
               ,name
               ,read_write
               ,0
               ,construct_func_t(DoOpen, 0))
{}

inline void shm_named_semaphore::post()
{  semaphore()->post();   }

inline void shm_named_semaphore::wait()
{  semaphore()->wait();   }

inline bool shm_named_semaphore::try_wait()
{  return semaphore()->try_wait();   }

inline bool shm_named_semaphore::timed_wait(const lslboost::posix_time::ptime &abs_time)
{
   if(abs_time == lslboost::posix_time::pos_infin){
      this->wait();
      return true;
   }
   return semaphore()->timed_wait(abs_time);
}

inline bool shm_named_semaphore::remove(const char *name)
{  return shared_memory_object::remove(name); }

}  //namespace ipcdetail {
}  //namespace interprocess {
}  //namespace lslboost {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_SHM_NAMED_SEMAPHORE_HPP