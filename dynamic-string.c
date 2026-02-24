#include "dynamic-string.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// 动态字符串结构体定义
struct DynamicString {
    char *data;
    size_t length;
    size_t capacity;
    // 用于存储用户手动设置的容量值，
    // 在动态调整容量时维持容量不低于这个值。
    size_t min_capacity;
};

// 内部函数声明
// 计算合适的容量值
static size_t calculate_new_capacity(const DString dstring, size_t needed_capacity);
// 调整容量
static int resize_capacity(DString dstr, size_t new_capacity);
// 数据流插入
static inline void memory_insert(
    void *dest, size_t dest_len, 
    const void *src, size_t src_len, 
    size_t index
);
// 计算格式化字符串所需长度
static size_t calculate_format_string_length(const char *format, va_list args);


// API 实现
// 创建一个动态字符串。
[[nodiscard("Ignoring this return value will result in a resource leak.")]]
DString dstr_create(void) {
    DString dstr;
    
    // 分配内存
    dstr = (DString)malloc(sizeof(struct DynamicString));
    if (!dstr) return NULL;
    
    // 初始化属性
    dstr->data = NULL;
    dstr->length = 0;
    dstr->capacity = 0;
    dstr->min_capacity = 0;

    return dstr;
}

// 销毁一个动态字符串，释放其占用的资源。
void dstr_destroy(DString dstr) {
    if (!dstr) return;

    if (dstr->data) free(dstr->data);

    free(dstr);
}

// 清空一个动态字符串的内容，不会释放内存，只是让其变为一个空字符串。
void dstr_clear(DString dstr) {
    if (!dstr) return;

    // 两种情况，第一种是 data 为空，表示还没有分配过内存
    // 第二种是 data 不为空，但是length 为 0，说明已经是空字符串了。
    if (!dstr->data || !dstr->length) return;

    dstr->data[0] = '\0';
    dstr->length = 0;
}

// 获取一个动态字符串的内部 C 形式的字符串（const char 指针）。
const char *dstr_cstr(const DString dstr) {
    if (!dstr) return NULL;

    return dstr->data;
}

// 获取一个动态字符串的当前长度（不包括终止符）。
size_t dstr_length(const DString dstr) {
    if (!dstr) return 0;
    
    return dstr->length;
}

// 获取一个动态字符串的当前容量。
size_t dstr_capacity(const DString dstr) {
    if (!dstr) return 0;
    
    return dstr->capacity;
}

// 手动设置一个动态字符串的容量，设置过后，容量会维持在所设置的容量之上。
int dstr_reserve(DString dstr, size_t new_capacity) {
    int result;
    
    // 指针参数检查，防止空指针访问
    if (!dstr) return DSTRING_NULL_POINTER;
    
    result = DSTRING_SUCCESS;

    // 如果新容量与当前容量相同，则不需要调整容量，但仍需要更新 min_capacity
    if (new_capacity == dstr->capacity) goto end;

    // 调整容量，如果失败则不更新 min_capacity
    if ((result = resize_capacity(dstr, new_capacity))
        != DSTRING_SUCCESS
    ) return result;

end:
    // 更新 min_capacity，维持容量不低于用户设置的值
    dstr->min_capacity = new_capacity;
    return result;
}

// 将一个 C 字符串赋值给一个动态字符串。
int dstr_assign_cstr(DString dstr, const char *cstr) {
    size_t cstr_length;
    int result;

    // 指针参数检查，防止空指针访问
    if (!dstr || !cstr) return DSTRING_NULL_POINTER;

    // 调整容量以适应新的内容
    cstr_length = strlen(cstr);
    if ((result = resize_capacity(
        dstr, calculate_new_capacity(dstr, cstr_length + 1)
    )) != DSTRING_SUCCESS) return result;

    // 插入数据流
    memory_insert(
        dstr->data, 0,
        cstr, cstr_length, 
        0
    );

    // 属性更新
    dstr->length = cstr_length;
    dstr->data[dstr->length] = '\0';
    return DSTRING_SUCCESS;
}

