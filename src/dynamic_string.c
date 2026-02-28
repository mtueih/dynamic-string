//
// Created by mtueih on 2026/2/25.
//

#include "dynamic_string.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct dynamic_string {
    char *m_data;
    size_t m_length;
    size_t m_capacity;
    size_t m_min_capacity;
};

// 静态函数声明
// 调整一个「动态字符串」的容量
static int capacity_resize(dstr_t *p_dstr, size_t new_length);
// 计算一个格式化字符串需要的长度
static size_t length_of_dstr_printf(const char *format, va_list args);


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
dstr_t *dstr_create(const char *cstr) {
    dstr_t *p_new_dstr;
    size_t cstr_length;

    p_new_dstr = (dstr_t *) malloc(sizeof(dstr_t));
    if (p_new_dstr == NULL) return NULL;

    *p_new_dstr = (dstr_t){0};

    if (cstr != NULL && (cstr_length = strlen(cstr)) != 0) {
        if (capacity_resize(p_new_dstr, cstr_length) != 0) {
            free(p_new_dstr);
            return NULL;
        }

        memcpy(p_new_dstr->m_data, cstr, cstr_length);
        p_new_dstr->m_length = cstr_length;
        p_new_dstr->m_data[cstr_length] = '\0';
    }

    return p_new_dstr;
}

/**
 * 销毁一个「动态字符串」。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 */
void dstr_destroy(dstr_t *p_dstr) {
    assert(p_dstr != NULL);

    if (p_dstr->m_data != NULL) free(p_dstr->m_data);

    free(p_dstr);
}

/**
 * 清空一个「动态字符串」的内容。
 *  不会修改容量，只是让其变为一个空字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 */
void dstr_clear(dstr_t *p_dstr) {
    assert(p_dstr != NULL);

    if (p_dstr->m_data == NULL || p_dstr->m_length == 0) return;

    p_dstr->m_data[0] = '\0';
    p_dstr->m_length = 0;
}

/**
 * 获取一个「动态字符串」的 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @return C 字符串。
 *
 */
const char *dstr_cstr(const dstr_t *p_dstr) {
    assert(p_dstr != NULL);

    return p_dstr->m_data;
}

/**
 * 获取一个「动态字符串」的长度。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @return 长度。
 *
 */
size_t dstr_length(const dstr_t *p_dstr) {
    assert(p_dstr != NULL);

    return p_dstr->m_length;
}

/**
 * 获取一个「动态字符串」的容量。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @return 容量。
 *
 */
