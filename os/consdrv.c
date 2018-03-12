#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "serial.h"
#include "lib.h"
#include "consdrv.h"

#define CONS_BUFFER_SIZE 24

static struct consreg {
    kz_thread_id_t id;  // コンソールを利用するスレッドID
    int index;  // 利用するシリアルの番号

    char *send_buf;  // 送信バッファ
    char *recv_buf;  // 受信バッファ
    int send_len;  // 送信バッファ中のデータサイズ
    int recv_len;  // 受信バッファ中のデータサイズ

    // kozos.cのkz_msgboxと同様の理由でダミーメンバでサイズ調整する
    long dummy[3];
} consreg[CONS_BUFFER_SIZE];  // 複数のコンソールを管理可能にするために配列にする