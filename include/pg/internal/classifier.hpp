#pragma once

namespace pg::internal {

template <int N>
struct Tag {
    char placeholder[N];
};

using PlainTag = Tag<1>;
using VisitableTag = Tag<2>;

template <typename T>
constexpr PlainTag classify(...) {
    return PlainTag{};
}

template <typename T>
constexpr VisitableTag classify(decltype(T::_POSTGRES_CXX_VISITABLE)) {
    return VisitableTag{};
}

template <typename T, typename U>
constexpr bool isTagged() {
    return sizeof(classify<T>(true)) == sizeof(U);
}

template <typename T>
constexpr bool isPlain() {
    return isTagged<T, PlainTag>();
}

template <typename T>
constexpr bool isVisitable() {
    return isTagged<T, VisitableTag>();
}

}  // namespace pg::internal
