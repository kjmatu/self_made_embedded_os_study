#include "defines.h"
#include "interrupt.h"
#include "serial.h"
#include "xmodem.h"
#include "elf.h"
#include "lib.h"

static int init(void)
{
	// 以下はリンカスクリプトで定義してあるシンボル
	extern int erodata, data_start, edata, bss_start, ebss;

	// データ領域とBSS(Block Started by Symbol)領域を初期化する
	// この処理以降でないとグローバル変数が初期化されないので注意
	// &erodata ROM上の.dataセクションの先頭アドレスから
	// &data_start RAM上の.dataセクションの先頭アドレスへコピー
	memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
	memset(&bss_start, 0, (long)&ebss - (long)&bss_start);

	// ソフトウェア割り込みベクタを初期化
	softvec_init();

	// シリアル初期化
	serial_init(SERIAL_DEFAULT_DEVICE);
	return 0;
}

static int dump(char *buf, long size)
{
	long i;
	if (size < 0)
	{
		puts((unsigned char *)"no data.\n");
		return -1;
	}

	for (i = 0; i < size; i++)
	{
		putxval(buf[i], 2);
		if ((i & 0xf) == 15) {
			puts((unsigned char *)"\n");
		} else {
			if ((i & 0xf) == 7)
			{
				puts((unsigned char *)" ");
			}
			puts((unsigned char *)" ");
		}
	}
	puts((unsigned char *)"\n");
	return 0;
}

static void wait()
{
	volatile long i;
	for (i = 0; i < 300000; i++)
	{
		;
	}
}

int main(void)
{
	static char buf[16];
	static long size = -1;
	static unsigned char *loadbuf = NULL;
	// リンカスクリプトで定義されているバッファ
	extern int buffer_start;

	INTR_DISABLE;  // 割り込み無効

	char *entry_point;
	void (*f)(void);

	init();

	puts((unsigned char *)"kzload (kozos boot loader) started.\n");

	while (1)
	{
		puts((unsigned char *)"kzload > ");  // プロンプトの表示
		gets((unsigned char *)buf);  // シリアルからのコマンド受信

		if (!strcmp(buf, "load")) { // XMODEMでのファイルダウンロード
			loadbuf = (unsigned char *)(&buffer_start);
			size = xmodem_recv((char *)loadbuf);
			wait(); // 転送アプリが終了し端末アプリに制御が戻るまで待つ
			if (size < 0) {
				puts((unsigned char *)"\nXMODEM receive error!\n");
			} else {
				puts((unsigned char *)"\nXMODEM receive suceeded.\n");
			}
		} else if (!strcmp(buf, "dump")) {  // メモリの16進ダンプ出力
			puts((unsigned char *)"size: ");
			putxval(size, 0);
			puts((unsigned char *)"\n");
			dump((char*)loadbuf, size);
		} else if (!strcmp(buf, "run")) {
			// ELF形式ファイルの実行
			entry_point = elf_load((char*)loadbuf);  // メモリ上に展開
			if (!entry_point) {
				puts((unsigned char *)"run error\n");
			} else {
				puts((unsigned char *)"starting from entry point: ");
				putxval((unsigned long)entry_point, 0);
				puts((unsigned char *)"\n");
				f = (void (*)(void))entry_point;
				f();  // ここでロードしたプログラムに処理を渡す
				// ここ以降は実行されない
			}
		} else {
			puts((unsigned char *)"unknown.\n");
		}
	}
	return 0;
}
