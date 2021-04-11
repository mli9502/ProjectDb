#include <gmock/gmock.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "projectdb/projectdb.h"
#include "test_utils.h"

using namespace std;

namespace projectdb {

namespace test {

class SmokeTestFixture : public ::testing::Test {
   public:
    void SetUp() override {
        for (unsigned i = 0; i < NUM_ELEMENTS; i++) {
            m_kvs.emplace_back(pair<string, string>{
                to_string(i), to_string(i) + "-Hello World!"});
        }

        // Set the config for testing.
        apply_testing_db_config();
    }

    void TearDown() override { try_remove_db_dir(); }

   protected:
    // NOTE: @mli:
    // We can't make ProjectDb as a member because try_remove_db_dir will be
    // called before ProjectDb dtor, causing directory not found issue.
    vector<pair<string, string>> m_kvs;

    static const unsigned NUM_ELEMENTS = 500;
};

TEST_F(SmokeTestFixture, SetGetTest) {
    ProjectDb db;
    for (const auto& [k, v] : m_kvs) {
        ASSERT_NO_THROW(db.set(k, v));
    }

    for (const auto& [k, v] : m_kvs) {
        optional<string> rtn;
        ASSERT_NO_THROW(rtn = db.get(k));
        ASSERT_TRUE(rtn.has_value());
        EXPECT_EQ(rtn.value(), v);
    }
}

TEST_F(SmokeTestFixture, SetRemoveAllGetTest) {
    ProjectDb db;
    for (const auto& [k, v] : m_kvs) {
        ASSERT_NO_THROW(db.set(k, v));
    }

    for (const auto& [k, _] : m_kvs) {
        ASSERT_NO_THROW(db.remove(k));
    }

    for (const auto& [k, v] : m_kvs) {
        optional<string> rtn;
        ASSERT_NO_THROW(rtn = db.get(k));
        EXPECT_FALSE(rtn.has_value());
    }
}

TEST_F(SmokeTestFixture, SetRemoveSomeGetTest) {
    ProjectDb db;
    for (const auto& [k, v] : m_kvs) {
        ASSERT_NO_THROW(db.set(k, v));
    }

    unsigned counter = 0;
    const unsigned keysToRemove = floor(m_kvs.size() / 2);

    for (const auto& [k, _] : m_kvs) {
        ASSERT_NO_THROW(db.remove(k));
        counter++;
        if (counter > keysToRemove) {
            break;
        }
    }

    counter = 0;
    for (const auto& [k, v] : m_kvs) {
        optional<string> rtn;
        ASSERT_NO_THROW(rtn = db.get(k));
        if (counter > keysToRemove) {
            ASSERT_TRUE(rtn.has_value());
            EXPECT_EQ(rtn.value(), v);
        } else {
            EXPECT_FALSE(rtn.has_value());
        }
        counter++;
    }
}

}  // namespace test

}  // namespace projectdb
