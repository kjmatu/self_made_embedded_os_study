#ifndef _INTR_H_INCLUDED_
#define _INTR_H_INCLUDED_

// ソフトウェア割り込みベクタの定義
#define SOFTVEC_TYPE_NUM 3  // ソフトウェア割り込みベクタの種別の個数
#define SOFTVEC_TYPE_SOFTERR 0  // ソフトウェアエラー
#define SOFTVEC_TYPE_SYSCALL 1  // システムコール
#define SOFTVEC_TYPE_SERINTR 2  // シリアル割り込み

#endif // !_INTR_H_INCLUDED_