// 将另一个动态字符串赋值给一个动态字符串。
int dstr_assign(DString dstr, const DString source_dstr) {
    int result;

    // 指针参数检查，防止空指针访问
    if (!dstr || !source_dstr || !source_dstr->data) return DSTRING_NULL_POINTER;

    // 调整容量以适应新的内容
    if ((result = resize_capacity(
        dstr, calculate_new_capacity(dstr, source_dstr->length + 1)
    )) != DSTRING_SUCCESS) return result;
    
    // 插入数据流
    memory_insert(
        dstr->data, 0,
        source_dstr->data, source_dstr->length, 
        0
    );

    // 属性更新
    dstr->length = source_dstr->length;
    dstr->data[dstr->length] = '\0';
    return DSTRING_SUCCESS;
}

// 在一个动态字符串末尾追加一个 C 字符串。
int dstr_append_cstr(DString dstr, const char *cstr) {
    size_t cstr_length;
    int result;

    // 指针参数检查，防止空指针访问
    if (!dstr || !cstr) return DSTRING_NULL_POINTER;

    // 调整容量以适应新的内容
    cstr_length = strlen(cstr);
    if ((result = resize_capacity(
        dstr, calculate_new_capacity(dstr, dstr->length + cstr_length + 1)
    )) != DSTRING_SUCCESS) return result;
    
    // 插入数据流
    memory_insert(
        dstr->data, dstr->length,
        cstr, cstr_length, 
        dstr->length
    );

    // 属性更新
    dstr->length += cstr_length;
    dstr->data[dstr->length] = '\0';
    return DSTRING_SUCCESS;
}

// 在一个动态字符串末尾追加另一个动态字符串。
int dstr_append(DString dstr, const DString source_dstr) {
    int result;

    // 指针参数检查，防止空指针访问
    if (!dstr || !source_dstr || !source_dstr->data) return DSTRING_NULL_POINTER;

    // 调整容量以适应新的内容
    if ((result = resize_capacity(
        dstr, calculate_new_capacity(
        dstr, dstr->length + source_dstr->length + 1
    ))) != DSTRING_SUCCESS) return result;

    // 插入数据流
    memory_insert(
        dstr->data, dstr->length,
        source_dstr->data, source_dstr->length, 
        dstr->length
    );

    // 属性更新
    dstr->length += source_dstr->length;
    dstr->data[dstr->length] = '\0';
    return DSTRING_SUCCESS;
}

// 在动态字符串的指定位置插入一个 C 字符串。
int dstr_insert_cstr(DString dstr, const char *cstr, size_t index) {
    size_t cstr_length;
    int result;

    // 指针参数检查，防止空指针访问
    if (!dstr || !cstr) return DSTRING_NULL_POINTER;
    // 索引参数检查，防止越界
    if (index > dstr->length) return DSTRING_INVALID_ARGUMENT;

    // 调整容量以适应新的内容
    cstr_length = strlen(cstr);
    if ((result = resize_capacity(
        dstr, calculate_new_capacity(dstr, dstr->length + cstr_length + 1)
    )) != DSTRING_SUCCESS) return result;

    // 插入数据流
    memory_insert(
        dstr->data, dstr->length,
        cstr, cstr_length, 
        index
    );

    // 属性更新
    dstr->length += cstr_length;
    dstr->data[dstr->length] = '\0';
    return DSTRING_SUCCESS;
}

// 在动态字符串的指定位置插入另一个动态字符串。
int dstr_insert(DString dstr, const DString source_dstr, size_t index) {
    int result;

    // 指针参数检查，防止空指针访问
    if (!dstr || !source_dstr || !source_dstr->data) return DSTRING_NULL_POINTER;
    // 索引参数检查，防止越界
    if (index > dstr->length) return DSTRING_INVALID_ARGUMENT;

    // 调整容量以适应新的内容
    if ((result = resize_capacity(
        dstr, calculate_new_capacity(dstr, dstr->length + source_dstr->length + 1)
    )) != DSTRING_SUCCESS) return result;

    // 插入数据流
    memory_insert(
        dstr->data, dstr->length,
        source_dstr->data, source_dstr->length, 
        index
    );

    // 属性更新
    dstr->length += source_dstr->length;
    dstr->data[dstr->length] = '\0';
    return DSTRING_SUCCESS;
}

