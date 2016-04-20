
#include "ShareMemory.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <string.h>
#include <pthread.h>

//./doc/ReadMe for more info
//atomic_t:since Linux2.6.18 is removed,use gcc "Built-in functions for atomic memory access" instead
//#include <asm/atomic.h>

typedef struct {  
    volatile int counter;  
} atomic_t;  

/*support spin lock*/
struct shmlock
{
	atomic_t sequence;
	pthread_spinlock_t writelock;
	int statue;
};

#define atomic_read(v)  (*(volatile int *)&(v)->counter)
#define atomic_inc(v) __sync_add_and_fetch(&(v)->counter,1)

int ShmCreat(char *filepath,int magickey,int sizeofshm,struct shmt *shmo,int shmflg)
{
	int shmid;
	void *shm;
	struct shmlock *shml; 

	key_t key = (key_t)ftok(filepath,magickey);
	if(key == -1)
	{
		return -200;
	}
    	shmid = shmget(key, sizeofshm+sizeof(struct shmlock), 0666|shmflg);
    	if(shmid == -1)
    	{
		return -1;
    	}
    	shm = (void *)shmat(shmid, 0, 0);
    	if(shm == NULL)
    	{
		shmctl(shmid,IPC_RMID,0);
		return -2;
    	}
	shml = (struct shmlock *)shm;

	shmo->shm = shm;
	shmo->shmid = shmid;
	shmo->size = sizeofshm;
	shml->statue = 1;
    	return 0;
}

void ShmInitSpinLock(struct shmt *shmo)
{
	struct shmlock *shml = (struct shmlock *)shmo->shm;
	pthread_spin_init(&(shml->writelock),PTHREAD_PROCESS_SHARED);
	return;
}

int ShmUnmap(struct shmt *shmo)
{
	int re;
	re = shmdt(shmo->shm);
	if(re == 0)
	{
		shmo->shm = NULL;
		shmo->shmid = -1;
		shmo->size = 0;
	}
	return re;
}

int ShmDestroy(struct shmt *shmo)
{
	int re=0;
	if(shmo->shm != NULL)
	{
		re = shmdt(shmo->shm);
	}
	if(re == 0)
	{
		if(shmo->shmid != -1)
		{
			re = shmctl(shmo->shmid, IPC_RMID, 0);
		}
		if(re == 0)
		{
			shmo->shm = NULL;
			shmo->shmid = -1;
			shmo->size = 0;
		}
	}
	return re;
}

int ShmRead(struct shmt *shmo,void *data)
{
	int sequence2,sequence1;
	int statue;
	//判断是否为偶数
	if(0x01 & (unsigned int)atomic_read(&(((struct shmlock *)(shmo->shm))->sequence)))
	{
		return -1;
	}
	sequence1 = atomic_read(&(((struct shmlock *)(shmo->shm))->sequence));
	memcpy(data,((char *)shmo->shm)+sizeof(struct shmlock),shmo->size);
	statue = ((struct shmlock *)(shmo->shm))->statue;
	sequence2 = atomic_read(&(((struct shmlock *)(shmo->shm))->sequence));
	if(statue)
	{
		if(sequence1 == sequence2)
		{
			return 0;
		}
	}
	return -2;
}

int ShmSpinWrite(struct shmt *shmo,void *data)
{
	struct shmlock *shml;
	shml = (struct shmlock *)shmo->shm;
	//共享内存被分配以后会初始化为0
	//此时共享内存中的shmlock结构还没有被正确初始化
	//shmlock结构直到最后一个进程释放共享内存前都有效
	//所以shmlock.statue在整个共享内存的生命周期内
	//只有一次从0到1的过程，变为1表明shmlock结构的
	//初始化完成。
	//所以这里直接先判断statue是安全的
	if(1 != shml->statue)
	{
		return -1;
	}
	//这是spinlock在用户空间的实现函数
	pthread_spin_lock(&(shml->writelock));
	atomic_inc(&(shml->sequence));
	memcpy(((char *)shmo->shm)+sizeof(struct shmlock),data,shmo->size);
	atomic_inc(&(shml->sequence));
	pthread_spin_unlock(&(shml->writelock));

	return 0;
}


int ShmWrite(struct shmt *shmo,void *data)
{
	struct shmlock *shml;
	shml = (struct shmlock *)shmo->shm;
	//共享内存被分配以后会初始化为0
	//此时共享内存中的shmlock结构还没有被正确初始化
	//shmlock结构直到最后一个进程释放共享内存前都有效
	//所以shmlock.statue在整个共享内存的生命周期内
	//只有一次从0到1的过程，变为1表明shmlock结构的
	//初始化完成。
	//所以这里直接先判断statue是安全的
	if(1 != shml->statue)
	{
		return -1;
	}
	atomic_inc(&(shml->sequence));
	memcpy(((char *)shmo->shm)+sizeof(struct shmlock),data,shmo->size);
	atomic_inc(&(shml->sequence));
	return 0;
}

