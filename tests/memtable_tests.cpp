//
// Created by Mengwen Li (ml4643) on 3/13/2021.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "memtable.h"
#include "serializer.h"

namespace projectdb {

namespace test {

class MemTableTestFixture : public MemTable, public ::testing::Test {};

TEST_F(MemTableTestFixture, SetGetRemoveTest) {
    Key key0("key0");
    Value value0("value0");
    set(key0, value0);
    auto rtn = getValue(key0);
    ASSERT_TRUE(rtn.has_value());
    EXPECT_EQ(rtn.value(), value0);
    // Set another value using the same key.
    Value value1("value1");
    set(key0, value1);
    rtn = getValue(key0);
    ASSERT_TRUE(rtn.has_value());
    EXPECT_EQ(rtn.value(), value1);
    // Remove with the same key. We should still have the entry in the map, just
    // with TOMBSTONE value.
    remove(key0);
    rtn = getValue(key0);
    ASSERT_TRUE(rtn.has_value());
    EXPECT_EQ(rtn.value(), Value());
    // Remove with a different key. We should have an additional entry in the
    // map.
    Key key1("key1");
    remove(key1);
    rtn = getValue(key1);
    ASSERT_TRUE(rtn.has_value());
    EXPECT_EQ(rtn.value(), Value());
}

}  // namespace test
}  // namespace projectdb
