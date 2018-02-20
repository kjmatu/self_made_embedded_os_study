#include "defines.h"
#include "serial.h"
#include "lib.h"

// 1文字送信(端末変換あり)
// シリアルへの文字出力関数
int putc(unsigned char c)
{
	if (c == '\n')
	{
		serial_send_byte(SERIAL_DEFAULT_DEVICE, '\r');
	}
	return serial_send_byte(SERIAL_DEFAULT_DEVICE, c);
}

// 文字列送信(端末変換なし)
int puts(unsigned char *str)
{
	while(*str)
	{
		putc(*(str++));
	}
	return 0;
}

void *memset(void *b, int c, long len)
{
	// ポインタ演算はvoid型のままではできないためchar型を用意する
	char *p;
	for (p = b; len > 0; len--)
	{
		// sizeof(char)づつポインタを移動させる
		// TODO: char型(?bit)にint型(16bit)を代入しているがいいのか？
		*(p++) = c;
	}
	return b;
}

void *memcpy(void *dst, const void *src, long len)
{
	char *d = dst;
	const char *s = src;
	for (; len > 0; len--)
	{
		*(d++) = *(s++);
	}
	return dst;
}

int memcmp(const void *b1, const void *b2, long len)
{
	const char *p1 = b1, *p2 = b2;
	for (; len > 0; len--)
	{
		if (*p1 != *p2)
			return (*p1 > *p2) ? 1 : -1;
		p1++;
		p2++;
	}
	return 0;
}

int strlen(const char *s)
{
	int len;
	for (len = 0; *s; s++, len++)
	{
		// do nothing
	}
	return len;
}

char *strcpy(char *dst, const char *src)
{
	char *d = dst;
	for (;; dst++, src++)
	{
		*dst = *src;
		if (!*src) break;  //
	}
	return d;
}

int strcmp(const char *s1, const char *s2)
{
	while (*s1 || *s2)
	{
		if (*s1 != *s2)
		{
			return (*s1 > *s2) ? 1 : -1;
		}
		s1++;
		s2++;
	}
	return 0;
}

int strncmp(const char *s1, const char *s2, int len)
{
	while ((*s1 || *s2) && (len > 0))
	{
		if (*s1 != *s2)
		{
			return (*s1 > *s2) ? 1 : -1;
		}
		s1++;
		s2++;
		len--;
	}
	return 0;
}

int putxval(unsigned long value, int column)
{
	char buf[9];  // 文字列出力用
	char *p;

	// 下の桁から処理するので、バッファの終端から格納
	// 配列の先頭ポインタアドレス + 配列のサイズ - 1
	// bufで確保されたメモリアドレスの終端アドレスをpに格納
	p = buf + sizeof(buf) - 1;
	// 終端文字
	*(p--) = '\0';

	if (!value && !column)
	{
		column++;
	}

	while (value || column)
	{
		// 16進数に変換してバッファに格納する
		*(p--) = "0123456789abcdef"[value & 0xf];
		// 次の桁にすすめる(4bit sift)
		value >>= 4;  // value = value >> 4
		// 桁数指定がある場合にはカウントする
		if (column)
		{
			column--;
		}
	}
	// バッファの内容を出力する
	puts(p + 1);
	return 0;
}
