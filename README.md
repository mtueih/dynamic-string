# Dynamic String Library

## 函数说明

- `dstr_create()`: 创建一个新的动态字符串。返回 DString 指针，失败时返回 NULL。
- `dstr_destroy(DString dstr)`: 销毁动态字符串并释放内存。
- `dstr_clear(DString dstr)`: 清空字符串内容，但保留内存。
- `dstr_cstr(const DString dstr)`: 返回字符串的 const char* 指针。
- `dstr_length(const DString dstr)`: 返回字符串长度（不含终止符）。
- `dstr_capacity(const DString dstr)`: 返回字符串容量。
- `dstr_reserve(DString dstr, size_t new_capacity)`: 设置最小容量。
- `dstr_assign_cstr(DString dstr, const char *cstr)`: 用 C 字符串赋值。
- `dstr_assign(DString dstr, const DString src_dstr)`: 用另一个动态字符串赋值。
- `dstr_append_cstr(DString dstr, const char *cstr)`: 追加 C 字符串。
- `dstr_append(DString dstr, const DString src_dstr)`: 追加另一个动态字符串。
- `dstr_insert_cstr(DString dstr, const char *cstr, size_t index)`: 在指定位置插入 C 字符串。
- `dstr_insert(DString dstr, const DString src_dstr, size_t index)`: 在指定位置插入另一个动态字符串。
- `dstr_erase(DString dstr, size_t index, size_t count)`: 从指定位置删除指定数量字符。
- `dstr_assign_format(DString dstr, const char *format, ...)`: 用格式化字符串赋值。
- `dstr_append_format(DString dstr, const char *format, ...)`: 用格式化字符串追加。
- `dstr_insert_format(DString dstr, size_t index, const char *format, ...)`: 在指定位置用格式化字符串插入。

## 错误码说明

- `DSTRING_SUCCESS`: 操作成功。
- `DSTRING_NULL_POINTER`: 传入空指针。
- `DSTRING_ALLOCATION_FAILURE`: 内存分配失败。
- `DSTRING_INVALID_ARGUMENT`: 参数无效。