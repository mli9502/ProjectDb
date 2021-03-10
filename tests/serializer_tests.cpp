//
// Created by Mengwen Li (ml4643) on 3/9/2021.
//

#include <gmock/gmock.h>

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

using TrivialWrapperTypes =
    ::testing::Types<TrivialWrapper<int>, TrivialWrapper<unsigned>,
                     TrivialWrapper<double>, TrivialWrapper<bool>,
                     TrivialWrapper<Value::Type>,
                     TrivialWrapper<TrivialStruct>>;

template <Serializable T>
class TrivialWrapperTestFixture : public ::testing::Test {
   public:
    static T m_trivialWrapper;
};

TYPED_TEST_SUITE(TrivialWrapperTestFixture, TrivialWrapperTypes);

// https://github.com/google/googletest/blob/master/docs/advanced.md#typed-tests
// http://www.cs.technion.ac.il/users/yechiel/c++-faq/nondependent-name-lookup-members.html
TYPED_TEST(TrivialWrapperTestFixture, RoundtripTest) {
    stringstream ss;
    EXPECT_NO_THROW(
        TrivialWrapperTestFixture<TypeParam>::m_trivialWrapper.serialize(ss));
    typename TypeParam::value_type expected;
    EXPECT_NO_THROW(expected = TypeParam().deserialize(ss));
    EXPECT_EQ(expected,
              TrivialWrapperTestFixture<TypeParam>::m_trivialWrapper.get());
}

// https://stackoverflow.com/questions/8507385/google-test-is-there-a-way-to-combine-a-test-which-is-both-type-parameterized-a
template <>
TrivialWrapper<int>
    TrivialWrapperTestFixture<TrivialWrapper<int>>::m_trivialWrapper{-5};
template <>
TrivialWrapper<unsigned>
    TrivialWrapperTestFixture<TrivialWrapper<unsigned>>::m_trivialWrapper{5};
template <>
TrivialWrapper<double>
    TrivialWrapperTestFixture<TrivialWrapper<double>>::m_trivialWrapper{5.5};
template <>
TrivialWrapper<bool>
    TrivialWrapperTestFixture<TrivialWrapper<bool>>::m_trivialWrapper{true};
template <>
TrivialWrapper<Value::Type>
    TrivialWrapperTestFixture<TrivialWrapper<Value::Type>>::m_trivialWrapper{
        Value::Type::TOMBSTONE_VALUE};
template <>
TrivialWrapper<TrivialStruct>
    TrivialWrapperTestFixture<TrivialWrapper<TrivialStruct>>::m_trivialWrapper{
        TrivialStruct(1, 2)};

}  // namespace projectdb
