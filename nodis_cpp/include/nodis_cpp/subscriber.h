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

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

#include "nodis_cpp/message.h"
#include "nodis_cpp/registration.h"
#include "nodis_cpp/types.h"

namespace nodis_cpp
{

template <typename T>
class Subscriber
{
public:
  using SyncFunction =
    std::function<std::vector<Message<T>>(const std::size_t, const std::optional<TimePoint>& time_point)>;
  using RegistrationFunction = std::function<void(const Registration, std::size_t capacity)>;

  //! Default constructor.
  Subscriber() = default;

  //! Copy constructor.
  Subscriber(const Subscriber& sub)
    : capacity_(sub.capacity_), sync_function_(sub.sync_function_), registration_function_(sub.registration_function_)
  {
    if (registration_function_)
    {
      registration_function_(Registration::Join, capacity_);
    }
  }

  //! Move constructor.
  Subscriber(Subscriber&& sub) = default;

  // Assignment operator.
  Subscriber& operator=(const Subscriber& sub)
  {
    if (registration_function_)
    {
      registration_function_(Registration::Leave, capacity_);
    }

    capacity_ = sub.capacity_;
    sync_function_ = sub.sync_function_;
    registration_function_ = sub.registration_function_;
    inbox_ = sub.inbox_;

    if (registration_function_)
    {
      registration_function_(Registration::Join, capacity_);
    }
  }

  //! Main constructor.
  Subscriber(const SyncFunction& sync_function,
             const RegistrationFunction& registration_function,
             const std::size_t capacity)
    : sync_function_(sync_function), registration_function_(registration_function), capacity_(capacity)
  {
    if (registration_function_)
    {
      registration_function_(Registration::Join, capacity_);
    }
  }

  //! Retrieve the entire buffer of messages from the nodis core.
  void sync()
  {
    if (sync_function_)
    {
      inbox_ = sync_function_(capacity_, std::optional<TimePoint>{});
    }
  }

  //! Retrieve all new messages from the nodis core.
  void syncNew()
  {
    if (sync_function_)
    {
      std::optional<TimePoint> time_point;
      if (!inbox_.empty())
      {
        time_point = inbox_.back().time_;
      }
      inbox_ = sync_function_(capacity_, time_point);
    }
  }

  //! Get the size of the current inbox.
  std::size_t size() const
  {
    return inbox_.size();
  }

  //! Get the maximum capacity of the inbox.
  std::size_t capacity() const
  {
    return capacity_;
  }

  //! Retrieve a specific message.
  const Message<T>& getMessage(const std::size_t index) const
  {
    if (index >= inbox_.size())
    {
      throw std::range_error("nodis_cpp::Subscriber::getMessage - indexed past the end of the inbox.");
    }

    return inbox_.at(index);
  }

  //! Destructor to de-register subscriber.
  ~Subscriber()
  {
    if (registration_function_)
    {
      registration_function_(Registration::Leave, capacity_);
    }
  }

protected:
  SyncFunction sync_function_;
  RegistrationFunction registration_function_;

  std::size_t capacity_;
  std::vector<Message<T>> inbox_;
};

}  // namespace nodis_cpp
