#include <gtest/gtest.h>

#include "cpp_concurrency_in_action/cpp_concurrency_in_action.hpp"

TEST(Greeting, IsNotEmpty)
{
    EXPECT_FALSE(cpp_concurrency_in_action::greeting().empty());
}