size_t dstr_capacity(const dstr_t *p_dstr) {
    assert(p_dstr != NULL);
    return p_dstr->m_capacity;
}

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
int dstr_resize(dstr_t *p_dstr, size_t new_length) {
    int result;
    size_t original_min_capacity;

    assert(p_dstr != NULL);

    original_min_capacity = p_dstr->m_min_capacity;
    p_dstr->m_min_capacity = 0;
    result = capacity_resize(p_dstr, new_length);

    if (result == 0) {
        p_dstr->m_min_capacity = new_length;
    } else {
        p_dstr->m_min_capacity = original_min_capacity;
    }

    return result;
}

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
int dstr_assign_cstr(dstr_t *p_dstr, const char *cstr) {
    int result;
    size_t cstr_length;

    assert(p_dstr != NULL && cstr != NULL);


    cstr_length  = strlen(cstr);

    result = capacity_resize(p_dstr, cstr_length + 1);
    if (result == 0) {
        memcpy(p_dstr->m_data, cstr, cstr_length);
        p_dstr->m_length = cstr_length;
        p_dstr->m_data[cstr_length] = '\0';
    }
    return result;
}

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
int dstr_assign(dstr_t *p_dst_dstr, const dstr_t *p_src_dstr) {
    int result;

    assert(p_dst_dstr != NULL && p_src_dstr != NULL);

    result = capacity_resize(p_dst_dstr, p_src_dstr->m_length + 1);
    if (result == 0) {
        memcpy(p_dst_dstr->m_data, p_src_dstr->m_data, p_src_dstr->m_length);
        p_dst_dstr->m_length = p_src_dstr->m_length;
        p_dst_dstr->m_data[p_dst_dstr->m_length] = '\0';
    }
    return result;
}

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
int dstr_append_cstr(dstr_t *p_dstr, const char *cstr) {
    int result;
    size_t cstr_length;

    assert(p_dstr != NULL && cstr != NULL);

    cstr_length = strlen(cstr);
    if (cstr_length == 0) return 0;
    result = capacity_resize(p_dstr, p_dstr->m_length + cstr_length + 1);
    if (result == 0) {
        memcpy(p_dstr->m_data + p_dstr->m_length, cstr, cstr_length);
        p_dstr->m_length += cstr_length;
        p_dstr->m_data[p_dstr->m_length] = '\0';
    }
    return result;
}

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
int dstr_append(dstr_t *p_dst_dstr, const dstr_t *p_src_dstr) {
    int result;

    assert(p_dst_dstr != NULL && p_src_dstr != NULL);

    if (p_src_dstr->m_length == 0) return 0;

    result = capacity_resize(p_dst_dstr, p_dst_dstr->m_length + p_src_dstr->m_length + 1);
    if (result == 0) {
        memcpy(p_dst_dstr->m_data + p_dst_dstr->m_length, p_src_dstr->m_data, p_src_dstr->m_length);
        p_dst_dstr->m_length += p_src_dstr->m_length;
        p_dst_dstr->m_data[p_dst_dstr->m_length] = '\0';
    }
    return result;
}

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
int dstr_insert_cstr(dstr_t *p_dstr, const char *cstr, size_t index) {
    int result;
    size_t cstr_length;

    assert(p_dstr != NULL && cstr != NULL && index <= p_dstr->m_length);

    cstr_length = strlen(cstr);
    if (cstr_length == 0) return 0;

    result = capacity_resize(p_dstr, p_dstr->m_length + cstr_length + 1);
    if (result == 0) {
        if (index < p_dstr->m_length) {
            memmove(p_dstr->m_data + index + cstr_length,
                    p_dstr->m_data + index,
                    p_dstr->m_length - index
            );
        }
        memcpy(p_dstr->m_data + index, cstr, cstr_length);
        p_dstr->m_length += cstr_length;
        p_dstr->m_data[p_dstr->m_length] = '\0';
    }
    return result;
}

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
int dstr_insert(dstr_t *p_dst_dstr, const dstr_t *p_src_dstr, size_t index) {
    int result;

    assert(p_dst_dstr != NULL && p_src_dstr != NULL && index <= p_dst_dstr->m_length);

    if (p_src_dstr->m_length == 0) return 0;

    result = capacity_resize(p_dst_dstr, p_dst_dstr->m_length + p_src_dstr->m_length + 1);
    if (result == 0) {
        if (index < p_dst_dstr->m_length) {
            memmove(p_dst_dstr->m_data + index + p_src_dstr->m_length,
                    p_dst_dstr->m_data + index,
                    p_dst_dstr->m_length - index
            );
        }
        memcpy(p_dst_dstr->m_data + index, p_src_dstr->m_data, p_src_dstr->m_length);
        p_dst_dstr->m_length += p_src_dstr->m_length;
        p_dst_dstr->m_data[p_dst_dstr->m_length] = '\0';
    }
    return result;
}

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
int dstr_remove(dstr_t *p_dstr, size_t index, size_t quantity) {
    assert(p_dstr != NULL && index < p_dstr->m_length && index + quantity <= p_dstr->m_length);

    if (index + quantity < p_dstr->m_length) {
        memmove(p_dstr->m_data + index,
                p_dstr->m_data + index + quantity,
                p_dstr->m_length - index - quantity
        );
    }

    p_dstr->m_length -= (quantity == 0) ? p_dstr->m_length - index : quantity;
    p_dstr->m_data[p_dstr->m_length] = '\0';

    return capacity_resize(p_dstr, p_dstr->m_length + 1);
}

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
int dstr_printf(dstr_t *p_dstr, const char *format, ...) {
    va_list args;
    size_t original_capcity;
    size_t needed_length;
    int result;
    int written_length;

    assert(p_dstr != NULL && format != NULL);

    va_start(args, format);
    needed_length = length_of_dstr_printf(format, args);
    if (needed_length == 0) {
        va_end(args);
        return EINVAL;
    }

    original_capcity = p_dstr->m_capacity;
    if (needed_length + 1 > p_dstr->m_capacity) {
        result = capacity_resize(p_dstr, needed_length + 1);
        if (result != 0) {
            va_end(args);
            return result;
        }
    }

    written_length = vsnprintf(p_dstr->m_data, needed_length + 1, format, args);
    va_end(args);

    if (written_length < 0) {
        if (needed_length + 1 > original_capcity) {
            capacity_resize(p_dstr, original_capcity);
        }
        return EINVAL;
    }

    if (written_length + 1 < p_dstr->m_capacity) {
        capacity_resize(p_dstr, written_length + 1);
    }

    p_dstr->m_length = written_length;
    p_dstr->m_data[p_dstr->m_length] = '\0';
    return 0;
}

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
int dstr_sub_cstr(dstr_t *p_dstr, const char *cstr, size_t index, size_t quantity) {
    size_t cstr_length;
    size_t sub_str_length;
    int result;

    assert(p_dstr != NULL && cstr != NULL);

    cstr_length = strlen(cstr);
    if (cstr_length == 0) return 0;

    sub_str_length = (quantity == 0) ? cstr_length - index : quantity;

    result = capacity_resize(p_dstr, sub_str_length + 1);
    if (result == 0) {
        memcpy(p_dstr->m_data, cstr + index, sub_str_length);
        p_dstr->m_length = sub_str_length;
        p_dstr->m_data[sub_str_length] = '\0';
    }
    return result;
}

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
int dstr_sub(dstr_t *p_dst_dstr, const dstr_t *p_src_dstr, size_t index, size_t quantity) {
    size_t sub_str_length;
    int result;

    if (p_dst_dstr == NULL || p_src_dstr == NULL
        || p_src_dstr->m_length == 0
        || index >= p_src_dstr->m_length || index + quantity > p_src_dstr->m_length
    )
        return EINVAL;

    sub_str_length = (quantity == 0) ? p_src_dstr->m_length - index : quantity;

    result = capacity_resize(p_dst_dstr, sub_str_length + 1);
    if (result == 0) {
        memcpy(p_dst_dstr->m_data, p_src_dstr->m_data + index, sub_str_length);
        p_dst_dstr->m_length = sub_str_length;
        p_dst_dstr->m_data[sub_str_length] = '\0';
    }
    return result;
}

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
int dstr_replace_cstr(dstr_t *p_dstr, const char *old_cstr, const char *new_cstr, size_t n, bool backward) {
    size_t old_cstr_length;
    size_t new_cstr_length;
    size_t i;
    int result;
    size_t find_count;
    char *find; // 指向当前比较的字符串
    size_t old_cstr_count;

    if (p_dstr == NULL || old_cstr == NULL || new_cstr == NULL
        || (old_cstr_length = strlen(old_cstr)) == 0
        || old_cstr_length > p_dstr->m_length
    )
        return EINVAL;

    old_cstr_count = dstr_count_cstr(p_dstr, old_cstr);
    if (old_cstr_count == 0)

    find_count = 0;
    if (backward == false) {
        find = p_dstr->m_data;
        while (find <= p_dstr->m_data + p_dstr->m_length - old_cstr_length
               && (n == 0 ? true : find_count < n)
        ) {
            if (strncmp(find, old_cstr, old_cstr_length) == 0) {
                ++find_count;
                find += old_cstr_length;
            } else {
                ++find;
            }
        }
    } else {
        find = p_dstr->m_data + p_dstr->m_length - old_cstr_length;
        while (find >= p_dstr->m_data
               && (n == 0 ? true : find_count < n)
        ) {
            if (strncmp(find, old_cstr, old_cstr_length) == 0) {
                ++find_count;
                find -= old_cstr_length;
            } else {
                --find;
            }
        }
    }

    if (n == 0 ? find_count == 0 : find_count < n) return EINVAL;

    new_cstr_length = strlen(new_cstr);
    result = capacity_resize(p_dstr,
                             p_dstr->m_length - old_cstr_length * find_count
                             + new_cstr_length * find_count
    );
    if (result != 0) return result;

    i = 0;
    if (backward == false) {
        for (find = p_dstr->m_data; i < find_count; ++i);
    } else {
        find = p_dstr->m_data + p_dstr->m_length - old_cstr_length;
        while (find >= p_dstr->m_data
               && (n == 0 ? true : find_count < n)
        ) {
            if (strncmp(find, old_cstr, old_cstr_length) == 0) {
                ++find_count;
                find -= old_cstr_length;
            } else {
                --find;
            }
        }
    }
}

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
int dstr_replace(dstr_t *p_dstr, const dstr_t *p_old_dstr, const dstr_t *p_new_dstr, size_t n, bool backward) {
}

