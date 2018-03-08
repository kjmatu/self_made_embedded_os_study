#ifndef _KOZOS_MEMORY_H_INCLUDED_
#define _KOZOS_MEMORY_H_INCLUDED_

// 動的メモリの初期化
int kzmem_init(void);

// 動的メモリの獲得
void *kzmem_alloc(int size);

// メモリの解放
void kzmem_free(void *mem);

#endif // !_KOZOS_MEMORY_H_INCLUDED_
