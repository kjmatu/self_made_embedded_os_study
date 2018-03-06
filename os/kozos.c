#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "syscall.h"
#include "lib.h"

#define THREAD_NUM 6  // TCBの個数
#define THREAD_NAME_SIZE 15  // スレッド名の最大長

// スレッドコンテキスト
// スレッドコンテキスト保存用の構造体定義
typedef struct _kz_context {
    // 汎用レジスタはスタックに保存されるので、TCBにコンテキストとして保存するのはスタックポインタのみ
    uint32 sp;  //  スタックポインタ
} kz_context;

// タスクコントロールブロック(TCB)
typedef struct _kz_thread {
    // レディーキューへの接続に利用するnextポインタ
    struct _kz_thread *next;
    // スレッド名
    char name[THREAD_NAME_SIZE];
    // スレッドのスタック
    char *stack;

    // スレッドのスタートアップ(thread_init())に渡すパラメータ
    struct {
        // スレッドのメイン関数
        kz_func_t func;
        // メイン関数に渡すargc
        int argc;
        // メイン関数に渡すargv
        char **argv;
    } init;

    // システムコール用のバッファ
    // システムコール発行時に利用するパラメータ領域
    struct {
        kz_syscall_type_t type;
        kz_syscall_param_t *param;
    } syscall;

    kz_context context;
} kz_thread;

// スレッドのレディーキュー
static struct {
    kz_thread *head;  // 先頭エントリ
    kz_thread *tail;  // 末尾エントリ
} readyque;

// 実行中のスレッド
static kz_thread *current;

// タスクコントロールブロック
static kz_thread threads[THREAD_NUM];

// 割り込みハンドラ OSが管理する
static kz_handler_t handlers[SOFTVEC_TYPE_NUM];

// スレッドディスパッチ用関数
// 実体はstartup.sにアセンブラで実装
void dispatch(kz_context *context);

// カレントスレッドをレディーキューから取り出す
static int getcurrent(void)
{
    if (current == NULL)
    {
        return -1;
    }
    // カレントスレッドは必ず先頭にあるので先頭から抜き出す
    // 先頭のエントリを抜き出す
    readyque.head = current->next;
    if (readyque.head == NULL)
    {
        readyque.tail = NULL;
    }
    // エントリをリンクリストから抜き出したのでnextポインタをクリアしておく
    current->next = NULL;
    return 0;
}

// カレントスレッドをレディーキューに繋げる
static int putcurrent(void)
{
    if (current == NULL)
    {
        return -1;
    }

    // レディーキューの末尾に接続する
    if (readyque.tail){
        readyque.tail->next = current;
    } else {
        readyque.head = current;
    }
    readyque.tail = current;
    return 0;
}

// スレッドの終了
static void thread_end(void)
{
    kz_exit();
}

// スレッドの開始
static void thread_init(kz_thread *thp)
{
    // スレッドのメイン関数を呼び出す
    thp->init.func(thp->init.argc, thp->init.argv);
    // メイン関数が終了したらスレッドを終了
    thread_end();
}

// システムコールの処理(kz_run(): スレッドの起動)
static kz_thread_id_t thread_run(kz_func_t func, char *name, int stacksize, int argc, char *argv[])
{
    int i;
    kz_thread *thp;
    uint32 *sp;
    extern char userstack;  // リンカスクリプトで定義されるスタック領域
    static char *thread_stack = &userstack;  // ユーザースタックに利用される領域

    // 開いているタスクコントロールブロックを検索
    for (i = 0; i < THREAD_NUM; i++)
    {
        thp = &threads[i];
        if (!thp->init.func)
        {
            // 見つかった
            break;
        }
    }
    if (i == THREAD_NUM)
    {
        // 見つからなかった
        return -1;
    }

    // TCBを0クリア
    memset(thp, 0, sizeof(*thp));

    // TCBの設定 各種パラメータを入力
    strcpy(thp->name, name);
    thp->next = NULL;
    thp->init.func = func;
    thp->init.argc = argc;
    thp->init.argv = argv;

    // スタック領域を獲得
    memset(thread_stack, 0, stacksize);

    //thread_stackはユーザースタック用意気の未使用部分の先頭を指す
    thread_stack += stacksize;
    thp->stack = thread_stack;  // スタックを設定

    // スタックの初期化
    // スタックにthread_init()からの戻り先としてthread_end()を設定する
    sp = (uint32 *)thp->stack;
    *(--sp) = (uint32)thread_end;

    // プログラムカウンタを設定する
    // ディスパッチ時にプログラムカウンタに格納される値としてthread_init()を設定する
    // よって、スレッドはthread_init()から動作を開始する
    *(--sp) = (uint32)thread_init;

    *(--sp) = 0;  // ER6
    *(--sp) = 0;  // ER5
    *(--sp) = 0;  // ER4
    *(--sp) = 0;  // ER3
    *(--sp) = 0;  // ER2
    *(--sp) = 0;  // ER1

    // スレッドのスタートアップ(thread_init())に渡す引数
    *(--sp) = (uint32)thp;  // ER0 第1引数

    // スレッドのコンテキストを設定
    // コンテキストとしてスタックポインタを保存
    thp->context.sp = (uint32)sp;

    // システムコールを呼び出したスレッドをレディーキューに戻す
    putcurrent();

    // 新規作成したスレッドをレディーキューに接続する
    current = thp;
    putcurrent();

    // 新規作成したスレッドのスレッドIDを戻り値として返す
    return (kz_thread_id_t)current;
}