/**
 * 判断一个「动态字符串」是否开始于一个 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param prefix 前缀 C 字符串。
 *
 */
bool dstr_starts_with_cstr(const dstr_t *p_dstr, const char *prefix) {
    size_t prefix_length;

    if (p_dstr == NULL || prefix == NULL ||
        (prefix_length = strlen(prefix)) == 0 ||
        prefix_length > p_dstr->m_length
    )
        return false;

    return strncmp(p_dstr->m_data, prefix, prefix_length) == 0;
}

/**
 * 判断一个「动态字符串」是否开始于另一个「动态字符串」。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param prefix 前缀「动态字符串」。
 *
 */
bool dstr_starts_with(const dstr_t *p_dstr, const dstr_t *prefix) {
    if (p_dstr == NULL || prefix == NULL ||
        prefix->m_length == 0 ||
        prefix->m_length > p_dstr->m_length
    )
        return false;

    return strncmp(p_dstr->m_data, prefix->m_data, prefix->m_length) == 0;
}

/**
 * 判断一个「动态字符串」是否结束于一个 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param suffix 后缀 C 字符串。
 *
 */
bool dstr_ends_with_cstr(const dstr_t *p_dstr, const char *suffix) {
    size_t suffix_length;

    if (p_dstr == NULL || suffix == NULL ||
        (suffix_length = strlen(suffix)) == 0 ||
        suffix_length > p_dstr->m_length
    )
        return false;

    return strncmp(p_dstr->m_data + p_dstr->m_length - suffix_length,
                   suffix, suffix_length) == 0;
}

