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
OBJS = startup.o main.o interrupt.o
OBJS += lib.o serial.o

# souces of kozos
OBJS += kozos.o syscall.o memory.o consdrv.o command.o

# 生成する実行形式ファイル名
TARGET = kozos

# コンパイルオプション
CFLAGS = -Wall -mh -nostdinc -nostdlib -fno-builtin
# CFLAGS += -mint32  # intを32bitにすると掛割ができなくなる
CFLAGS += -I.
# CFLAGS += -g
CFLAGS += -Os
CFLAGS += -DKOZOS

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

# *.cファイルのコンパイルルール
.c.o : $<
		$(CC) -c $(CFLAGS) $<

# アセンブラファイルのアセンブラルール
.s.o : $<
		$(CC) -c $(CFLAGS) $<

# *.Sのビルドルールを追加
.S.o : $<
		$(CC) -c $(CFLAGS) $<

clean :
		rm -f $(OBJS) $(TARGET) $(TARGET).elf