#include "defines.h"
#include "kozos.h"
#include "interrupt.h"
#include "lib.h"

// システムタスクとユーザースレッドの起動
static int start_threads(int argc, char *argv[])
{
    // サンプルプログラムを起動
    kz_run(test10_1_main, "test10_1", 1, 0x100, 0, NULL);

    // 優先順位を下げて、アイドルスレッドに移行する
    kz_chpri(15);
    // 割り込み有効
    INTR_ENABLE;
    while (1)
    {
        // 省電力モードに移行
        asm volatile ("sleep");
    }
    return 0;
}

int main(void)
{
    // 割り込み無効
    INTR_DISABLE;

    puts("kozos boot succeed!\n");

    // 初期スレッドとしてstart_threads()を優先度0で起動し
    // OSの動作を開始する
    kz_start(start_threads, "idle", 0, 0x100, 0, NULL);

    // ここにはこない
    return 0;
}
