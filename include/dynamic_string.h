//
// Created by mtueih on 2026/2/25.
//

#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#include <stdbool.h>
#include <stddef.h>
#include "portable_attributes.h"

// ADT 类型别名声明
typedef struct DynamicString DString;

// API 函数原型（声明）
// 创建、销毁、清空
DString *dstr_create(
    const char *cstr
) NODISCARD;

void dstr_destroy(
    DString *dstr
) NONNULL(1);

void dstr_clear(
    DString *dstr
) NONNULL(1);

// 属性获取与设置
const char *dstr_cstr(
    const DString *dstr
) PURE NONNULL(1);

size_t dstr_length(
    const DString *dstr
) PURE NONNULL(1);

size_t dstr_capacity(
    const DString *dstr
) PURE NONNULL(1);

// 设置容量
bool dstr_resize_capacity(
    DString *dstr,
    size_t new_capacity
) NONNULL(1);

// 复制、追加、插入、删除
// 复制、追加、插入完整现有字符串到目标字符串
bool dstr_cpy_cstr(
    DString *dest,
    const char *src
) NONNULL(1, 2);

bool dstr_cpy(
    DString *dest,
    const DString *src
) NONNULL(1, 2);

bool dstr_cat_cstr(
    DString *dest,
    const char *src
) NONNULL(1, 2);

bool dstr_cat(
    DString *dest,
    const DString *src
) NONNULL(1, 2);

bool dstr_insert_cstr(
    DString *dest,
    const char *src,
    size_t index
) NONNULL(1, 2);

bool dstr_insert(
    DString *dest,
    const DString *src,
    size_t index
) NONNULL(1, 2);

// 复制、追加、插入现有字符串的子串到目标字符串
bool dstr_cpy_sub_cstr(
    DString *dest,
    const char *src,
    size_t sub_index,
    size_t sub_count
) NONNULL(1, 2);

bool dstr_cpy_sub(
    DString *dest,
    const DString *src,
    size_t sub_index,
    size_t sub_count
) NONNULL(1, 2);

bool dstr_cat_sub_cstr(
    DString *dest,
    const char *src,
    size_t sub_index,
    size_t sub_count
) NONNULL(1, 2);

bool dstr_cat_sub(
    DString *dest,
    const DString *src,
    size_t sub_index,
    size_t sub_count
) NONNULL(1, 2);

bool dstr_insert_sub_cstr(
    DString *dest,
    const char *src,
    size_t index,
    size_t sub_index,
    size_t sub_count
) NONNULL(1, 2);

bool dstr_insert_sub(
    DString *dest,
    const DString *src,
    size_t index,
    size_t sub_index,
    size_t sub_count
) NONNULL(1, 2);

// 删除子串
void dstr_remove(
    DString *dstr,
    size_t sub_index,
    size_t sub_count
) NONNULL(1);

// 删除特定内容
/**
 * 去除收尾空白字符。
 */
void dstr_trim(
    DString *dstr
) NONNULL(1);

// 格式化写入
bool dstr_printf(
    DString *dstr,
    const char *format,
    ...
) FORMAT(2, 3) NONNULL(1, 2);

// 从现有字符串生成新字符串
// 提取子串
DString *dstr_sub_cstr(
    const char *cstr,
    size_t sub_index,
    size_t sub_count
) NODISCARD NONNULL(1);

DString *dstr_sub(
    const DString *dstr,
    size_t sub_index,
    size_t sub_count
) NODISCARD NONNULL(1);

// 克隆
DString *dstr_clone(
    const DString *dstr
) NODISCARD NONNULL(1);

// 查找、统计与替换
bool dstr_find_cstr(
    const DString *dstr,
    const char *sub,
    size_t *out_index,
    bool backward
) NONNULL(1, 2, 3) PURE;

bool dstr_find(
    const DString *dstr,
    const DString *sub,
    size_t *out_index,
    bool backward
) NONNULL(1, 2, 3) PURE;

size_t dstr_count_cstr(
    const DString *dstr,
    const char *sub
) NONNULL(1, 2) PURE;

size_t dstr_count(
    const DString *dstr,
    const DString *sub
) NONNULL(1, 2) PURE;

bool dstr_find_nth_cstr(
    const DString *dstr,
    const char *sub,
    size_t *out_index,
    size_t n,
    bool backward
) NONNULL(1, 2, 3) PURE;

bool dstr_find_nth(
    const DString *dstr,
    const DString *sub,
    size_t *out_index,
    size_t n,
    bool backward
) NONNULL(1, 2, 3) PURE;

size_t dstr_replace_cstr(
    DString *dstr,
    const char *old,
    const char *new,
    size_t n,
    bool backward
) NONNULL(1, 2, 3);

size_t dstr_replace(
    DString *dstr,
    const DString *old,
    const DString *new,
    size_t n,
    bool backward
) NONNULL(1, 2, 3);

// 判断与比较
bool dstr_starts_with_cstr(
    const DString *dstr,
    const char *prefix
) NONNULL(1, 2) PURE;

bool dstr_starts_with(
    const DString *dstr,
    const DString *prefix
) NONNULL(1, 2) PURE;

bool dstr_ends_with_cstr(
    const DString *dstr,
    const char *suffix
) NONNULL(1, 2) PURE;

bool dstr_ends_with(
    const DString *dstr,
    const DString *suffix
) NONNULL(1, 2) PURE;

bool dstr_contains_cstr(
    const DString *dstr,
    const char *sub
) NONNULL(1, 2) PURE;

bool dstr_contains(
    const DString *dstr,
    const DString *sub
) NONNULL(1, 2) PURE;

bool dstr_equals_cstr(
    const DString *dstr,
    const char *cstr
) NONNULL(1, 2) PURE;

bool dstr_equals(
    const DString *dstr_1,
    const DString *dstr_2
) NONNULL(1, 2) PURE;

int dstr_compare_cstr(
    const DString *dstr,
    const char *cstr
) NONNULL(1, 2) PURE;

int dstr_compare(
    const DString *dstr_1,
    const DString *dstr_2
) NONNULL(1, 2) PURE;


#endif // DYNAMIC_STRING_H
