/*
Copyright (c) 2025 Krispin Davies.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "nodis_cpp/message.h"
#include "nodis_cpp/publisher_in.h"
#include "nodis_cpp/registration.h"
#include "nodis_cpp/subscriber_in.h"

#include <any>
#include <functional>
#include <map>
#include <mutex>
#include <deque>
#include <typeindex>
#include <typeinfo>

namespace nodis_cpp
{

class Core
{
public:
  using TopicType = std::pair<std::string, std::type_index>;

  template <typename T>
  PublisherIn<T> publisherIn(const std::string& topic)
  {
    const std::type_index type = typeid(T);
    const TopicType topic_type = std::make_pair(topic, type);

    const PublisherIn<T>::PublishFunction pub_func = [this, topic_type](const TimePoint& time, const DataPtr& data) -> bool
    {
      std::scoped_lock lock(pub_sub_mutex_);

      // Find the appropriate pub/sub table entry.
      auto pub_sub_iter = pub_sub_table_.find(topic_type);
      if (pub_sub_iter == pub_sub_table_.end())
      {
        return false;
      }

      // Construct an any message and add it to the inbox.
      MessageAny msg;
      msg.time_ = time;
      msg.data_ = data;
      pub_sub_iter->second.inbox_.push_back(&&msg);

      // Reduce inbox down to max capacity.
      while (pub_sub_iter->second.inbox_ > pub_sub_iter->second.max_capacity_)
      {
        pub_sub_iter->second.inbox_.pop_front();
      }
      return true;
    };

    const PublisherIn<T>::RegistrationFunction reg_func = [this, topic_type](const Registration registration)
    {
      std::scoped_lock lock(pub_sub_mutex_);
      auto pub_sub_iter = pub_sub_table_.find(topic_type);
      switch (registration)
      {
        case Registration::Join:
          if (pub_sub_iter == pub_sub_table_.end())
          {
            pub_sub_table_[topic_type] = PubSubEntry{ 1, 0, 1 };
          }
          else
          {
            pub_sub_iter->second.publishers_ += 1;
          }
          break;

        case Registration::Leave:
          if (pub_sub_iter != pub_sub_table_.end())
          {
            if (pub_sub_iter->second.publishers_ > 0)
            {
              pub_sub_iter->second.publishers_ -= 1;
            }
            if (pub_sub_iter->second.publishers_ == 0 && pub_sub_iter->second.subscribers_ == 0)
            {
              pub_sub_table_.erase(pub_sub_iter);
            }
          }
          break;
      }
    };
    
    return PublisherIn<T>{ pub_func, reg_func };
  }

protected:
  struct PubSubEntry
  {
    PubSubEntry(const std::size_t publishers, const std::size_t subscribers, const std::size_t max_capacity)
    : publishers_(publishers), 
    {
    }

    std::size_t publishers_{ 0 };
    std::size_t subscribers_{ 0 };
    std::size_t max_capacity_{ 0 };
    std::deque<MessageAny> inbox_;
  };

  std::mutex pub_sub_mutex_;
  std::map<TopicType, PubSubEntry> pub_sub_table_;
};

}
