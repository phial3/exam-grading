#include "../exercise.h"
#include <cstring>
#include <numeric>


// READ: 类模板 <https://zh.cppreference.com/w/cpp/language/class_template>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;

    Tensor4D(unsigned int const shape_[4], T const *data_) {
        // TODO: 填入正确的 shape 并计算 size
        std::memcpy(shape, shape_, 4 * sizeof(unsigned int));
        auto size = std::accumulate(
            shape,
            shape + 4,
            1u,
            std::multiplies<unsigned int>());
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T));
    }
    ~Tensor4D() {
        delete[] data;
    }

    // 为了保持简单，禁止复制和移动
    Tensor4D(Tensor4D const &) = delete;
    Tensor4D(Tensor4D &&) noexcept = delete;

    // 这个加法需要支持“单向广播”。
    // 具体来说，`others` 可以具有与 `this` 不同的形状，形状不同的维度长度必须为 1。
    // `others` 长度为 1 但 `this` 长度不为 1 的维度将发生广播计算。
    // 例如，`this` 形状为 `[1, 2, 3, 4]`，`others` 形状为 `[1, 2, 1, 4]`，
    // 则 `this` 与 `others` 相加时，3 个形状为 `[1, 2, 1, 4]` 的子张量各自与 `others` 对应项相加。
    Tensor4D &operator+=(Tensor4D const &others) {
        // TODO: 实现单向广播的加法

        // 多维索引变化规则: 从右到左, 到达最大值后进位

        // 首先验证两个张量在维度上是否可单向广播
        for (int i = 0; i < 4; ++i) {
            if (shape[i] != others.shape[i] && others.shape[i] != 1) {
                // 对于本题的上下文，并没有要求报错处理。
                // 若需要，可在此抛出异常或断言失败。
                throw std::runtime_error("Shape mismatch");
            }
        }

        // 计算 `this` 的总元素数
        auto total_size = std::accumulate(
            shape,
            shape + 4,
            1u,
            std::multiplies<unsigned int>());

        // 我们需要根据线性索引计算 (i0, i1, i2, i3) 四维索引。
        // 对于给定线性索引 idx，以及 shape[4] = [D0, D1, D2, D3]：
        //
        // i0 = idx / (D1 * D2 * D3)
        // r0 = idx % (D1 * D2 * D3)
        // i1 = r0  / (D2 * D3)
        // r1 = r0  % (D2 * D3)
        // i2 = r1  / (D3)
        // i3 = r1  % (D3)

        auto D1 = shape[1],
             D2 = shape[2],
             D3 = shape[3];

        auto OD0 = others.shape[0],
             OD1 = others.shape[1],
             OD2 = others.shape[2],
             OD3 = others.shape[3];

        // others 的各维步长，用于线性索引转换
        auto O_stride0 = OD1 * OD2 * OD3,
             O_stride1 = OD2 * OD3,
             O_stride2 = OD3;

        // ODi == 1 时，对应维度长度为 1, 也就是单位一, 这相当于这个维度并不存在
        // 因此，我们可以将 ODi == 1 的维度索引设置为 0
        for (unsigned int idx = 0; idx < total_size; ++idx) {
            auto i0 = idx / (D1 * D2 * D3),
                 r0 = idx % (D1 * D2 * D3),
                 i1 = r0 / (D2 * D3),
                 r1 = r0 % (D2 * D3),
                 i2 = r1 / D3,
                 i3 = r1 % D3;

            // 确定 others 对应位置的索引
            auto j0 = (OD0 == 1 ? 0 : i0),
                 j1 = (OD1 == 1 ? 0 : i1),
                 j2 = (OD2 == 1 ? 0 : i2),
                 j3 = (OD3 == 1 ? 0 : i3);

            // 计算 others 的线性索引
            auto o_idx = j0 * O_stride0 + j1 * O_stride1 + j2 * O_stride2 + j3;

            this->data[idx] += others.data[o_idx];
        }

        return *this;
    }
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D(shape, data);
        auto t1 = Tensor4D(shape, data);
        t0 += t1;
        for (auto i = 0u; i < sizeof(data) / sizeof(*data); ++i) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}