// 从一个动态字符串的指定位置删除指定数量的字符。
int dstr_erase(DString dstr, size_t index, size_t count) {
    // 指针参数检查，防止空指针访问
    if (!dstr || !dstr->data) return DSTRING_NULL_POINTER;
    // 索引参数检查，防止越界
    if (index >= dstr->length || index + count >= dstr->length) 
        return DSTRING_INVALID_ARGUMENT;

    // 如果删除到末尾，那么久不需要进行移动操作
    if (index + count < dstr->length - 1) {
        memmove(dstr->data + index, 
            dstr->data + index + count, dstr->length - index - count
        );
    }

    // 属性更新
    dstr->length -= count;
    dstr->data[dstr->length] = '\0';

    // 尝试缩小容量，但此处没有调整成功也无上大雅
    resize_capacity(dstr, calculate_new_capacity(dstr, dstr->length + 1));

    return DSTRING_SUCCESS;
}

// 使用格式化字符串将内容赋值给一个动态字符串。
int dstr_assign_format(DString dstr, const char *format, ...) {
    va_list args;
    size_t needed_length;
    int result;
    int written_length;
    
    // 指针参数检查，防止空指针访问
    if (!dstr || !format) return DSTRING_NULL_POINTER;
    
    va_start(args, format);
    // 计算格式化字符串所需长度
    needed_length = calculate_format_string_length(format, args);

    // 如果计算长度失败，或者需要的长度为 0，则认为是无效参数
    if (needed_length == 0) { 
        va_end(args);
        return DSTRING_INVALID_ARGUMENT;
    }

    // 调整容量以适应新的内容
    if ((result = resize_capacity(
        dstr, calculate_new_capacity(dstr, needed_length + 1)
    )) != DSTRING_SUCCESS) {
        va_end(args);
        return result;
    }

    // 写入格式化字符串内容
    written_length = vsnprintf(dstr->data, dstr->capacity, format, args);
    va_end(args);

    // 如果写入失败，则需要恢复容量
    if (written_length < 0) {
        resize_capacity(dstr, calculate_new_capacity(dstr, dstr->length + 1));
        return DSTRING_ALLOCATION_FAILURE;
    }

    // 属性更新
    dstr->length = written_length;
    dstr->data[dstr->length] = '\0';
    return DSTRING_SUCCESS;
}

// 使用格式化字符串在一个动态字符串末尾追加内容。
int dstr_append_format(DString dstr, const char *format, ...) {
    va_list args;
    size_t needed_length;
    int result;
    int written_length;

    // 指针参数检查，防止空指针访问
    if (!dstr || !format) return DSTRING_NULL_POINTER;

    va_start(args, format);
    // 计算格式化字符串所需长度
    needed_length = vsnprintf(NULL, 0, format, args);

    // 如果计算长度失败，或者需要的长度为 0，则认为是无效参数
    if (needed_length == 0) {
        va_end(args);
        return DSTRING_INVALID_ARGUMENT;
    }

    // 调整容量以适应新的内容
    if ((result = resize_capacity(
        dstr, calculate_new_capacity(dstr, dstr->length + needed_length + 1)
    )) != DSTRING_SUCCESS) {
        va_end(args);
        return result;
    }

    // 写入格式化字符串内容
    written_length = vsnprintf(dstr->data + dstr->length, 
        dstr->capacity - dstr->length, format, args
    );
    va_end(args);

    // 如果写入失败，则需要恢复容量
    if (written_length < 0) {
        resize_capacity(dstr, calculate_new_capacity(dstr, dstr->length + 1));
        return DSTRING_ALLOCATION_FAILURE;
    }

    // 属性更新
    dstr->length += written_length;
    dstr->data[dstr->length] = '\0';
    return DSTRING_SUCCESS;
}