/**
 * 判断一个「动态字符串」是否结束于另一个「动态字符串」。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param suffix 后缀「动态字符串」。
 *
 */
bool dstr_ends_with(const dstr_t *p_dstr, const dstr_t *suffix) {
    if (p_dstr == NULL || suffix == NULL ||
        suffix->m_length == 0 ||
        suffix->m_length > p_dstr->m_length
    )
        return false;

    return strncmp(p_dstr->m_data + p_dstr->m_length - suffix->m_length,
                   suffix->m_data, suffix->m_length) == 0;
}

/**
 * 判断一个「动态字符串」是否包含一个 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param sub_cstr 包含的 C 字符串。
 *
 */
bool dstr_contains_cstr(const dstr_t *p_dstr, const char *sub_cstr) {
    size_t sub_cstr_length;

    if (p_dstr == NULL || sub_cstr == NULL ||
        (sub_cstr_length = strlen(sub_cstr)) == 0 ||
        sub_cstr_length > p_dstr->m_length
    )
        return false;

    return strstr(p_dstr->m_data, sub_cstr) != NULL;
}

/**
 * 判断一个「动态字符串」是否包含另一个「动态字符串」。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param p_sub_dstr 包含的「动态字符串」。
 *
 */
bool dstr_contains(const dstr_t *p_dstr, const dstr_t *p_sub_dstr) {
    if (p_dstr == NULL || p_sub_dstr == NULL ||
        p_sub_dstr->m_length == 0 ||
        p_sub_dstr->m_length > p_dstr->m_length
    )
        return false;

    return strstr(p_dstr->m_data, p_sub_dstr->m_data) != NULL;
}

/**
 * 判断一个「动态字符串」是否等于一个 C 字符串。
 *
 * @param p_dstr 「动态字符串」指针。
 *
 * @param cstr C 字符串。
 *
 */
bool dstr_equals_cstr(const dstr_t *p_dstr, const char *cstr) {
    size_t cstr_length;

    if (p_dstr == NULL || cstr == NULL ||
        (cstr_length = strlen(cstr)) == 0 ||
        cstr_length != p_dstr->m_length
    )
        return false;

    return strncmp(p_dstr->m_data, cstr, cstr_length) == 0;
}

/**
 * 判断一个「动态字符串」是否等于另一个「动态字符串」。
 *
 * @param p_dstr_1 「动态字符串」1 的指针。
 *
 * @param p_dstr_2 「动态字符串」2 的指针。
 *
 */
