#include <gtest/gtest.h>

#include <NetworkingCore/Socket.h>

namespace detail
{
  std::string prefix_with_length(std::string const& data)
  {
    int32_t len = data.size();

    // Create copy of the string with prefixed length.
    std::string prefixed;
    prefixed.resize(len + 4);
    // Copy the string.
    data.copy(prefixed.data() + 4, len);
    // Prefix with length.
    // First 4 bytes must correspond to the length of the message.
    std::memcpy(prefixed.data(), &len, 4);

    return prefixed;
  }
}

TEST(MessageBuilderTest, HandlesOneCompleteMessage) 
{
  // First create a message prefixed with length.
  const std::string message = "Hello World!";
  const auto prefixed = detail::prefix_with_length(message);

  // Build a message with builder.
  MessageBuilder builder;
  const auto messages = builder.build(prefixed);

  // We expect one message, equal to the original one.
  ASSERT_EQ(messages.size(), 1);
  EXPECT_EQ(messages.at(0), message);
}

TEST(MessageBuilderTest, HandlesMultipleCompleteMessagesOneByOne) 
{
  const std::vector<std::string> messages = { "Hello World!", "Foo Bar", "Random string."};

  // Create message builder.
  MessageBuilder builder;

  for (const auto msg : messages)
  {
    // Create message with length prefix.
    const auto prefixed = detail::prefix_with_length(msg);

    // Build message with builder.
    const auto built_messages = builder.build(prefixed);

    // We expect one message, equal to the original one.
    ASSERT_EQ(built_messages.size(), 1);
    EXPECT_EQ(built_messages.at(0), msg);
  }
}

TEST(MessageBuilderTest, HandlesMultipleCompleteMessagesAllAtOnce) 
{
  // Create one chunk of data from multiple messages.
  const std::vector<std::string> messages = { "Hello World!", "Foo Bar" };

  std::string prefixed_data;
  for (const auto& msg : messages)
  {
    auto prefixed = detail::prefix_with_length(msg);
    prefixed_data.append(prefixed);
  }

  // Build messages with builder.
  MessageBuilder builder;
  const auto final_messages = builder.build(prefixed_data);

  // We expect all original messages, each equal to the original one in the same order.
  ASSERT_EQ(messages.size(), final_messages.size());
  int i = 0;
  for (const auto& msg : messages)
  {
    EXPECT_EQ(msg, final_messages.at(i));
    ++i;
  }
}

TEST(MessageBuilderTest, HandlesOneMessageInTwoParts) 
{
  // First create a message prefixed with length.
  const std::string message = "Hello World!";
  const auto prefixed = detail::prefix_with_length(message);

  // Break message into two parts.
  const int break_point = prefixed.size() / 2;
  const auto first_part = prefixed.substr(0, break_point);
  const auto second_part = prefixed.substr(break_point, prefixed.size() - break_point);
  ASSERT_EQ(prefixed, first_part + second_part);

  // Create a builder. Build message in two parts.
  MessageBuilder builder;
  
  // Build first part. We expect that result is an empty vector of messages, since no message could be built.
  const auto result_1 = builder.build(first_part);
  EXPECT_EQ(result_1.size(), 0);
  
  // Build second part. We expect one message that equals the original message.
  const auto result_2 = builder.build(second_part);
  ASSERT_EQ(result_2.size(), 1);
  EXPECT_EQ(result_2.at(0), message);
}

TEST(MessageBuilderTest, HandlesOneMessageInMoreParts) 
{
  // First create a message prefixed with length.
  const std::string message = "This is a random string: !#$%?";
  const auto prefixed = detail::prefix_with_length(message);

  const std::vector<int> break_points = { 7, 14, 15, 24, 28 };

  // Create a builder. Build message in more parts.
  MessageBuilder builder;
  for (int i = 0; i <= break_points.size(); ++i)
  {
    const auto start = i == 0 ? 0 : break_points.at(i - 1);
    const auto end = i == break_points.size() ? prefixed.size() : break_points.at(i);
    const auto part = prefixed.substr(start, end - start);

    const auto result = builder.build(part);
    if (i == break_points.size())
    {
      // This is the last part. We expect the message to be reconstructed.
      ASSERT_EQ(result.size(), 1);
      EXPECT_EQ(result.at(0), message);
    }
    else
    {
      // We are not at the end. We expect no message to be reconstructed at this point.
      EXPECT_EQ(result.size(), 0);
    }
  }
}
