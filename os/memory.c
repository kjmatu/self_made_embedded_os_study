#include "defines.h"
#include "kozos.h"
#include "lib.h"
#include "memory.h"

// メモリブロック構造体
// 獲得された各領域は先頭に以下の構造体を持っている
typedef struct _kzmem_block
{
    struct _kzmem_block *next;
    int size;
} kzmem_block;

// メモリプール
// ブロックサイズ毎に確保される
typedef struct _kzmem_pool
{
    // メモリブロックのサイズ
    int size;
    // メモリブロックの個数
    int num;
    // 解放済み領域のリンクリストへのポインタ
    kzmem_block *free;
} kzmem_pool;

// メモリプールの定義(個々のサイズと個数)
static kzmem_pool pool[] = {
    // 16バイト、32バイト、64バイトの3種類のメモリプールを定義
    {16, 8, NULL}, {32, 8, NULL}, {64, 4, NULL},
}

// メモリプールの種類の個数
#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))

// メモリプールの初期化
static int kzmem_init_pool(kzmem_pool *p)
{
    int i;
    kzmem_block *mp;  // メモリブロックを指すポインタ
    kzmem_block **mpp;  // 解放済みリンクリスト
    extern char freearea;  // リンカスクリプトで定義される空き領域
    static char *area = &freearea;

    // 空き領域の先頭をメモリブロックを指すポインタに設定
    mp = (kzmem_block *)area;

    // 各メモリブロックを全て解放済みリンクリストにつなぐ
    mpp = &p->free;

    // 動的メモリ用の領域から必要個数だけブロックを切り出す
    // その後、解放済みリンクリストにつなぐ
    for (i = 0; i < p->num; i++)
    {
        // メモリヘッダのnextポインタのアドレスを指す
        *mpp = mp;

        // メモリヘッダを0で初期化
        memset(mp, 0, sizeof(*mp));

        // メモリヘッダ(サイズ)を設定
        mp->size = p->size;

        // ひとつ先のメモリブロックのnextポインタのアドレスを指す
        mpp = &(mp->next);

        // メモリブロックを指すmpをひとつ先のブロックにすすめる
        mp = (kzmem_block *) ((char *)mp + p->size);
        area += p->size;
    }
    return 0;
}

// 動的メモリの初期化
int kzmem_init(void)
{
    int i;
    for (i = 0; i < MEMORY_AREA_NUM; i++)
    {
        // 各メモリプールを初期化する
        kzmem_init_pool(&pool[i]);
    }
    return 0;
}
