//
// Created by Mengwen Li (ml4643) on 3/9/2021.
//

#include <gmock/gmock.h>

#include "key.h"
#include "serializer.h"
#include "value.h"

using namespace std;

namespace projectdb {

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

bool operator!=(const TrivialStruct& lhs, const TrivialStruct& rhs) {
    return !(lhs == rhs);
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
}  // namespace

using SerializationWrapperTypes =
    ::testing::Types<int, unsigned, double, bool, Value::Type, TrivialStruct,
                     Key, Value>;

template <typename T>
class SerializationWrapperTestFixture : public ::testing::Test {
   public:
    static SerializationWrapper<T> m_serializationWrapper;
};

TYPED_TEST_SUITE(SerializationWrapperTestFixture, SerializationWrapperTypes);

// https://github.com/google/googletest/blob/master/docs/advanced.md#typed-tests
// http://www.cs.technion.ac.il/users/yechiel/c++-faq/nondependent-name-lookup-members.html
TYPED_TEST(SerializationWrapperTestFixture, RoundtripTest) {
    stringstream ss;
    // Make a copy of m_t because it might be moved latter.
    auto expected =
        SerializationWrapperTestFixture<TypeParam>::m_serializationWrapper.m_t;
    EXPECT_NO_THROW(
        move(SerializationWrapperTestFixture<TypeParam>::m_serializationWrapper)
            .serialize(ss));
    TypeParam deserialized;
    EXPECT_NO_THROW(deserialized =
                        SerializationWrapper<TypeParam>().deserialize(ss));
    EXPECT_EQ(expected, deserialized);
}

// https://stackoverflow.com/questions/8507385/google-test-is-there-a-way-to-combine-a-test-which-is-both-type-parameterized-a
template <>
SerializationWrapper<int>
    SerializationWrapperTestFixture<int>::m_serializationWrapper{-5};
template <>
SerializationWrapper<unsigned>
    SerializationWrapperTestFixture<unsigned>::m_serializationWrapper{5};
template <>
SerializationWrapper<double>
    SerializationWrapperTestFixture<double>::m_serializationWrapper{5.5};
template <>
SerializationWrapper<bool>
    SerializationWrapperTestFixture<bool>::m_serializationWrapper{true};
template <>
SerializationWrapper<Value::Type>
    SerializationWrapperTestFixture<Value::Type>::m_serializationWrapper{
        Value::Type::TOMBSTONE_VALUE};
template <>
SerializationWrapper<TrivialStruct>
    SerializationWrapperTestFixture<TrivialStruct>::m_serializationWrapper{
        TrivialStruct(1, 2)};

/**
 * TODO: @mli: How do we make it possible to have multiple values for Key for
 * testing? Maybe we need another TYPED_TEST that takes vector<T> as types?
 */
template <>
SerializationWrapper<Key>
    SerializationWrapperTestFixture<Key>::m_serializationWrapper{
        Key("Hello World Key!")};

template <>
SerializationWrapper<Value>
    SerializationWrapperTestFixture<Value>::m_serializationWrapper{
        Value("Hello World Value!")};

}  // namespace projectdb
