all:
	gcc -c -std=gnu99 ServerY.c
	gcc -o ServerYExe ServerY.o -pthread -lm -lrt
	gcc -c -std=gnu99 ServerZ.c
	gcc -o ServerZExe ServerZ.o -pthread -lm -lrt
	gcc -c -std=gnu99 Client.c
	gcc -o ClientExe Client.o -pthread -lm -lrt
	@echo "Usage: ./ServerYExe [portNumber] [IpAddressZ] [portNumber] [PoolProSize] [PoolForwardSize] [time] [LogFile] "
	@echo "Usage: ./ServerZExe [portnumber] [PoolProSize] [time] [LogFile]"
	@echo "Usage: ./ClientExe  [ClientName] [IpAddressY] [portNumberY]"