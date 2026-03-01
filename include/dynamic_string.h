//
// Created by mtueih on 2026/2/25.
//

#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#include <stdbool.h>
#include <stddef.h>

#if defined(__GNUC__) || defined(__clang__)
    #define DARR_NODISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
    #define DARR_NODISCARD _Check_return_
#else
    #define DARR_NODISCARD
#endif


/**
 * 「动态字符串」类型。
 */
typedef struct dynamic_string dstr_t;

/**
 * 创建一个「动态字符串」。
 *
 * @param cstr 用来初始化「动态字符串」的 C 字符串。
 *  为 NULL 表示创建空「动态字符串」。
 *
 * @return 「动态字符串」指针。
 *  为 NULL 表示创建失败。
 *
 */
DARR_NODISCARD
dstr_t *dstr_create(const char *cstr);

/**
 * 销毁一个「动态字符串」。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 */
void dstr_destroy(dstr_t *p_dstr);

/**
 * 清空一个「动态字符串」的内容。
 *  不会修改容量，只是让其变为一个空字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 */
void dstr_clear(dstr_t *p_dstr);

/**
 * 获取一个「动态字符串」的 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @return C 字符串。
 *
 */
const char *dstr_cstr(const dstr_t *p_dstr);

/**
 * 获取一个「动态字符串」的长度。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @return 长度。
 *
 */
size_t dstr_length(const dstr_t *p_dstr);

/**
 * 获取一个「动态字符串」的容量。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @return 容量。
 *
 */
size_t dstr_capacity(const dstr_t *p_dstr);

/**
 * 设置一个「动态字符串」的容量。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param new_length 新容量。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_resize(dstr_t *p_dstr, size_t new_length);

/**
 * 将一个 C 字符串赋值给一个「动态字符串」。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param cstr C 字符串。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_assign_cstr(dstr_t *p_dstr, const char *cstr);

/**
 * 将一个「动态字符串」赋值给另一个「动态字符串」。
 *
 * @param p_dst_dstr 目标「动态字符串」指针。
 *
 * @param p_src_dstr  源「动态字符串」指针。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_assign(dstr_t *p_dst_dstr, const dstr_t *p_src_dstr);

/**
 * 将一个 C 字符串追加到一个「动态字符串」后面。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param cstr C 字符串。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_append_cstr(dstr_t *p_dstr, const char *cstr);

/**
 * 将一个「动态字符串」追加到另一个「动态字符串」后面。
 *
 * @param p_dst_dstr 目标「动态字符串」指针。
 *
 * @param p_src_dstr 源「动态字符串」指针。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_append(dstr_t *p_dst_dstr, const dstr_t *p_src_dstr);

/**
 * 将一个 C 字符串插入到一个「动态字符串」的指定索引处。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param cstr C 字符串。
 *
 * @param index 插入位置的索引。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_insert_cstr(dstr_t *p_dstr, const char *cstr, size_t index);

/**
 * 将一个「动态字符串」插入到另一个「动态字符串」的指定索引处。
 *
 * @param p_dst_dstr 目标「动态字符串」指针。
 *
 * @param p_src_dstr 源「动态字符串」字符串。
 *
 * @param index 插入位置的索引。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_insert(dstr_t *p_dst_dstr, const dstr_t *p_src_dstr, size_t index);

/**
 * 从一个「动态字符串」中，删除从指定索引出开始，向后的指定数量的字符。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param index 起始位置的索引。
 *
 * @param quantity 删除的数量，为 0 表示删除至末尾。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_remove(dstr_t *p_dstr, size_t index, size_t quantity);

/**
 * 写入一个格式化字符串到一个「动态字符串」中。
 * @param p_dstr 「动态字符串」指针。
 *
 * @param format 格式字符串。
 *
 * @param ... 可变参数列表。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_printf(dstr_t *p_dstr, const char *format, ...);

/**
 * 从一个 C 字符串中提取一个子字符串，并写入一个「动态字符串」中。
 *
 * @param p_dstr 目标「动态字符串」指针。
 *
 * @param cstr 被提取 C 字符串。
 *
 * @param index 提取的起始位置的索引。
 *
 * @param quantity 提取的字符数量，为 0 表示提取至末尾。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_sub_cstr(dstr_t *p_dstr, const char *cstr, size_t index, size_t quantity);

/**
 * 从一个「动态字符串」中提取一个子字符串，并写入另一个「动态字符串」中。
 *
 * @param p_dst_dstr 目标「动态字符串」指针。
 *
 * @param p_src_dstr 被提取「动态字符串」。
 *
 * @param index 提取的起始位置的索引。
 *
 * @param quantity 提取的字符数量，为 0 表示提取至末尾。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
int dstr_sub(dstr_t *p_dst_dstr, const dstr_t *p_src_dstr, size_t index, size_t quantity);

/**
 * 将一个「动态字符串」中出现一次过多次的 C 字符串，替换为另一个 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param old_cstr 原 C 字符串。
 *
 * @param new_cstr 新 C 字符串。
 *
 * @param n 替换的次数，为 0 表示替换全部。
 *
 * @param backward 是否反向替换（从右向左替换）。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
size_t dstr_replace_cstr(dstr_t *p_dstr, const char *old_cstr, const char *new_cstr, size_t n, bool backward);

/**
 * 将一个「动态字符串」中出现一次过多次的子「动态字符串」，替换为另一个「动态字符串」。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param p_old_dstr 原「动态字符串」。
 *
 * @param p_new_dstr 新「动态字符串」。
 *
 * @param n 替换的次数，为 0 表示替换全部。
 *
 * @param backward 是否反向替换（从右向左替换）。
 *
 * @return 0 表示成功，非 0 表示失败：
 *  EINVAL：代表参数错误；
 *  ENOMEM：代表内存分配失败。
 *
 */
