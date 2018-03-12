#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "serial.h"
#include "lib.h"
#include "consdrv.h"

#define CONS_BUFFER_SIZE 24

// コンソール管理用構造体
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

// 以下の関数(send_char(), send_string())は割り込み処理とスレッドから呼ばれるが
// 送信バッファを操作しており、再入不可のためスレッドから呼び出す場合は排他のため割り込み禁止状態で呼ぶこと

// 送信バッファの先頭1文字を送信する
static void send_char(struct consreg *cons)
{
    int i;
    serial_send_byte(cons->index, cons->send_buf[0]);
    cons->send_len--;
    // 先頭文字を送信したので1文字ぶんずらす
    for (i = 0; i < cons->send_len; i++)
    {
        cons->send_buf[i] = cons->send_buf[i + 1];
    }
}

// 文字列を送信バッファに書き込み送信開始する
static void send_string(struct consreg *cons, char *str, int len)
{
    int i;
    // 文字列を送信バッファにコピー
    for (i = 0; i < len; i++)
    {
        // \nを\r\nに変換
        if (str[i] == '\n')
        {
            cons->send_buf[cons->send_len++] = '\r';
        }
        cons->send_buf[cons->send_len++] = str[i];
    }

    // 送信割り込み無効ならば、送信開始されていないので送信開始する
    // 送信割り込み有効ならば、送信開始されており送信割り込みの延長で
    // 送信バッファ内のデータが順次送信されるので何もしなくてよい
    if (cons->send_len && !serial_intr_is_send_enable(cons->index))
    {
        // 送信割り込み有効化
        serial_intr_send_enable(cons->index);
        // 送信開始
        send_char(cons);
    }
}