#include "defines.h"
#include "kozos.h"
#include "lib.h"

int test08_1_main(int argc, char *argv[])
{
    char buf[32];

    puts("test08_1 started.\n");

    while (1)
    {
        puts("> ");
        // コンソールからの行単位読み込み
        gets(buf);
        if (!strncmp(buf, "echo", 4))
        {
            // echoコマンドの場合、後続の文字列を出力
            puts(buf + 4);
            puts("\n");
        } else if (!strncmp(buf, "exit", 4))
        {
            break;
        } else
        {
            puts("unknown.\n");
        }
    }
    puts("test08_1 exit.\n");
    return 0;
}
