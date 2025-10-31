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

#include "nodis_cpp/core.h"
#include "nodis_cpp/subscriber_in.h"
#include "nodis_cpp/subscriber_in.h"

#include <iostream>

int main()
{
  nodis_cpp::Core core;
  auto double_pub = core.publisherIn<double>("/data_link");
  auto double_sub = core.subscriberIn<double>("/data_link", 10);
  double_pub.publish(6.4);
  double_pub.publish(3.6);
  double_pub.publish(4.9);
  double_sub.sync();
  std::cout << "Subscriber size: [" << double_sub.size() << "]" << std::endl;
  std::cout << "Subscriber capacity: [" << double_sub.capacity() << "]" << std::endl;
  for (std::size_t index = 0; index < double_sub.size(); index++)
  {
    std::cout << "Subscriber message [" << index << "], value [" << *(double_sub.getMessage(index).data_) << "]." << std::endl;
  }
  std::cout << "Done!" << std::endl;
}