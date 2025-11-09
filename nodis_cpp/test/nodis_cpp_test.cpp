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

#include <gtest/gtest.h>

#include <iostream>

#include "nodis_cpp/core.h"

TEST(NodisCppTest, pub_sub_test)
{
  // Set up the core.
  nodis_cpp::Core core;

  // Set up the publisher.
  auto double_pub = core.publisher<double>("data_link");
  auto topic_info = core.topicInfo<double>("data_link");
  EXPECT_EQ(1, topic_info.publishers_);
  EXPECT_EQ(0, topic_info.subscribers_);
  EXPECT_EQ(1, topic_info.capacity_);

  // Set up the subscriber.
  auto double_sub = core.subscriber<double>("data_link", 10);
  EXPECT_EQ(10, double_sub.capacity());
  EXPECT_EQ(0, double_sub.size());
  topic_info = core.topicInfo<double>("data_link");
  EXPECT_EQ(1, topic_info.publishers_);
  EXPECT_EQ(1, topic_info.subscribers_);
  EXPECT_EQ(10, topic_info.capacity_);

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

  // Copy the publisher.
  auto double_pub_copy = double_pub;
  topic_info = core.topicInfo<double>("data_link");
  EXPECT_EQ(2, topic_info.publishers_);
  EXPECT_EQ(1, topic_info.subscribers_);
  EXPECT_EQ(10, topic_info.capacity_);

  // Publish another message.
  ASSERT_NO_THROW(double_pub_copy.publish(-4.4));

  // Check that the copied publisher's message showed up on the other end.
  ASSERT_NO_THROW(double_sub.syncNew());
  EXPECT_EQ(10, double_sub.capacity());
  ASSERT_EQ(1, double_sub.size());
  EXPECT_EQ(-4.4, *(double_sub.getMessage(0).data_));

  // Copy the subscriber.
  auto double_sub_copy = double_sub;
  topic_info = core.topicInfo<double>("data_link");
  EXPECT_EQ(2, topic_info.publishers_);
  EXPECT_EQ(2, topic_info.subscribers_);
  EXPECT_EQ(10, topic_info.capacity_);

  // Check that the copied subscriber can see past messages.
  ASSERT_NO_THROW(double_sub_copy.sync());
  EXPECT_EQ(10, double_sub_copy.capacity());
  ASSERT_EQ(6, double_sub_copy.size());
  EXPECT_EQ(6.4, *(double_sub_copy.getMessage(0).data_));
  EXPECT_EQ(3.6, *(double_sub_copy.getMessage(1).data_));
  EXPECT_EQ(4.9, *(double_sub_copy.getMessage(2).data_));
  EXPECT_EQ(1.9, *(double_sub_copy.getMessage(3).data_));
  EXPECT_EQ(9.7, *(double_sub_copy.getMessage(4).data_));
  EXPECT_EQ(-4.4, *(double_sub_copy.getMessage(5).data_));

  // Check that we didn't affect the original subscriber.
  EXPECT_EQ(10, double_sub.capacity());
  ASSERT_EQ(1, double_sub.size());
  EXPECT_EQ(-4.4, *(double_sub.getMessage(0).data_));

  // Move-copy the publisher.
  auto double_pub_move_copy = std::move(double_pub_copy);
  topic_info = core.topicInfo<double>("data_link");
  EXPECT_EQ(2, topic_info.publishers_);
  EXPECT_EQ(2, topic_info.subscribers_);
  EXPECT_EQ(10, topic_info.capacity_);

  // Publish another message.
  ASSERT_NO_THROW(double_pub_move_copy.publish(-6.7));

  // Check that the move copied publisher's message showed up on the other end.
  ASSERT_NO_THROW(double_sub.syncNew());
  EXPECT_EQ(10, double_sub.capacity());
  ASSERT_EQ(1, double_sub.size());
  EXPECT_EQ(-6.7, *(double_sub.getMessage(0).data_));

  // Move-copy the subscriber.
  auto double_sub_move_copy = std::move(double_sub_copy);
  topic_info = core.topicInfo<double>("data_link");
  EXPECT_EQ(2, topic_info.publishers_);
  EXPECT_EQ(2, topic_info.subscribers_);
  EXPECT_EQ(10, topic_info.capacity_);

  // Check that the move-copied subscriber only synced the new message.
  ASSERT_NO_THROW(double_sub_move_copy.syncNew());
  EXPECT_EQ(10, double_sub_move_copy.capacity());
  ASSERT_EQ(1, double_sub_move_copy.size());
  EXPECT_EQ(-6.7, *(double_sub_move_copy.getMessage(0).data_));

  // Assign the publisher.
  ASSERT_NO_THROW(double_pub_move_copy = double_pub);
  topic_info = core.topicInfo<double>("data_link");
  EXPECT_EQ(2, topic_info.publishers_);
  EXPECT_EQ(2, topic_info.subscribers_);
  EXPECT_EQ(10, topic_info.capacity_);

  // Publish another message.
  ASSERT_NO_THROW(double_pub_move_copy.publish(19.2));

  // Check that the assigned publisher's message showed up on the other end.
  ASSERT_NO_THROW(double_sub.syncNew());
  EXPECT_EQ(10, double_sub.capacity());
  ASSERT_EQ(1, double_sub.size());
  EXPECT_EQ(19.2, *(double_sub.getMessage(0).data_));

  // Assign the subscriber.
  ASSERT_NO_THROW(double_sub_move_copy = double_sub);
  topic_info = core.topicInfo<double>("data_link");
  EXPECT_EQ(2, topic_info.publishers_);
  EXPECT_EQ(2, topic_info.subscribers_);
  EXPECT_EQ(10, topic_info.capacity_);

  // Check that the assigned subscriber doesn't have any new messages.
  ASSERT_NO_THROW(double_sub.syncNew());
  EXPECT_EQ(10, double_sub.capacity());
  ASSERT_EQ(0, double_sub.size());

  // Check that an empty subscriber can still retrieve only new messages.
  ASSERT_NO_THROW(double_sub.syncNew());
  EXPECT_EQ(10, double_sub.capacity());
  ASSERT_EQ(0, double_sub.size());
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
