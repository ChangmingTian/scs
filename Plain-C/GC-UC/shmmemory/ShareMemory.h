
/*[2016-04-19T10:18:30]*/
/*永远都不要为了目的而忘了初衷。*/
/*就像给风命名的,不是它要去的方向,而是它来时的方向。*/
/*Email:372909853@qq.com*/
/*review by tcm*/

#ifndef __SHAREMEMORY_H__
#define __SHAREMEMORY_H__
  
#include <sys/ipc.h>
#include <sys/shm.h>
//NOTICE!!!需要编译选项 -pthread,共享内存操作api使用的system V版本 

struct shmt
{
	void *shm;
	int shmid;
	int size;
};

/*B:创建一个共享内存结构
 *P0:filepath 一个文件路径,用于ftok生成共享内存ID
 *P1:magickey 一个整数值,用于ftok生成共享内存ID
 *P2:sizeofshm 要创建的共享内存区域的字节数,实际的共享内存字节数会大于该值
 *P3:shmo 指向一个struct shmt结构,存储所创建的共享内存信息
 *P4:shmflg IPC_CREATE 创建新的共享内存 0x00链接已经存在的共享内存
 *R:0 成功,其他失败
 */
int ShmCreat(char *filepath,int magickey,int sizeofshm,struct shmt *shmo,int shmflg);
/*B:如果需要支持写写互斥,需要在调用ShmCreat成功后调用该函数
 *初始化自旋锁,同时写入也要使用ShmSpinWrite函数.
 *P0:shmo 指向一个struct shmt结构,存储所创建的共享内存信息
 *R:0 成功,其他失败
 */
void ShmInitSpinLock(struct shmt *shmo);
/*B:解除共享内存到当前进程的映射
 *P0:shmo 指向一个struct shmt结构,存储所创建的共享内存信息
 *R:0 成功,其他失败
 */
int ShmUnmap(struct shmt *shmo);
/*B:解除共享内存到当前进程的映射,并且之后移除该共享内存
 *当所有对一个共享内存的引用都被释放以后,该共享内存由系统回收
 *P0:shmo 指向一个struct shmt结构,存储所创建的共享内存信息
 *R:0 成功,其他失败
 */
int ShmDestroy(struct shmt *shmo);
/*B:读取共享内存的数据
 *P0:shmo 指向一个struct shmt结构,存储所创建的共享内存信息
 *P1:data 指向存储读取结果的缓冲区
 *R:0 成功,其他失败
 */
int ShmRead(struct shmt *shmo,void *data);
/*B:向共享内存中写入数据,不支持自旋锁
 *P0:shmo 指向一个struct shmt结构,存储所创建的共享内存信息
 *P1:data 指向待写入的数据
 *R:0 成功,其他失败
 */
int ShmWrite(struct shmt *shmo,void *data);
/*B:向共享内存中写入数据,支持自旋锁
 *P0:shmo 指向一个struct shmt结构,存储所创建的共享内存信息
 *P1:data 指向待写入的数据
 *R:0 成功,其他失败
 */
int ShmSpinWrite(struct shmt *shmo,void *data);

//定义并初始化一个struct shmt结构
#define DEFINITSHMT(x) struct shmt x={NULL,-1,-1}
//初始化一个struct shmt结构
#define INITSHMT(x) x={NULL,-1,-1}

#endif


