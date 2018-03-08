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

// 動的メモリの確保
void *kz_kmalloc(int size);

// 動的メモリの解放
int kz_kmfree(void *p);

// メッセージ送信
int kz_send(kz_msgbox_id_t id, int size, char *p);

// メッセージ受信
kz_thread_id_t kz_recv(kz_msgbox_id_t id, int *sizep, char **p);

// ライブラリ関数
// 初期スレッドを起動し、OSの動作を開始する
void kz_start(kz_func_t func, char *name, int priority, int stacksize, int argc, char *argv[]);

// 致命的エラーのときに呼び出す
void kz_sysdown(void);

// システムコールを実行する
void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t *param);

// ユーザースレッド
// ユーザースレッドのメイン関数
int test11_1_main(int argc, char *argv[]);
int test11_2_main(int argc, char *argv[]);

#endif // !_KOZOS_H_INCLUDED_
