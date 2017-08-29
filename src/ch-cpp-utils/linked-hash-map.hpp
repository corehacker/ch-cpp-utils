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
 * \file   linked-hash-map.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Mar 16, 2017
 *
 * \brief  
 *
 ******************************************************************************/

#include <iostream>
#include <string>
#include <memory>
#include <list>
#include <unordered_map>

#ifndef __SRC_UTILS_LINKED_HASH_MAP_HPP__
#define __SRC_UTILS_LINKED_HASH_MAP_HPP__


/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/

/***************************** FUNCTION PROTOTYPES ****************************/

template<class T>
   class LinkedHashMap
   {
   private:
      std::shared_ptr<std::list<T> > list;
      std::shared_ptr<std::unordered_map<std::string, T> > map;
      public:
      LinkedHashMap ();

      ~LinkedHashMap ();

      void
      insert (std::string key, T value);

      bool
      has (std::string key);

      T
      find (std::string key);

      void
      erase (std::string key);

      bool
      empty ();

      size_t
      size ();

      void
      print ();
   };

template<class T>
   LinkedHashMap<T>::LinkedHashMap ()
   {
      list = std::make_shared<std::list<T> > ();
      map = std::make_shared<std::unordered_map<std::string, T> > ();
   }

template<class T>
   LinkedHashMap<T>::~LinkedHashMap ()
   {

   }

template<class T>
   void
   LinkedHashMap<T>::insert (std::string key, T value)
   {
      map->insert (std::make_pair (key, value));
   }

template<class T>
   bool
   LinkedHashMap<T>::has (std::string key)
   {
      return false;
   }

template<class T>
   T
   LinkedHashMap<T>::find (std::string key)
   {
      auto search = map->find (key);
      return search->second;
   }

template<class T>
   void
   LinkedHashMap<T>::erase (std::string key)
   {
      map->erase (key);
   }

template<class T>
   bool
   LinkedHashMap<T>::empty ()
   {
      return map->empty ();
   }

template<class T>
   size_t
   LinkedHashMap<T>::size ()
   {
      return map->size ();
   }

template<class T>
   void
   LinkedHashMap<T>::print ()
   {
      for (auto iter = map->begin (); iter != map->end (); ++iter)
      {
         auto key = iter->first;
         auto value = iter->second;
         std::cout << key << " -> " << value << std::endl;
      }
   }

#endif /* __SRC_UTILS_LINKED_HASH_MAP_HPP__ */