bool dstr_equals(const dstr_t *p_dstr_1, const dstr_t *p_dstr_2) {
    if (p_dstr_1 == NULL || p_dstr_2 == NULL ||
        p_dstr_2->m_length == 0 ||
        p_dstr_2->m_length != p_dstr_1->m_length
    )
        return false;

    return strncmp(p_dstr_1->m_data, p_dstr_2->m_data, p_dstr_2->m_length) == 0;
}

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
int dstr_compare_cstr(const dstr_t *p_dstr, const char *cstr) {
    if (p_dstr == NULL || cstr == NULL || p_dstr->m_data == NULL
    )
        return EINVAL;

    return strcmp(p_dstr->m_data, cstr);
}

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
int dstr_compare(const dstr_t *p_dstr_1, const dstr_t *p_dstr_2) {
    if (p_dstr_1 == NULL || p_dstr_2 == NULL ||
        p_dstr_1->m_data == NULL || p_dstr_2->m_data == 0
    )
        return EINVAL;

    return strcmp(p_dstr_1->m_data, p_dstr_2->m_data);
}

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
bool dstr_find_cstr(const dstr_t *p_dstr, const char *sub_cstr, size_t *p_index) {
  size_t sub_cstr_length;
    char *find;

    if (p_dstr == NULL || sub_cstr == NULL || p_index == NULL
        || (sub_cstr_length = strlen(sub_cstr)) == 0
        || sub_cstr_length > p_dstr->m_length
        ) return false;

     find = strstr(p_dstr->m_data, sub_cstr);

    if (find == NULL) return false;

    *p_index = find - p_dstr->m_data;
    return true;
}

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
bool dstr_find(const dstr_t *p_dstr, const dstr_t *p_sub_dstr, size_t *p_index) {
    size_t sub_cstr_length;
    char *find;

    if (p_dstr == NULL || p_sub_dstr == NULL || p_index == NULL
        || p_sub_dstr->m_length == 0
        || p_sub_dstr->m_length > p_dstr->m_length
        ) return false;

    find = strstr(p_dstr->m_data, p_sub_dstr->m_data);

    if (find == NULL) return false;

    *p_index = find - p_dstr->m_data;
    return true;
}

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
bool dstr_rfind_cstr(const dstr_t *p_dstr, const char *sub_cstr, size_t *p_index) {
    size_t sub_cstr_length;
    char *find;

    if (p_dstr == NULL || sub_cstr == NULL || p_index == NULL
        || (sub_cstr_length = strlen(sub_cstr)) == 0
        || sub_cstr_length > p_dstr->m_length
        ) return false;

    for (find = p_dstr->m_data + p_dstr->m_length - sub_cstr_length;
        find >= p_dstr->m_data; --find
    ) {
        if (strncmp(find, sub_cstr, sub_cstr_length) == 0) {
            break;
        }
    }

    if (find < p_dstr->m_data) return false;

    *p_index = find - p_dstr->m_data;
    return true;
}

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
bool dstr_rfind(const dstr_t *p_dstr, const dstr_t *p_sub_dstr, size_t *p_index) {
    char *find;

    if (p_dstr == NULL || p_sub_dstr == NULL || p_index == NULL
        || p_sub_dstr->m_length == 0
        || p_sub_dstr->m_length > p_dstr->m_length
        ) return false;

    for (find = p_dstr->m_data + p_dstr->m_length - p_sub_dstr->m_length;
        find >= p_dstr->m_data; --find
    ) {
        if (strncmp(find, p_sub_dstr->m_data, p_sub_dstr->m_length) == 0) {
            break;
        }
    }

    if (find < p_dstr->m_data) return false;

    *p_index = find - p_dstr->m_data;
    return true;
}

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
size_t dstr_count_cstr(const dstr_t *p_dstr, const char *sub_cstr) {
    size_t sub_cstr_length;
    char *find;
    size_t find_count;

    if (p_dstr == NULL || sub_cstr == NULL
        || (sub_cstr_length = strlen(sub_cstr)) == 0
        || sub_cstr_length > p_dstr->m_length
        ) return 0;

    for (find_count = 0, find = p_dstr->m_data;
        find <= p_dstr->m_data + p_dstr->m_length - sub_cstr_length;
    ) {
        if (strncmp(find, sub_cstr, sub_cstr_length) == 0) {
            ++find_count;
            find += sub_cstr_length;
        } else {
            ++find;
        }
    }

    return find_count;
}

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
size_t dstr_count(const dstr_t *p_dstr, const dstr_t *p_sub_dstr) {
    char *find;
    size_t find_count;

    if (p_dstr == NULL || p_sub_dstr == NULL
        || p_sub_dstr->m_length == 0
        || p_sub_dstr->m_length > p_dstr->m_length
        ) return 0;

    for (find_count = 0, find = p_dstr->m_data;
        find <= p_dstr->m_data + p_dstr->m_length - p_sub_dstr->m_length;
    ) {
        if (strncmp(find, p_sub_dstr->m_data, p_sub_dstr->m_length) == 0) {
            ++find_count;
            find += p_sub_dstr->m_length;
        } else {
            ++find;
        }
    }

    return find_count;
}

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
bool dstr_find_nth_cstr(const dstr_t *p_dstr, const char *sub_cstr, size_t *p_index, size_t n) {
    size_t sub_cstr_length;
    char *find;
    size_t find_count;

    if (p_dstr == NULL || sub_cstr == NULL || p_index == NULL
        || n == 0 || n > p_dstr->m_length
        || (sub_cstr_length = strlen(sub_cstr)) == 0
        || sub_cstr_length > p_dstr->m_length
        || n * sub_cstr_length > p_dstr->m_length
        ) return false;

    for (find_count = 0, find = p_dstr->m_data;
        find <= p_dstr->m_data + p_dstr->m_length - sub_cstr_length;
    ) {
        if (strncmp(find, sub_cstr, sub_cstr_length) == 0) {
            ++find_count;
            if (find_count == n) {
                *p_index = find - p_dstr->m_data;
                return true;
            }
            find += sub_cstr_length;
        } else {
            ++find;
        }
    }

    return false;
}

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
bool dstr_find_nth(const dstr_t *p_dstr, const dstr_t *p_sub_dstr, size_t *p_index, size_t n) {
    size_t sub_cstr_length;
    char *find;
    size_t find_count;

    if (p_dstr == NULL || p_sub_dstr == NULL || p_index == NULL
        || n == 0 || n > p_dstr->m_length
        || p_sub_dstr->m_length == 0
        || p_sub_dstr->m_length > p_dstr->m_length
        || n * p_sub_dstr->m_length > p_dstr->m_length
        ) return false;

    for (find_count = 0, find = p_dstr->m_data;
        find <= p_dstr->m_data + p_dstr->m_length - p_sub_dstr->m_length;
    ) {
        if (strncmp(find, p_sub_dstr->m_data, p_sub_dstr->m_length) == 0) {
            ++find_count;
            if (find_count == n) {
                *p_index = find - p_dstr->m_data;
                return true;
            }
            find += p_sub_dstr->m_length;
        } else {
            ++find;
        }
    }

    return false;
}

