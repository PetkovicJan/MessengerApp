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