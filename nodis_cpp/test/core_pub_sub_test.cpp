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

#include <gtest/gtest.h>

#include <iostream>

TEST(CorePubSubTest, core_pub_sub_test)
{
  // Set up the core.
  nodis_cpp::Core core;

  // Set up the publisher.
  auto double_pub = core.publisherIn<double>("/data_link");

  // Set up the subscriber.
  auto double_sub = core.subscriberIn<double>("/data_link", 10);
  EXPECT_EQ(10, double_sub.capacity());
  EXPECT_EQ(0, double_sub.size());
  

  // Publish some messages.
  ASSERT_NO_THROW(double_pub.publish(6.4));
  ASSERT_NO_THROW(double_pub.publish(3.6));
  ASSERT_NO_THROW(double_pub.publish(4.9));

  // Retrieve all messages.
  ASSERT_NO_THROW(double_sub.sync());
  EXPECT_EQ(10, double_sub.capacity());
  ASSERT_EQ(3, double_sub.size());
  EXPECT_EQ(6.4, *(double_sub.getMessage(0).data_));
  EXPECT_EQ(3.6, *(double_sub.getMessage(1).data_));
  EXPECT_EQ(4.9, *(double_sub.getMessage(2).data_));
  
  // Publish new messages.
  ASSERT_NO_THROW(double_pub.publish(1.9));
  ASSERT_NO_THROW(double_pub.publish(9.7));

  // Retrieve only new messages.
  ASSERT_NO_THROW(double_sub.syncNew());
  EXPECT_EQ(10, double_sub.capacity());
  ASSERT_EQ(2, double_sub.size());
  EXPECT_EQ(1.9, *(double_sub.getMessage(0).data_));
  EXPECT_EQ(9.7, *(double_sub.getMessage(1).data_));
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}