// 静态函数实现

static int capacity_resize(dstr_t *p_dstr, size_t new_length) {
    char *new_cstr;
    size_t adjusted_capacity;

    if (new_length == 0) {
        free(p_dstr->m_data);
        *p_dstr = (dstr_t){0};
        return 0;
    }

    adjusted_capacity = (new_length > p_dstr->m_min_capacity)
                            ? new_length
                            : p_dstr->m_min_capacity;

    adjusted_capacity = (adjusted_capacity % sizeof(void *) == 0)
                            ? adjusted_capacity
                            : (adjusted_capacity / sizeof(void *) + 1) * sizeof(void *);

    new_cstr = (char *) realloc(p_dstr->m_data, adjusted_capacity);
    if (new_cstr == NULL) {
        adjusted_capacity = new_length;
        new_cstr = (char *) realloc(p_dstr->m_data, adjusted_capacity);
        if (new_cstr == NULL) {
            return ENOMEM;
        }
    }

    p_dstr->m_data = new_cstr;
    p_dstr->m_capacity = adjusted_capacity;

    if (adjusted_capacity <= p_dstr->m_length) {
        p_dstr->m_length -= p_dstr->m_length + 1 - adjusted_capacity;
        new_cstr[p_dstr->m_length] = '\0';
    }

    return 0;
}

static size_t length_of_dstr_printf(const char *format, va_list args) {
    va_list temp_args;
    int needed_length;

    va_copy(temp_args, args);
    needed_length = vsnprintf(NULL, 0, format, temp_args);
    va_end(temp_args);

    return needed_length < 0 ? 0 : (size_t) needed_length;
}
