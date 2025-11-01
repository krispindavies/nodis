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

#include "nodis_cpp/registration.h"

#include <chrono>
#include <functional>
#include <memory>

namespace nodis_cpp
{

template <typename T>
class PublisherIn
{
public:
  using PublishFunction = std::function<bool(const TimePoint&, const std::shared_ptr<const T>&)>;
  using RegistrationFunction = std::function<void(const Registration)>;

  //! Default constructor.
  PublisherIn() = default;

  //! Copy constructor.
  PublisherIn(const PublisherIn& pub)
  : publish_function_(pub.publish_function_), registration_function_(pub.registration_function_)
  {
    if (registration_function_)
    {
      registration_function_(Registration::Join);
    }
  }

  //! Move constructor.
  // TODO(kdavies) = Check if we need to update registration, not sure if destructor is called.
  PublisherIn(PublisherIn&& pub) = default;

  //! Main constructor, used by the nodis backbone structure.
  PublisherIn(const PublishFunction& publish_function, const RegistrationFunction& registration_function)
  : publish_function_(publish_function), registration_function_(registration_function)
  {
    if (registration_function_)
    {
      registration_function_(Registration::Join);
    }
  }

  //! Publish a message to the nodis backbone.
  bool publish(const T& data) const
  {
    if (publish_function_)
    {
      return publish(std::make_shared<const T>(data));
    }
    return false;
  }

  //! Publish a message to the nodis backbone.
  bool publish(const std::shared_ptr<T>& data) const
  {
    if (publish_function_)
    {
      return publish_function_(std::chrono::utc_clock::now(), std::static_pointer_cast<const T>(data));
    }
    return false;
  }

  //! Publish a message to the nodis backbone.
  bool publish(const std::shared_ptr<const T>& data) const
  {
    if (publish_function_)
    {
      return publish_function_(std::chrono::utc_clock::now(), data);
    }
    return false;
  }

  //! Destructor to de-register publisher.
  ~PublisherIn()
  {
    if (registration_function_)
    {
      registration_function_(Registration::Leave);
    }
  }

protected:
  PublishFunction publish_function_;
  RegistrationFunction registration_function_;
};

}
