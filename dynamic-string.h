#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#include <stddef.h>
#include <stdbool.h>

enum DStringError {
    DSTRING_SUCCESS = 0,
    DSTRING_NULL_POINTER,
    DSTRING_ALLOCATION_FAILURE,
    DSTRING_INVALID_ARGUMENT
};

typedef struct DynamicString *DString;

// 创建一个动态字符串。
[[nodiscard("Ignoring this return value will result in a resource leak.")]]
DString dstr_create(void);
// 销毁一个动态字符串，释放其占用的资源。
void dstr_destroy(DString dstr);
// 清空一个动态字符串的内容，不会释放内存，只是让其变为一个空字符串。
void dstr_clear(DString dstr);

// 获取一个动态字符串的内部 C 形式的字符串（const char 指针）。
const char *dstr_cstr(const DString dstr);
// 获取一个动态字符串的当前长度（不包括终止符）。
size_t dstr_length(const DString dstr);
// 获取一个动态字符串的当前容量。
size_t dstr_capacity(const DString dstr);

// 手动设置一个动态字符串的容量，设置过后，容量会维持在所设置的容量之上。
int dstr_reserve(DString dstr, size_t new_capacity);

// 将一个 C 字符串赋值给一个动态字符串。
int dstr_assign_cstr(DString dstr, const char *cstr);
// 将另一个动态字符串赋值给一个动态字符串。
int dstr_assign(DString dstr, const DString src_dstr);

// 在一个动态字符串末尾追加一个 C 字符串。
int dstr_append_cstr(DString dstr, const char *cstr);
// 在一个动态字符串末尾追加另一个动态字符串。
int dstr_append(DString dstr, const DString src_dstr);

// 在动态字符串的指定位置插入一个 C 字符串。
int dstr_insert_cstr(DString dstr, const char *cstr, size_t index);
// 在动态字符串的指定位置插入另一个动态字符串。
int dstr_insert(DString dstr, const DString src_dstr, size_t index);

// 从一个动态字符串的指定位置删除指定数量的字符。
int dstr_erase(DString dstr, size_t index, size_t count);

// 使用格式化字符串将内容赋值给一个动态字符串。
int dstr_assign_format(DString dstr, const char *format, ...);
// 使用格式化字符串在一个动态字符串末尾追加内容。
int dstr_append_format(DString dstr, const char *format, ...);
// 使用格式化字符串在一个动态字符串的指定位置插入内容。
int dstr_insert_format(DString dstr, size_t index, const char *format, ...);

// 判断一个动态字符串是否开始于一个 C 字符串
bool dstr_start_with_cstr(const DString dstr, const char *prefix);
// 判断一个动态字符串是否开始于另一个动态字符串
bool dstr_start_with(const DString dstr, const DString prefix);
// 判断一个动态字符串是否结束于一个 C 字符串
bool dstr_end_with_cstr(const DString dstr, const char *suffix);
// 判断一个动态字符串是否结束于另一个动态字符串
bool dstr_end_with(const DString dstr, const DString suffix);
// 判断一个动态字符串是否包含一个 C 字符串
bool dstr_contain_cstr(const DString dstr, const char *substr);
// 判断一个动态字符串是否包含另一个动态字符串
bool dstr_contain(const DString dstr, const DString substr);
// 判断一个动态字符串是否与一个 C 字符串相等
bool dstr_equal_cstr(const DString dstr, const char *cstr);
// 判断一个动态字符串是否与另一个动态字符串相等
bool dstr_equal(const DString dstr, const DString other);

// 查找一个 C 字符串在一个动态字符串中第一次出现的位置，返回 -1 表示未找到。
ssize_t dstr_find_cstr(const DString dstr, const char *substr);
// 查找一个动态字符串在另一个动态字符串中第一次出现的位置，返回 -1 表示未找到。
ssize_t dstr_find(const DString dstr, const DString substr);
// 查找一个 C 字符串在一个动态字符串中最后一次出现的位置，返回 -1 表示未找到。
ssize_t dstr_rfind_cstr(const DString dstr, const char *substr);
// 查找一个动态字符串在另一个动态字符串中最后一次出现的位置，返回 -1 表示未找到。
ssize_t dstr_rfind(const DString dstr, const DString substr);
// 统计一个 C 字符串在一个动态字符串中出现的次数。
size_t dstr_count_cstr(const DString dstr, const char *substr);
// 统计一个动态字符串在另一个动态字符串中出现的次数。
size_t dstr_count(const DString dstr, const DString substr);
// 返回一个 C 字符串在一个动态字符串中第 n 次出现的位置，返回 -1 表示未找到。
ssize_t dstr_find_nth_cstr(const DString dstr, const char *substr, size_t n);
// 返回一个动态字符串在另一个动态字符串中第 n 次出现的位置，返回 -1 表示未找到。
ssize_t dstr_find_nth(const DString dstr, const DString substr, size_t n);


#endif // DYNAMIC_STRING_H