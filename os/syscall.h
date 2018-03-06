#ifndef _KOZOS_SYSCALL_H_INCLUDED_

#define _KOZOS_SYSCALL_H_INCLUDED_
#include "defines.h"

// システムコール番号の定義
typedef enum {
    KZ_SYSCALL_TYPE_RUN = 0,  // kz_run()のシステムコール番号
    KZ_SYSCALL_TYPE_EXIT,  // kz_exit()のシステムコール番号
} kz_syscall_type_t;

// システムコール呼び出し時のパラメータ格納域の定義
typedef struct {
    union {
        // kz_run()のためのパラメータ
        struct {
            kz_func_t func;  // メイン関数
            char *name;  // スレッド名
            int stacksize;  // スタックサイズ
            int argc;  // メイン関数に渡す引数の個数
            char **argv;  // メイン関数に渡す引数(argv形式)
            kz_thread_id_t ret;  // kz_run()戻り値
        } run;
        // kz_exit()のためのパラメータ
        struct {
            int dummy;  // パラメータなしだが、空は良くないのでダミーメンバを定義
        } exit;
    } un;  // 複数のパラメータ領域を同時に利用することはないため共用体で定義
} kz_syscall_param_t;

#endif // !_KOZOS_SYSCALL_H_INCLUDED_