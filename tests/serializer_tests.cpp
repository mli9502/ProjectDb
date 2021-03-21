//
// Created by Mengwen Li (ml4643) on 3/9/2021.
//

#include <gmock/gmock.h>

#include "key.h"
#include "memtable.h"
#include "serializer.h"
#include "value.h"

using namespace std;

namespace projectdb {

namespace test {

namespace {

struct TrivialStruct {
   public:
    static const int N = 3;

    TrivialStruct() = default;

    TrivialStruct(int a, int b) : m_a(a), m_b(b), m_c{1.1, 2.2, 3.3} {}

    // NOTE: Providing brace-or-equal-initializers makes a class non-trivial.
    // https://arne-mertz.de/2015/08/new-c-features-default-initializers-for-member-variables/
    int m_a;
    int m_b;
    double m_c[N];
};

bool operator==(const TrivialStruct& lhs, const TrivialStruct& rhs) {
    return (lhs.m_a == rhs.m_a) && (lhs.m_b == rhs.m_b);
}

ostream& operator<<(ostream& os, const TrivialStruct& t) {
    os << "{" << t.m_a << ", " << t.m_b;
    for (auto i = 0; i < TrivialStruct::N; i++) {
        os << t.m_c[i];
        if (i != TrivialStruct::N - 1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}

struct NotSerializableStruct {
    vector<string> m_vec;
};

}  // namespace

/**
 * Tests to make sure that the following types are Serializable.
 */
// clang-format off
using SerializableTypes = ::testing::Types<int,
                                            Value::Type,
                                            TrivialStruct,
                                            Key,
                                            Value,
                                            pair<int, string>,
                                            pair<Key, Value>,
                                            pair<pair<Key, Value>, pair<int, string>>,
                                            vector<int>,
                                            vector<Key>,
                                            vector<Value>,
                                            map<int, string>,
                                            map<Key, Value>,
                                            map<Key, vector<Value>>,
                                            pair<map<Key, vector<Value>>, pair<map<int, string>, vector<Value>>>>;
// clang-format on

template <typename T>
class SerializableConceptTestFixture : public ::testing::Test {};

TYPED_TEST_SUITE(SerializableConceptTestFixture, SerializableTypes);

TYPED_TEST(SerializableConceptTestFixture, SerializableTypesTest) {
    EXPECT_TRUE(Serializable<TypeParam>);
}

/**
 * Tests to make sure that the following types are NOT Serializable.
 */
using NotSerializableTypes =
    ::testing::Types<NotSerializableStruct, pair<int, NotSerializableStruct>,
                     vector<NotSerializableStruct>>;

template <typename T>
class NotSerializableConceptTestFixture : public ::testing::Test {};

TYPED_TEST_SUITE(NotSerializableConceptTestFixture, NotSerializableTypes);

TYPED_TEST(NotSerializableConceptTestFixture, NotSerializableTypesTest) {
    EXPECT_FALSE(Serializable<TypeParam>);
}

// clang-format off
using SerializationWrapperTypes =
    ::testing::Types<int,
                    unsigned,
                    double,
                    bool,
                    Value::Type,
                    TrivialStruct,
                    Key,
                    Value>;
// clang-format on

template <typename T>
class SerializationWrapperTestFixture : public ::testing::Test {
   public:
    static T m_t;
};

TYPED_TEST_SUITE(SerializationWrapperTestFixture, SerializationWrapperTypes);

// https://github.com/google/googletest/blob/master/docs/advanced.md#typed-tests
// http://www.cs.technion.ac.il/users/yechiel/c++-faq/nondependent-name-lookup-members.html
TYPED_TEST(SerializationWrapperTestFixture, RoundtripTest) {
    stringstream ss;
    // Make a copy of m_t because it might be moved latter.
    auto expected = SerializationWrapperTestFixture<TypeParam>::m_t;
    EXPECT_NO_THROW(SerializationWrapper<TypeParam>{expected}(ss));
    TypeParam deserialized;
    EXPECT_NO_THROW(deserialized = DeserializationWrapper<TypeParam>{}(ss));
    EXPECT_EQ(expected, deserialized);
}

// https://stackoverflow.com/questions/8507385/google-test-is-there-a-way-to-combine-a-test-which-is-both-type-parameterized-a
template <>
int SerializationWrapperTestFixture<int>::m_t{-5};
template <>
unsigned SerializationWrapperTestFixture<unsigned>::m_t{5};
template <>
double SerializationWrapperTestFixture<double>::m_t{5.5};
template <>
bool SerializationWrapperTestFixture<bool>::m_t{true};
template <>
Value::Type SerializationWrapperTestFixture<Value::Type>::m_t{
    Value::Type::TOMBSTONE_VALUE};
template <>
TrivialStruct SerializationWrapperTestFixture<TrivialStruct>::m_t{
    TrivialStruct(1, 2)};

/**
 * TODO: @mli: How do we make it possible to have multiple values for Key for
 * testing? Maybe we need another TYPED_TEST that takes vector<T> as types?
 */
template <>
Key SerializationWrapperTestFixture<Key>::m_t{Key("Hello World Key!")};

template <>
Value SerializationWrapperTestFixture<Value>::m_t{Value("Hello World Value!")};

}  // namespace test

}  // namespace projectdb
