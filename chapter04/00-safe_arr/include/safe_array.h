#pragma once
#include <stdbool.h> // for bool
#include <stddef.h>  // for size_t

// 1. safety, 要求提供一组接口, 确保任何数组的访问都在其合法范围内.
// 2. encapsulation, 封装性, 隐藏内部的原始数组, 要求用户(下属程序员)
// 只能通过这些接口来访问数组, 而不能直接访问内部数据.
// 3. usability 易用性 暴露给用户的API必须清晰直观.
// interface & information hiding
// --- Opaque Pointer Declaration 不透明指针 ---
// "安全数组"的魔法: 用户只知道有 SafeArray 这个类型, 而不知道 SafeArray
// 的内部实现. 但完全不知道它内部是用什么实现的, 它们无法直接访问内部的原始数组.
// Forward Declaration of SafeArray 向前声明
// 它向编译器声明 SafeArray 是一个结构体类型, 但不定义它的内容.
// 这样做的好处是, 用户无法直接访问 SafeArray 的内部成员数据,
// 因此我们可以定义指向它的指针 (或者叫做指向 SafeArray 的指针).
typedef struct SafeArray SafeArray;

// --- Public API Prototypes ---

/**
 * @brief 创建一个新的安全数组.
 *
 * @param capacity 数组可以容纳的元素数量
 * @return 成功时返回指向安全数组的指针, 失败返回 NULL.
 */
SafeArray *sarray_create(size_t capacity);

/**
 * @brief 销毁一个安全数组并释放所有内存.
 *
 * @param p_sarray 指向安全数组指针的指针, 函数会将其置为 NULL
 */
void sarray_destroy(SafeArray **p_sarray);

/**
 * @brief 安全地设置指定索引位置的值.
 *
 * @param sarray 指向要操作的安全数组的指针
 * @param index 要设置的索引位置
 * @param value 要设置的值
 * @return 成功返回 true, 如果索引越界或数组为NULL则返回 false.
 */
bool sarray_set(SafeArray *sarray, size_t index, int value);

/**
 * @brief 安全地获取指定索引位置的值。
 *
 * @param sarray 指向要操作的安全数组。
 * @param index  要获取的元素的索引。
 * @param out_value 指向一个整数的指针，用于接收获取到的值。out-parameter
 * 输出参数
 * @return 成功返回 true，如果索引越界或参数无效则返回 false。
 */
bool sarray_get(const SafeArray *sarray, size_t index, int *out_value);

/**
 * @brief 获取安全数组的容量。
 *
 * @param sarray 指向要查询的数组。
 * @return 返回数组的容量，如果数组为NULL则返回0。
 */
size_t sarray_get_capacity(const SafeArray *sarray);

/**
 * @brief 打印安全数组的内容。
 *
 * @param sarray 指向要打印的安全数组
 */
void sarray_print(const SafeArray *sarray);

