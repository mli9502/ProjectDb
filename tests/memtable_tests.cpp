//
// Created by Mengwen Li (ml4643) on 3/13/2021.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "memtable.h"
#include "serializer.h"

namespace projectdb {

namespace test {

class MemTableTestFixture : public MemTable, public ::testing::Test {
   public:
    map<key_type, mapped_type> getMemTable() { return m_memTable; }
};

TEST_F(MemTableTestFixture, SetGetRemoveTest) {
    Key key0("key0");
    Value value0("value0");
    set(key0, value0);
    auto rtn = getValueEntry(key0);
    ASSERT_TRUE(rtn.has_value());
    EXPECT_EQ(rtn.value(), value0);
    // Set another value using the same key.
    Value value1("value1");
    set(key0, value1);
    rtn = getValueEntry(key0);
    ASSERT_TRUE(rtn.has_value());
    EXPECT_EQ(rtn.value(), value1);
    // Remove with the same key. We should still have the entry in the map, just
    // with TOMBSTONE value.
    remove(key0);
    rtn = getValueEntry(key0);
    ASSERT_TRUE(rtn.has_value());
    EXPECT_EQ(rtn.value(), Value());
    // However, if we call getValue(), then it should be empty.
    auto valRtn = getValue(key0);
    EXPECT_FALSE(valRtn.has_value());
    // Remove with a different key. We should have an additional entry in the
    // map.
    Key key1("key1");
    remove(key1);
    rtn = getValueEntry(key1);
    ASSERT_TRUE(rtn.has_value());
    EXPECT_EQ(rtn.value(), Value());
    valRtn = getValue(key1);
    EXPECT_FALSE(valRtn.has_value());
}

TEST_F(MemTableTestFixture, RoundtripTest) {
    MemTable expected;
    expected.set(Key("key0"), Value("value0"));
    expected.set(Key("key1"), Value("value1"));
    expected.remove(Key("key2"));
    stringstream ss;
    EXPECT_NO_THROW(SerializationWrapper<MemTable>(expected).serialize(ss));
    MemTable deserialized;
    EXPECT_NO_THROW(deserialized =
                        SerializationWrapper<MemTable>().deserialize(ss));
    EXPECT_EQ(expected, deserialized);
}

}  // namespace test
}  // namespace projectdb
