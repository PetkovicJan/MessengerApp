#include <gtest/gtest.h>

#include <NetworkingCore/Socket.h>

TEST(MessageBuilderTest, HandlesMessage) 
{
  // First create a message prefixed with length. They are expected to be sent in this form.
  std::string const prefixed = MessageBuilder::prefixWithLength("Hello World!");

  // Build a message with builder.
  MessageBuilder builder;
  auto const messages = builder.build(prefixed);

  // We expect one message, equal to the original one.
  ASSERT_EQ(messages.size(), 1);
  EXPECT_EQ(messages[0], "Hello World!");
}

TEST(MessageBuilderTest, HandlesMultipleMessagesSeparately) 
{
  std::vector<std::string> const messages = { "Hello World!", "Foo Bar", "Random string."};

  MessageBuilder builder;

  for (auto const& msg : messages) {
    std::string const prefixed = MessageBuilder::prefixWithLength(msg);
    auto const messages = builder.build(prefixed);

    // We expect one message, equal to the original one.
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0], msg);
  }
}

TEST(MessageBuilderTest, HandlesMultipleMessagesAtOnce) 
{
  std::vector<std::string> const messages = { "Hello World!", "Foo Bar" };

  // Create one chunk of data from multiple messages.
  std::string joinedData;
  for (auto const& msg : messages) {
    std::string const prefixed = MessageBuilder::prefixWithLength(msg);
    joinedData.append(prefixed);
  }

  MessageBuilder builder;
  const auto result = builder.build(joinedData);

  // We expect all original messages, each equal to the original one in the same order.
  ASSERT_EQ(messages.size(), result.size());

  for(std::size_t i = 0; i < messages.size(); ++i) {
    EXPECT_EQ(messages[i], result[i]);
  }
}

TEST(MessageBuilderTest, HandlesMessageInTwoParts) 
{
  std::string const message = "Hello World!";
  std::string const prefixed = MessageBuilder::prefixWithLength(message);

  // Break message into two parts.
  int const breakPoint = prefixed.size() / 2;
  std::string const firstPart = prefixed.substr(0, breakPoint);
  std::string const second_part = prefixed.substr(breakPoint, prefixed.size() - breakPoint);
  ASSERT_EQ(prefixed, firstPart + second_part);

  // Create a builder. Build message in two parts.
  MessageBuilder builder;
  
  // Build first part. We expect that result is an empty vector of messages, 
  // since no message could be built from partial data.
  auto const result1 = builder.build(firstPart);
  EXPECT_EQ(result1.size(), 0);
  
  // Build second part. We expect one message that equals the original message.
  auto const result2 = builder.build(second_part);
  ASSERT_EQ(result2.size(), 1);
  EXPECT_EQ(result2[0], message);
}

TEST(MessageBuilderTest, HandlesMessageInMoreParts) 
{
  std::string const message = "This is a random string: !#$%?";
  std::string const prefixed = MessageBuilder::prefixWithLength(message);

  std::vector<std::size_t> const breakPoints = { 7, 14, 15, 24, 28 };

  // Create a builder. Build message in more parts.
  MessageBuilder builder;

  for (std::size_t i = 0; i <= breakPoints.size(); ++i) {
    std::size_t const begin = i == 0 ? 0 : breakPoints.at(i - 1);
    std::size_t const end = i == breakPoints.size() ? prefixed.size() : breakPoints.at(i);
    std::string const partialMsg = prefixed.substr(begin, end - begin);

    auto const result = builder.build(partialMsg);

    bool const isFinalPart = i == breakPoints.size();
    if (isFinalPart) {
      // This was the last part of the message. We expect the message to be reconstructed.
      ASSERT_EQ(result.size(), 1);
      EXPECT_EQ(result[0], message);
    }
    else {
      // This was not the last part of the message. It should not be reconstructed at this point.
      EXPECT_EQ(result.size(), 0);
    }
  }
}
