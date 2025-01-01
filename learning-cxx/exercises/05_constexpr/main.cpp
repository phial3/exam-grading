#include "../exercise.h"

struct Matrix2D {
    unsigned long long a, b, c, d;
    constexpr Matrix2D operator*(Matrix2D const &other) const {
        return {
            a * other.a + b * other.c,
            a * other.b + b * other.d,
            c * other.a + d * other.c,
            c * other.b + d * other.d};
    }
    constexpr Matrix2D operator^(int n) const {
        // iterative
        Matrix2D ans{1, 0, 0, 1};
        Matrix2D base = *this;
        while (n) {
            if (n & 1) {
                ans = ans * base;
            }
            base = base * base;
            n >>= 1;
        }
        return ans;
    }
};

constexpr unsigned long long fibonacci(int i) {
    // 矩阵快速幂实现
    Matrix2D base{1, 1, 1, 0};
    return (base ^ i).b;
}

int main(int argc, char **argv) {
    constexpr auto FIB20 = fibonacci(20);
    ASSERT(FIB20 == 6765, "fibonacci(20) should be 6765");
    std::cout << "fibonacci(20) = " << FIB20 << std::endl;

    // TODO: 观察错误信息，修改一处，使代码编译运行
    // PS: 编译运行，但是不一定能算出结果…… (直接上 constexpr + 迭代法矩阵快速幂, 轻松拿下)
    constexpr auto ANS_N = 100;
    auto ANS = fibonacci(ANS_N);
    std::cout << "fibonacci(" << ANS_N << ") = " << ANS << std::endl;

    return 0;
}