// 使用格式化字符串在一个动态字符串的指定位置插入内容。
int dstr_insert_format(DString dstr, size_t index, const char *format, ...) {
    va_list args;
    size_t needed_length;
    int result;
    int written_length;

    // 指针参数检查，防止空指针访问
    if (!dstr || !format) return DSTRING_NULL_POINTER;
    // 索引参数检查，防止越界
    if (index > dstr->length) return DSTRING_INVALID_ARGUMENT;

    va_start(args, format);
    // 计算格式化字符串所需长度
    needed_length = vsnprintf(NULL, 0, format, args);

    // 如果计算长度失败，或者需要的长度为 0，则认为是无效参数
    if (needed_length == 0) {
        va_end(args);
        return DSTRING_INVALID_ARGUMENT;
    }

    // 调整容量以适应新的内容
    if ((result = resize_capacity(
        dstr, calculate_new_capacity(dstr, dstr->length + needed_length + 1)
    )) != DSTRING_SUCCESS) {
        va_end(args);
        return result;
    }

    // 如果插入位置不是字符串末尾及最后一个字符，则需要先移动原有内容，为新内容腾出空间
    if (index < dstr->length - 1) {
        memmove(dstr->data + index + needed_length, 
            dstr->data + index, dstr->length - index
        );
    }

    // 写入格式化字符串内容
    written_length = vsnprintf(dstr->data + index, needed_length + 1, format, args);
    va_end(args);

    // 如果写入失败，则需要将之前移动的内容恢复到原位，并恢复容量
    if (written_length < 0) {
        memmove(dstr->data + index, 
            dstr->data + index + needed_length, dstr->length - index
        );
        dstr->data[dstr->length] = '\0';
        resize_capacity(dstr, calculate_new_capacity(dstr, dstr->length + 1));
        return DSTRING_ALLOCATION_FAILURE;
    }

    // 属性更新
    dstr->length += written_length;
    dstr->data[dstr->length] = '\0';
    return DSTRING_SUCCESS;
}


/* 内部函数定义 */
// 计算合适的容量值
static size_t calculate_new_capacity(const DString dstring, size_t needed_capacity) {
    size_t capacity;

    if (!needed_capacity) return 0;
    // 维持容量大于用户手动设置的容量
    capacity = needed_capacity > dstring->min_capacity 
                ? needed_capacity : dstring->min_capacity;

    // 于计算机字长的倍数向上吸附
    if (!(capacity % sizeof(void *))) return capacity;
    return (capacity / sizeof(void *) + 1) * sizeof(void *);
}

// 调整容量
static int resize_capacity(DString dstr, size_t new_capacity) {
    char *new_data;

    // 如果容量为 0，则单独执行释放操作，避免 realloc 在某些平台上表现不一致
    if (!new_capacity && dstr->data) {
        free(dstr->data);
        dstr->data = NULL;
        dstr->length = 0;
        dstr->capacity = 0;
        return DSTRING_SUCCESS;
    }

    // 重新分配内存
    new_data = (char *)realloc(dstr->data, new_capacity);
    if (!new_data) return DSTRING_ALLOCATION_FAILURE;

    // 如果新容量小于当前长度，则需要调整长度并添加终止符
    if (new_capacity < dstr->length + 1) {
        dstr->length = new_capacity - 1;
        dstr->data[dstr->length] = '\0';
    }

    // 更新属性
    dstr->data = new_data;
    dstr->capacity = new_capacity;
    return DSTRING_SUCCESS;
}

// 数据流插入
static inline void memory_insert(
    void *dest, size_t dest_len, 
    const void *src, size_t src_len, 
    size_t index
) {
    // 当插入位置刚好出于目标流末尾时，则不需要进行内存移动
    if (dest_len - index) {
        memmove(dest + index + src_len, dest + index, dest_len - index);
    }
    // 内存拷贝
    memcpy(dest + index, src, src_len);
}

// 计算格式化字符串所需长度
static size_t calculate_format_string_length(const char *format, va_list args) {
    va_list temp_args;
    int needed_length;

    va_copy(temp_args, args);
    needed_length = vsnprintf(NULL, (size_t)0, format, temp_args);
    va_end(temp_args);

    return needed_length < 0 ? 0 : (size_t)needed_length;
}
