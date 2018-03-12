#include "defines.h"
#include "kozos.h"
#include "consdrv.h"
#include "lib.h"

// コンソールドライバの使用開始をコンソールドライバに依頼する
// コンソールの初期化を依頼
static void send_use(int index)
{
    char *p;
    // コマンド通知用の領域を確保
    p = kz_kmalloc(3);
    p[0] = '0';
    p[1] = CONSDRV_CMD_USE;  // 初期化コマンドを設定
    p[2] = '0' + index;
    // コンソールドライバスレッドに送信
    kz_send(MSGBOX_ID_CONSOUTPUT, 3, p);
}

// コンソールへの文字列出力をコンソールドライバに依頼する
// 文字列の出力を依頼
static void send_write(char *str)
{
    char *p;
    int len;
    len = strlen(str);
    // コマンド通知用の領域を確保
    p = kz_kmalloc(len + 2);
    p[0] = '0';
    p[1] = CONSDRV_CMD_WRITE;  // 文字列出力コマンドを設定
    memcpy(&p[2], str, len);
    // コンソールドライバスレッドに送信
    kz_send(MSGBOX_ID_CONSOUTPUT, len + 2, p);
}

// コマンドスレッドのメイン関数
int command_main(int argc, char *argv[])
{
    char *p;
    int size;

    // コンソールドライバスレッドにコンソールの初期化依頼をする
    send_use(SERIAL_DEFAULT_DEVICE);

    while (1)
    {
        // プロンプト表示
        send_write("command > ");

        // コンソールドライバスレッドから受信文字列を受け取る
        kz_recv(MSGBOX_ID_CONSINPUT, &size, &p);
        p[size] = '\0';

        // echoコマンド
        if (!strncmp(p, "echo", 4))
        {
            send_write(p + 4);  // echoに続く文字列を出力する
            send_write("\n");
        }
        else
        {
            send_write("unkown.\n");
        }
        // メッセージにより受信した領域(送信元で確保されたもの)を解放
        kz_kmfree(p);
    }
    return 0;
}