size_t dstr_replace(dstr_t *p_dstr, const dstr_t *p_old_dstr, const dstr_t *p_new_dstr, size_t n, bool backward);

/**
 * 判断一个「动态字符串」是否开始于一个 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param prefix 前缀 C 字符串。
 *
 */
bool dstr_starts_with_cstr(const dstr_t *p_dstr, const char *prefix);

/**
 * 判断一个「动态字符串」是否开始于另一个「动态字符串」。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param prefix 前缀「动态字符串」。
 *
 */
bool dstr_starts_with(const dstr_t *p_dstr, const dstr_t *prefix);

/**
 * 判断一个「动态字符串」是否结束于一个 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param suffix 后缀 C 字符串。
 *
 */
bool dstr_ends_with_cstr(const dstr_t *p_dstr, const char *suffix);

/**
 * 判断一个「动态字符串」是否结束于另一个「动态字符串」。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param suffix 后缀「动态字符串」。
 *
 */
bool dstr_ends_with(const dstr_t *p_dstr, const dstr_t *suffix);

/**
 * 判断一个「动态字符串」是否包含一个 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param sub_cstr 包含的 C 字符串。
 *
 */
bool dstr_contains_cstr(const dstr_t *p_dstr, const char *sub_cstr);

/**
 * 判断一个「动态字符串」是否包含另一个「动态字符串」。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param p_sub_dstr 包含的「动态字符串」。
 *
 */
bool dstr_contains(const dstr_t *p_dstr, const dstr_t *p_sub_dstr);

/**
 * 判断一个「动态字符串」是否等于一个 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param cstr C 字符串。
 *
 */
bool dstr_equals_cstr(const dstr_t *p_dstr, const char *cstr);

/**
 * 判断一个「动态字符串」是否等于另一个「动态字符串」。
 *
 * @param p_dstr_1 「动态字符串」1 的指针。
 *
 * @param p_dstr_2 「动态字符串」2 的指针。
 *
 */
bool dstr_equals(const dstr_t *p_dstr_1, const dstr_t *p_dstr_2);

/**
 * 比较一个「动态字符串」和一个 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param cstr  C 字符串。
 *
 * @return 为 0 表示两者相等；为正数表示前者 > 后者；为负数表示前者 < 后者。
 *
 */
int dstr_compare_cstr(const dstr_t *p_dstr, const char *cstr);

/**
 * 比较两个「动态字符串」。
 *
 * @param p_dstr_1 「动态字符串」1 的指针。
 *
 * @param p_dstr_2 「动态字符串」2 的指针。
 *
 * @return 为 0 表示两者相等；为正数表示前者 > 后者；为负数表示前者 < 后者。
 *
 */
int dstr_compare(const dstr_t *p_dstr_1, const dstr_t *p_dstr_2);

