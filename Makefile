all: dns-server dns-client

dns-server:
			gcc -c -I h/ -L api/cnaiapi.o servDNS.c
			gcc -I h/ api/cnaiapi.o api/readln.o servDNS.o -o servDNS

dns-client:
			gcc -w -c -I h/ -L api/cnaiapi.o clDNS.c
			gcc -w -I h/ api/cnaiapi.o clDNS.c -o clDNS
