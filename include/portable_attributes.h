//
// Created by isadids on 2026/3/3.
//

#ifndef DYNAMIC_STRING_PORTABLE_ATTRIBUTES_H
#define DYNAMIC_STRING_PORTABLE_ATTRIBUTES_H

/**
 * @file portable_attributes.h
 * @brief 可移植的函数属性宏封装（C11/C17/C23 兼容）
 *
 * 支持：
 *   - C23 标准 [[...]] 属性（若编译器支持）
 *   - GCC / Clang __attribute__
 *   - MSVC __declspec / SAL 注解
 *
 * 定义以下宏（若平台支持）：
 *   NODISCARD        —— 返回值不应被忽略
 *   PURE             —— 无副作用，可读全局/内存
 *   FORMAT(fmt, args) —— 检查 printf 风格
 *   NONNULL(...)     —— 指定参数非空
 */

#ifndef PORTABLE_ATTRIBUTES_H
#define PORTABLE_ATTRIBUTES_H

/*─────────────────────────────── 特性检测 ───────────────────────────────*/

// 检查是否启用 C23 或更高
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#  define HAS_C23_ATTRIBUTES 1
#else
#  define HAS_C23_ATTRIBUTES 0
#endif

// 编译器识别
#if defined(__clang__)
#  define COMPILER_CLANG 1
#  define COMPILER_GCC 0
#  define COMPILER_MSVC 0
#elif defined(__GNUC__)
#  define COMPILER_CLANG 0
#  define COMPILER_GCC 1
#  define COMPILER_MSVC 0
#elif defined(_MSC_VER)
#  define COMPILER_CLANG 0
#  define COMPILER_GCC 0
#  define COMPILER_MSVC 1
#else
#  define COMPILER_CLANG 0
#  define COMPILER_GCC 0
#  define COMPILER_MSVC 0
#endif

/*─────────────────────────────── NODISCARD ───────────────────────────────*/

#if HAS_C23_ATTRIBUTES
#  define NODISCARD [[nodiscard]]
#elif COMPILER_CLANG
#  if __has_attribute(nodiscard)
#    define NODISCARD __attribute__((nodiscard))
#  elif __has_attribute(warn_unused_result)
#    define NODISCARD __attribute__((warn_unused_result))
#  else
#    define NODISCARD
#  endif
#elif COMPILER_GCC
#  if __GNUC__ >= 7
#    define NODISCARD __attribute__((warn_unused_result))
#  else
#    define NODISCARD
#  endif
#elif COMPILER_MSVC
#  if _MSC_VER >= 1700  // Visual Studio 2012+
#    include <sal.h>
#    define NODISCARD _Check_return_
#  else
#    define NODISCARD
#  endif
#else
#  define NODISCARD
#endif

/*─────────────────────────────── PURE ───────────────────────────────*/

// pure: 无副作用，但可读全局状态或通过指针读内存
#if COMPILER_CLANG || COMPILER_GCC
#  define PURE __attribute__((pure))
#elif COMPILER_MSVC
// MSVC 无直接等价物，但 __declspec(noalias) + 文档约定可部分替代
// 这里留空，依赖注释或 SAL
#  define PURE
#else
#  define PURE
#endif

/*─────────────────────────────── FORMAT ───────────────────────────────*/

// FORMAT(fmt_index, vararg_index)
// 示例: void log(const char* fmt, ...) FORMAT(1, 2);
#if COMPILER_CLANG || COMPILER_GCC
#  define FORMAT(fmt, args) __attribute__((format(printf, fmt, args)))
#elif COMPILER_MSVC
// MSVC 无编译期格式检查，但可用 SAL 注解辅助静态分析
// _Printf_format_string_ 需配合 /analyze 使用
#  include <sal.h>
#  define FORMAT(fmt, args) _Printf_format_string_
#else
#  define FORMAT(fmt, args)
#endif

/*─────────────────────────────── NONNULL ───────────────────────────────*/

// NONNULL()         → 所有指针参数非空
// NONNULL(1, 2)     → 第1、第2个参数非空
#if COMPILER_CLANG || COMPILER_GCC
#  define NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#elif COMPILER_MSVC
// 使用 SAL 注解：_In_ 表示输入且非空（默认）
// 注意：SAL 不强制编译错误，需 /analyze
#  include <sal.h>
// 由于 SAL 无法在函数声明层面指定“哪些参数”，我们提供占位宏
// 实际使用时建议在参数上加 _In_
#  define NONNULL(...)
#else
#  define NONNULL(...)
#endif

/*─────────────────────────────── 辅助宏（可选）──────────────────────────────*/

// 如果你希望在参数上直接标注（尤其 MSVC），可额外定义：
#if COMPILER_MSVC
#  define IN_NONNULL _In_
#  define IN_NULLABLE _In_opt_
#else
#  define IN_NONNULL
#  define IN_NULLABLE
#endif

#endif // PORTABLE_ATTRIBUTES_H

#endif //DYNAMIC_STRING_PORTABLE_ATTRIBUTES_H
