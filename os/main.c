#include "defines.h"
#include "intr.h"
#include "interrupt.h"
#include "serial.h"
#include "lib.h"

// 割り込みハンドラ
static void intr(softvec_type_t type, unsigned long sp)
{
    int c;
    static char buf[32];  // 受信バッファ
    static int len;

    // 受信割り込みをきっかけにコンソールから1文字受信
    c = getc();
    if (c != '\n')
    {
        // 受信したものが改行でないならばバッファに保存
        buf[len++] = c;
    } else {
        buf[len++] = '\0';
        if (!strncmp(buf, "echo", 4))
        {
            puts(buf + 4);
            puts("\n");
        } else {
            puts("unknown.\n");
        }
        puts("> ");
        len = 0;
    }
}

int main(void)
{
    // 割り込み無効状態で初期化を行う
    INTR_DISABLE;
    puts("kozos boot suceed!\n");  // ブートメッセージを変更

    // ソフトウェア割り込みベクタにシリアル割り込みのハンドラを設定
    softvec_setintr(SOFTVEC_TYPE_SERINTR, intr);
    // シリアル受信割り込みを有効化
    serial_intr_recv_enable(SERIAL_DEFAULT_DEVICE);

    puts("> ");

    INTR_ENABLE;  // 割り込み有効化

    while (1)
    {
        asm volatile ("sleep");  // 省電力モードに移行
        // 以降は割り込みベースでの動作
    }
    puts("kozos end!\n");
    return 0;
}
