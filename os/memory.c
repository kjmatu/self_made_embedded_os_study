#include "defines.h"
#include "kozos.h"
#include "lib.h"
#include "memory.h"

// メモリブロック構造体
// 獲得された各領域は先頭に以下の構造体を持っている
typedef struct _kzmem_block
{
    int size;
    int num;
} kzmem_block;

// メモリプール
// ブロックサイズ毎に確保される
typedef struct _kzmem_pool
{
    int size;
    int num;
    kzmem_block *free;
} kzmem_pool;

// メモリプールの定義(個々のサイズと個数)
static kzmem_pool pool[] = {
    // 16バイト、32バイト、64バイトの3種類のメモリプールを定義
    {16, 8, NULL}, {32, 8, NULL}, {64, 4, NULL},
}

// メモリプールの種類の個数
#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))