PREFIX = /usr/local
ARCH = h8300-elf
BINDIR = $(PREFIX)/bin
ADDNAME = $(ARCH)-

AR = $(BINDIR)/$(ADDNAME)ar
AS = $(BINDIR)/$(ADDNAME)as
CC = $(BINDIR)/$(ADDNAME)gcc
LD = $(BINDIR)/$(ADDNAME)ld
NM = $(BINDIR)/$(ADDNAME)nm
OBJCOPY = $(BINDIR)/$(ADDNAME)objcopy
OBJDUMP = $(BINDIR)/$(ADDNAME)objdump
RANLIB = $(BINDIR)/$(ADDNAME)ranlib
STRIP = $(BINDIR)/$(ADDNAME)strip

# コンパイルするソースコード
OBJS = vector.o startup.o intr.o main.o interrupt.o
OBJS += lib.o serial.o xmodem.o elf.o

# 生成する実行形式ファイル名
TARGET = kzload

# コンパイルオプション
CFLAGS = -Wall -mh -nostdinc -nostdlib -fno-builtin
# CFLAGS += -mint32  # intを32bitにすると掛割ができなくなる
CFLAGS += -I.
# CFLAGS += -g
CFLAGS += -Os
CFLAGS += -DKZLOAD

# リンクオプション
LFLAGS = -static -T ld.scr -L.

.SUFFIXES: .c .o
.SUFFIXES: .s .o
.SUFFIXES: .S .o

all : $(TARGET)

# 実行形式の生成ルール
$(TARGET) : $(OBJS)
			$(CC) $(OBJS) -o $(TARGET) $(CFLAGS) $(LFLAGS)
			cp $(TARGET) $(TARGET).elf
			$(STRIP) $(TARGET)
			$(OBJCOPY) -O srec $(TARGET) $(TARGET).mot

# *.cファイルのコンパイルルール
.c.o : $<
		$(CC) -c $(CFLAGS) $<

# アセンブラファイルのアセンブラルール
.s.o : $<
		$(CC) -c $(CFLAGS) $<

.S.o : $<
		$(CC) -c $(CFLAGS) $<

# モトローラSレコードフォーマットへの変換ルール
$(TARGET).mot : $(TARGET)
				$(OBJCOPY) -O srec $(TARGET) $(TARGET).mot

image : $(TARGET).mot

clean :
		rm -f $(OBJS) $(TARGET) $(TARGET).elf $(TARGET).mot *.o

# フラッシュ書き込みツール
H8WRITE = /mnt/d/workspace/kozos/tools/kz_h8write-v0.2.1/bin/kz_h8write

# シリアル接続先
H8WRITE_SERDEV = /dev/ttyS6

# フラッシュROMへの転送
write : $(TARGET).mot
		$(H8WRITE) -3069 -f20 $(TARGET).mot $(H8WRITE_SERDEV)
