# Makefile
CC=gcc
CFLAGS_DBG=-DDBG -g -ggdb -Og -Wall
CFLAGS=-O2 -Wall

ALL := raw_sock_ethframe raw_eth_ipv4_udp_tx
all: ${ALL}

raw_sock_ethframe: raw_sock_ethframe.c
	${CC} ${CFLAGS} raw_sock_ethframe.c -o raw_sock_ethframe
raw_eth_ipv4_udp_tx: raw_eth_ipv4_udp_tx.c
	${CC} ${CFLAGS} raw_eth_ipv4_udp_tx.c -o raw_eth_ipv4_udp_tx
clean:
	rm -f ${ALL}
