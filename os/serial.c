#include "defines.h"
#include "serial.h"

#define SERIAL_SCI_NUM 3  // Serial Control Interfaceの数

#define H8_3069F_SCI0 ((volatile struct h8_3069f_sci *)0xffffb0)
#define H8_3069F_SCI1 ((volatile struct h8_3069f_sci *)0xffffb8)
#define H8_3069F_SCI2 ((volatile struct h8_3069f_sci *)0xffffc0)

struct h8_3069f_sci
{
	volatile uint8 smr;  // シリアル通信モード設定
	volatile uint8 brr;  // ボーレート設定
	volatile uint8 scr;  // 送受信の有効無効
	volatile uint8 tdr;  // 送信した1文字を書き込む
	volatile uint8 ssr;  // 送信完了 受信完了などを表す
	volatile uint8 rdr;  // 受信した1文字を読み出す
	volatile uint8 scmr; // ICカード(スマートカード)インタフェース切り替え
};

// SMR各ビット定義
#define H8_3069F_SCI_SMR_CKS_PER1 (0<<0)
#define H8_3069F_SCI_SMR_CKS_PER4 (1<<0)
#define H8_3069F_SCI_SMR_CKS_PER16 (2<<0)
#define H8_3069F_SCI_SMR_CKS_PER64 (3<<0)
#define H8_3069F_SCI_SMR_MP (1<<2)
#define H8_3069F_SCI_SMR_STOP (1<<3)
#define H8_3069F_SCI_SMR_OE (1<<4)
#define H8_3069F_SCI_SMR_PE (1<<5)
#define H8_3069F_SCI_SMR_CHR (1<<6)
#define H8_3069F_SCI_SMR_CA (1<<7)

// SCR各ビット定義
#define H8_3069F_SCI_SCR_CKE0 (1<<0)
#define H8_3069F_SCI_SCR_CKE1 (1<<1)
#define H8_3069F_SCI_SCR_TEIE (1<<2)
#define H8_3069F_SCI_SCR_MPIE (1<<3)
#define H8_3069F_SCI_SCR_RE (1<<4)  //受信有効
#define H8_3069F_SCI_SCR_TE (1<<5)  //送信有効
#define H8_3069F_SCI_SCR_RIE (1<<6)  //受信割り込み有効
#define H8_3069F_SCI_SCR_TIE (1<<7)  //送信割り込み有効

// SSR各ビット定義
#define H8_3069F_SCI_SSR_MPBT (1<<0)
#define H8_3069F_SCI_SSR_MPB (1<<1)
#define H8_3069F_SCI_SSR_TEND (1<<2)
#define H8_3069F_SCI_SSR_PER (1<<3)
#define H8_3069F_SCI_SSR_FERERS (1<<4)
#define H8_3069F_SCI_SSR_ORER (1<<5)
#define H8_3069F_SCI_SSR_RDRF (1<<6)  //受信完了
#define H8_3069F_SCI_SSR_TDRE (1<<7)  //送信完了

static struct
{
	volatile struct h8_3069f_sci *sci;
} regs[SERIAL_SCI_NUM] = {
	{ H8_3069F_SCI0 },
	{ H8_3069F_SCI1 },
	{ H8_3069F_SCI2 },
};


// デバイス初期化
int serial_init(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;

	sci->scr = 0;
	sci->smr = 0;
	//20MHzのクロックから9600bpsを生成(25MHzの場合は80にする)
	sci->brr = 64;
	//送受信可能
	sci->scr = H8_3069F_SCI_SCR_RE | H8_3069F_SCI_SCR_TE;
	sci->ssr = 0;
	return 0;
}

// 送信可能か
int serial_is_send_enable(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	return (sci->ssr & H8_3069F_SCI_SSR_TDRE);
}

// 1文字送信
int serial_send_byte(int index, unsigned char c)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;

	// 送信可能になるまで待つ
	while(!serial_is_send_enable(index));
	sci->tdr = c;
	// 送信開始
	sci->ssr &= ~H8_3069F_SCI_SSR_TDRE;
	return 0;
}

// 受信可能か
int serial_is_recv_enable(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	return (sci->ssr & H8_3069F_SCI_SSR_RDRF);
}

// 1文字受信
unsigned char serial_recv_byte(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	unsigned char c;

	// 受信文字が来るまで待つ
	while (!serial_is_recv_enable(index))
	{
		;
	}
	c = sci->rdr;
	sci->ssr &= ~H8_3069F_SCI_SSR_RDRF;  // 受信完了 次データの受信を可能にする
	return c;
}

// 送信割り込み確認
int serial_intr_is_send_enable(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	// SCRのTIEビットの値を返す
	return (sci->scr & H8_3069F_SCI_SCR_TIE) ? 1 : 0;
}

// 送信割り込みの有効化
void serial_intr_send_enable(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	// SCRのTIEビットを立てる
	sci->scr |= H8_3069F_SCI_SCR_TIE;
}

// 送信割り込みの無効化
void serial_intr_send_disable(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	// SCRのTIEビットを落とす
	sci->scr &= ~H8_3069F_SCI_SCR_TIE;
}

// 受信割り込み確認
int serial_intr_is_recv_enable(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	// SCRのRIEビットの値を返す
	return (sci->scr & H8_3069F_SCI_SCR_RIE) ? 1 : 0;
}

// 受信割り込みの有効化
void serial_intr_recv_enable(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	// SCRのRIEビットを立てる
	sci->scr |= H8_3069F_SCI_SCR_RIE;
}

// 受信割り込みの無効化
void serial_intr_recv_disable(int index)
{
	volatile struct h8_3069f_sci *sci = regs[index].sci;
	// SCRのRIEビットを落とす
	sci->scr &= ~H8_3069F_SCI_SCR_RIE;
}
