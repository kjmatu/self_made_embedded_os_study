#include "defines.h"
#include "kozos.h"
#include "syscall.h"

// システムコール
kz_thread_id_t kz_run(kz_func_t func, char *name, int stacksize, int argc, char *argv[])
{
    // スタックはスレッドごとに確保されるので
    // パラメータ域は自動変数としてスタック上に確保する
    kz_syscall_param_t param;

    // 引数として渡されたパラメータを設定する
    param.un.run.func = func;
    param.un.run.name = name;
    param.un.run.stacksize = stacksize;
    param.un.run.argc = argc;
    param.un.run.argv = argv;

    // システムコールを呼び出す
    kz_syscall(KZ_SYSCALL_TYPE_RUN, &param);

    // システムコールの応答が構造体に格納されるので
    // 戻り値として返す
    return param.un.run.ret;
}

// kz_exit()の実体
void kz_exit(void)
{
    // システムコールを呼び出す
    kz_syscall(KZ_SYSCALL_TYPE_EXIT, NULL);
}
