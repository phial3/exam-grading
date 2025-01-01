#include "../exercise.h"
#include <cstdlib>

#ifdef __cplusplus
#if __cplusplus > 201703L
template<typename LeftType, typename RightType, typename OutputType>
concept Addable = requires(LeftType l, RightType r) {
    { l + r } -> std::convertible_to<OutputType>;
};

template<typename T>
concept SimpleAddable = Addable<T, T, T>;
#endif
#endif

// READ: 函数模板 <https://zh.cppreference.com/w/cpp/language/function_template>
// TODO: 将这个函数模板化
template<typename T>
#ifdef __cplusplus
#if __cplusplus > 201703L
    requires SimpleAddable<T>
#endif
#endif
auto plus(T a, T b) {
    return a + b;
}

template<typename T>
#ifdef __cplusplus
#if __cplusplus > 201703L
    requires SimpleAddable<T>
#endif
#endif
auto abs(T a, T b) {
    if (a > b) {
        return a - b;
    } else {
        return b - a;
    }
}

int main(int argc, char **argv) {
    ASSERT(plus(1, 2) == 3, "Plus two int");
    ASSERT(plus(1u, 2u) == 3u, "Plus two unsigned int");

    // THINK: 浮点数何时可以判断 ==？何时必须判断差值？
    ASSERT(plus(1.25f, 2.5f) == 3.75f, "Plus two float");
    ASSERT(plus(1.25, 2.5) == 3.75, "Plus two double");
    // TODO: 修改判断条件使测试通过
    ASSERT(abs(plus(0.1, 0.2), 0.3) < 1e-6, "How to make this pass?");

    return 0;
}
