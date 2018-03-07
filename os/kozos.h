#ifndef _KOZOS_H_INCLUDED_
#define _KOZOS_H_INCLUDED_

#include "defines.h"
#include "syscall.h"

// システムコール
// スレッドの起動のシステムコール
kz_thread_id_t kz_run(kz_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]);

// スレッド終了のシステムコール
void kz_exit(void);

// カレントスレッドをレディーキューの後ろにつなぎ、スレッドを切り替える
int kz_wait(void);

// スレッドをレディーキューから外してスリープ状態にする
int kz_sleep(void);

// スリープ状態のスレッドをレディーキューにつなぎ直して、レディ状態に戻す
int kz_wakeup(kz_thread_id_t id);

// 自身のスレッドIDを取得する
kz_thread_id_t kz_getid(void);

// スレッドの優先度を変更する
int kz_chpri(int priority);

// ライブラリ関数
// 初期スレッドを起動し、OSの動作を開始する
void kz_start(kz_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]);

// 致命的エラーのときに呼び出す
void kz_sysdown(void);

// システムコールを実行する
void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t *param);

// ユーザースレッド
// ユーザースレッドのメイン関数
int test09_1_main(int argc, char *argv[]);
int test09_2_main(int argc, char *argv[]);
int test09_3_main(int argc, char *argv[]);
extern kz_thread_id_t test09_1_id;
extern kz_thread_id_t test09_2_id;
extern kz_thread_id_t test09_3_id;
#endif // !_KOZOS_H_INCLUDED_
