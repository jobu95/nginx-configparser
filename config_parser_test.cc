#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

/*
 * The following is copied from the 11 Jan lecture.
 */

// foo bar;
TEST(NginxConfigTest, ToString) {
  NginxConfigStatement statement;
  statement.tokens_.push_back("foo");
  statement.tokens_.push_back("bar");

  EXPECT_EQ(statement.ToString(0), "foo bar;\n");
}

class NginxStringConfigTest : public ::testing::Test {
  protected:
    bool ParseString(const std::string config_string) {
      std::stringstream config_stream(config_string);
      return parser_.Parse(&config_stream, &out_config_);
    }
    NginxConfigParser parser_;
    NginxConfig out_config_;
};

TEST_F(NginxStringConfigTest, SimpleConfigStream) {
  EXPECT_TRUE(ParseString("foo bar;"));
  EXPECT_EQ(1, out_config_.statements_.size()) << "Config has one statement";
  EXPECT_EQ("foo", out_config_.statements_[0]->tokens_[0]);
}

TEST_F(NginxStringConfigTest, InvalidConfig) {
  EXPECT_FALSE(ParseString("foo bar"));
}

TEST_F(NginxStringConfigTest, Nested) {
  EXPECT_TRUE(ParseString("baz { foo bar; }"));
}

/*
 * The bug we found in class.
 */
TEST_F(NginxStringConfigTest, UnmatchedBraces) {
  EXPECT_FALSE(ParseString("baz { foo bar; "));
}

/*
 * The following is my own work.
 */

TEST_F(NginxStringConfigTest, NestedInvalidConfig) {
  EXPECT_FALSE(ParseString("baz { foo }"));
  EXPECT_FALSE(ParseString("baz { foo bar }"));
}

/*
 * This test failed before my patch. Our parser did not allow for multiple
 * nested blocks.
 */
TEST_F(NginxStringConfigTest, DeeplyNested) {
  std::string nest = "";
  nest += "outer {\n";
  nest += "inner {\n";
  nest += "hello world;\n";
  nest += "}\n";
  nest += "}\n";
  EXPECT_TRUE(ParseString(nest));
}