// システムコールの処理(kz_exit():スレッドの終了)
static int thread_exit(void)
{
    // 本来ならスタックも開放して再利用できるようにすべきだが省略
    // このため頻繁にスレッドを生成、消去はできない
    puts(current->name);
    puts("EXIT.\n");
    // TCBクリア
    memset(current, 0, sizeof(*current));
    return 0;
}

// 割り込みハンドラの登録
static int setintr(softvec_type_t type, kz_handler_t handler)
{
    static void thread_intr(softvec_type_t type, unsigned long sp);

    // 割り込みを受け付けるためにソフトウェア割り込みベクタに
    // OS割り込み処理の入り口となる関数を登録する
    softvec_setintr(type, thread_intr);
    handlers[type] = handler;  // OS側から呼び出す割り込みハンドラを登録
    return 0;
}

// システムコールの処理関数の呼び出し
static void call_functions(kz_syscall_type_t type, kz_syscall_param_t *p)
{
    // システムコールの実行中にcurrentが書き換わるので注意
    switch (type)  // 各種システムコールに応じて処理用の関数を呼び出す
    {
    case KZ_SYSCALL_TYPE_RUN:
        p->un.run.ret = thread_run(p->un.run.func, p->un.run.name, p->un.run.stacksize, p->un.run.argc, p->un.run.argv);
        break;
    case KZ_SYSCALL_TYPE_EXIT:
        // TCBが消去されるので戻り値を書き込んではいけない
        thread_exit();
        break;
    default:
        break;
    }
}

// システムコールの処理
static void syscall_proc(kz_syscall_type_t type, kz_syscall_param_t *p)
{
    // システムコールで呼び出したスレッドをレディーキューから外した状態で処理関数を呼び出す
    // このためシステムコールを呼び出したスレッドをそのまま動作継続させたい場合には
    // 処理関数の内部でputcurrent()を行う必要がある
    getcurrent();  // カレントスレッドをレディーキューから外す
    call_functions(type, p);  // システムコールの処理関数を呼び出す
}

// スレッドのスケジューリング
static void schedule(void)
{
    if (!readyque.head)
    {
        // 見つからなかった
        kz_sysdown();
    }
    current = readyque.head;  // カレントスレッドに設定する
}

// システムコールの呼び出し
static void syscall_intr(void)
{
    syscall_proc(current->syscall.type, current->syscall.param);
}

// ソフトウェアエラーの発生
static void softerr_intr(void)
{
    puts(current->name);
    puts(" DOWN.\n");
    getcurrent();  // レディーキューから外す
    thread_exit();  // スレッドを終了
}

// 割り込み処理の入り口関数
static void thread_intr(softvec_type_t type, unsigned long sp)
{
    // カレントスレッドのコンテキストを保存する
    current->context.sp = sp;

    // 割り込みごとの処理を実行する
    // SOFTVEC_TYPE_SYSCALL, SOFTVEC_TYPE_SOFTERRの場合は
    // syscall_intr(),softerr_intr()がハンドラに登録されているのでそれらが実行されるd
    if (handlers[type])
    {
        // 割り込みに対応した各ハンドラを実行する
        handlers[type]();
    }
    schedule();  // 次に動作するスレッドのスケジューリング

    // スレッドのディスパッチ
    // dispatch()関数の本体はstartup.sにありアセンブラで記述
    dispatch(&current->context);
    // ディスパッチ後はそのスレッドの動作に入るのでここには来ない
}

// 初期スレッドを起動し、OSの動作を開始する
void kz_start(kz_func_t func, char *name, int stacksize, int argc, char *argv[])
{
    // 以降で呼び出すスレッド関連のライブラリ関数の内部でcurrentを参照している場合があるので
    // currentをNULLに初期化しておく
    current = NULL;

    // 各種データの初期化
    readyque.head = readyque.tail = NULL;
    memset(threads, 0, sizeof(threads));
    memset(handlers, 0, sizeof(handlers));

    // 割り込みハンドラの登録 ソフトウェア割り込みベクタを設定する
    setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr);
    setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr);

    // システムコール発行不可なので直接関数を呼び出してスレッドを作成する
    current = (kz_thread *)thread_run(func, name, stacksize, argc, argv);

    // 最初のスレッドを起動
    dispatch(&current->context);
    // ここにはこない
}

// OS内部で致命的エラーが発生した場合にはこの関数を呼ぶ
void kz_sysdown(void)
{
    puts("system error!\n");
    while (1)
    {
        // 無限ループに入って停止
    }
}

// システムコール呼び出し用ライブラリ関数
void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t *param)
{
    // システムコール番号の設定
    current->syscall.type = type;

    // パラメータの設定
    current->syscall.param = param;

    // トラップ割り込み発行
    asm volatile("trapa #0");
}
