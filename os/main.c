#include "defines.h"
#include "kozos.h"
#include "interrupt.h"
#include "lib.h"

// システムタスクとユーザースレッドの起動
static int start_threads(int argc, char *argv[])
{
    // コマンド処理スレッドを起動
    kz_run(test08_1_main, "command", 0x100, 0, NULL);
    return 0;
}

int main(void)
{
    // 割り込み無効
    INTR_DISABLE;

    puts("kozos boot succeed!\n");

    // OSの動作開始
    // 初期スレッドとしてstart_threads()を起動し
    // OSの動作を開始する
    kz_start(start_threads, "start", 0x100, 0, NULL);

    // ここにはこない
    return 0;
}