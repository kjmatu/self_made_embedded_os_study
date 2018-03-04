#include "intr.h"
    .h8300h
    .section .text

    .global _intr_softerr
#   .type   _intr_softerr,@function
;ソフトウェアエラー
_intr_softerr:
    ;汎用レジスタの値をスタックに保存する
    mov.l er6,@-er7
    mov.l er5,@-er7
    mov.l er4,@-er7
    mov.l er3,@-er7
    mov.l er2,@-er7
    mov.l er1,@-er7
    mov.l er0,@-er7

    ;第2引数にスタックポインタを設定
    mov.l er7,er1

    mov.w #SOFTVEC_TYPE_SOFTERR,r0
    jsr   @_interrupt; interrupt()の呼び出し

    ;スタックから汎用レジスタの値を復旧する
    mov.l @er7+,er0
    mov.l @er7+,er1
    mov.l @er7+,er2
    mov.l @er7+,er3
    mov.l @er7+,er4
    mov.l @er7+,er5
    mov.l @er7+,er6
    rte; 割り込み復帰命令

    .global _intr_syscall
#   .type   _intr_syscall,@function
;システムコール
_intr_syscall:
    mov.l er6,@-er7
    mov.l er5,@-er7
    mov.l er4,@-er7
    mov.l er3,@-er7
    mov.l er2,@-er7
    mov.l er1,@-er7
    mov.l er0,@-er7
    mov.l er7,er1
    mov.w #SOFTVEC_TYPE_SYSCALL,r0
    jsr   @_interrupt
    mov.l @er7+,er0
    mov.l @er7+,er1
    mov.l @er7+,er2
    mov.l @er7+,er3
    mov.l @er7+,er4
    mov.l @er7+,er5
    mov.l @er7+,er6
    rte

    .global _intr_serintr
#   .type   _intr_serintr,@function
;シリアル割り込み
_intr_serintr:
    mov.l er6,@-er7
    mov.l er5,@-er7
    mov.l er4,@-er7
    mov.l er3,@-er7
    mov.l er2,@-er7
    mov.l er1,@-er7
    mov.l er0,@-er7
    mov.l er7,er1
    mov.w #SOFTVEC_TYPE_SERINTR,r0
    jsr   @_interrupt
    mov.l @er7+,er0
    mov.l @er7+,er1
    mov.l @er7+,er2
    mov.l @er7+,er3
    mov.l @er7+,er4
    mov.l @er7+,er5
    mov.l @er7+,er6
    rte