/**
 * 在一个「动态字符串」中查找一个 C 字符串第一次出现的位置。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param sub_cstr 要查找的 C 字符串。
 *
 * @param p_index 指向一个 size_t 变量，该变量用于存储查找到的位置索引。
 *  如果找到，函数返回 true，并将出现的位置索引写入该指针指向的变量；
 *  如果没找到，函数返回 false，并不会修改该指针指向的值。
 *
 * @return 表示是否查找到了。
 *
 */
bool dstr_find_cstr(const dstr_t *p_dstr, const char *sub_cstr, size_t *p_index);

/**
 * 在一个「动态字符串」中查找另一个「动态字符串」第一次出现的位置。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param p_sub_dstr 要查找的「动态字符串」的指针。
 *
 * @param p_index 指向一个 size_t 变量，该变量用于存储查找到的位置索引。
 *  如果找到，函数返回 true，并将出现的位置索引写入该指针指向的变量；
 *  如果没找到，函数返回 false，并不会修改该指针指向的值。
 *
 * @return 表示是否查找到了。
 *
 */
bool dstr_find(const dstr_t *p_dstr, const dstr_t *p_sub_dstr, size_t *p_index);

/**
 * 在一个「动态字符串」中查找一个 C 字符串最后一次出现的位置（从右往左查找）。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param sub_cstr 要查找的 C 字符串。
 *
 * @param p_index 指向一个 size_t 变量，该变量用于存储查找到的位置索引。
 *  如果找到，函数返回 true，并将出现的位置索引写入该指针指向的变量；
 *  如果没找到，函数返回 false，并不会修改该指针指向的值。
 *
 * @return 表示是否查找到了。
 *
 */
bool dstr_rfind_cstr(const dstr_t *p_dstr, const char *sub_cstr, size_t *p_index);

/**
 * 在一个「动态字符串」中查找另一个「动态字符串」最后一次出现的位置（从右往左查找）。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param p_sub_dstr 要查找的「动态字符串」的指针。
 *
 * @param p_index 指向一个 size_t 变量，该变量用于存储查找到的位置索引。
 *  如果找到，函数返回 true，并将出现的位置索引写入该指针指向的变量；
 *  如果没找到，函数返回 false，并不会修改该指针指向的值。
 *
 * @return 表示是否查找到了。
 *
 */
bool dstr_rfind(const dstr_t *p_dstr, const dstr_t *p_sub_dstr, size_t *p_index);

/**
 * 统计一个「动态字符串」中，一个 C 字符串出现的次数。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param sub_cstr 要查找的子 C 字符串。
 *
 * @return 出现的次数。
 *
 */
size_t dstr_count_cstr(const dstr_t *p_dstr, const char *sub_cstr);

/**
 * 统计一个「动态字符串」中，另一个「动态字符串」出现的次数。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param p_sub_dstr 要查找的子「动态字符串」的指针。
 *
 * @return 出现的次数。
 *
 */
size_t dstr_count(const dstr_t *p_dstr, const dstr_t *p_sub_dstr);

/**
 * 获取一个「动态字符串」中，一个 C 字符串第 n 次出现的位置索引。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param sub_cstr 要查找的子 C 字符串。
 *
 * @param p_index 指向一个 size_t 变量，该变量用于存储查找到的位置索引。
 *  如果找到，函数返回 true，并将出现的位置索引写入该指针指向的变量；
 *  如果没找到，函数返回 false，并不会修改该指针指向的值。
 *
 * @param n 表示要查找第 n 次出现。n 从 1 开始。
 *
 * @return 表示是否查找到了。
 *
 */
bool dstr_find_nth_cstr(const dstr_t *p_dstr, const char *sub_cstr, size_t *p_index, size_t n);

/**
 * 获取一个「动态字符串」中，另一个「动态字符串」第 n 次出现的位置索引。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param p_sub_dstr 要查找的子「动态字符串」的指针。
 *
 * @param p_index 指向一个 size_t 变量，该变量用于存储查找到的位置索引。
 *  如果找到，函数返回 true，并将出现的位置索引写入该指针指向的变量；
 *  如果没找到，函数返回 false，并不会修改该指针指向的值。
 *
 * @param n 表示要查找第 n 次出现。n 从 1 开始。
 *
 * @return 表示是否查找到了。
 *
 */
bool dstr_find_nth(const dstr_t *p_dstr, const dstr_t *p_sub_dstr, size_t *p_index, size_t n);

#endif // DYNAMIC_STRING_H
