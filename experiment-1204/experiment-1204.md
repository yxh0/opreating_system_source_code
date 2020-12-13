# Linux 多线程编程——读者优先、写着优先问题



## **读者优先描述**

### 如果读者来

1. 无读者、写者，新读者可以读;
2. 无写者等待，但有其他读者正在读，新读者可以读;
3. 有写者等待，但有其他读者正在读，新读者可以读;
4. 有写者写，新读者等

### 如果写者先来

1. 无读者，新写者可以写；
2. 有读者，新写者等待；
3. 有其他写者写或等待，新写者等待

## **写者优先描述**

### 如果读者来

1. 无读者、写者，新读者可以读;

2. 无写者等待，但有其他读者正在读，新读者可以读;

3. 有写者等待，但有其他读者正在读，新读者等;

4. 有写者写，新读者等

### 如果写着来

1. 无读者，新写者可以写;

2. 有读者，新写者等待;

3. 有其他写者或等待，新写者等待



## 信号量和互斥锁的区别

- 互斥量用于线程的互斥，信号量用于线程的同步
   - 这是互斥量和信号量的根本区别，也就是互斥和同步之间的区别。 
      互斥：是指某一资源同时只允许一个访问者对其进行访问，具有唯一性和排它性。但互斥无法限制访问者对资源的访问顺序，即访问是无序的。 
      同步：是指在互斥的基础上（大多数情况），通过其它机制实现访问者对资源的有序访问。在大多数情况下，同步已经实现了互斥，特别是所有写入资源的情况必定是互斥的。少数情况是指可以允许多个访问者同时访问资源 
- 互斥量值只能为0/1，信号量值可以为非负整数
  - 也就是说，一个互斥量只能用于一个资源的互斥访问，它不能实现多个资源的多线程互斥问题。信号量可以实现多个同类资源的多线程互斥和同步。当信号量为单值信号量是，也可以完成一个资源的互斥访问。
- 互斥量的加锁和解锁必须由同一线程分别对应使用，信号量可以由一个线程释放，另一个线程得到

### 读者优先

使用互斥锁来确保同一时间只能一个进程写文件，实现互斥。使用信号量来实现访问资源的同步。

首先，写者的代码应该是这样一种形式,才能保证同一时刻只有一个写者修改数据。

考虑到写者对读者的影响是:当任何读者想读时，写者都必须被阻塞；并且，读者阻塞了写者并停止阻塞之前，后续的任何写者都会读者优先于执行。这就如同有一个读者队列，当第一个读者入队时，写者完全被阻塞，直到最后一个读者离开队列。

据此，可以用`readerCnt`来统计读者的数量，而用信号量`sem_read`来互斥各线程对`readerCnt`的访问。



##  Code Reference 1:

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<semaphore.h>


#define N_WRITER 30 //写者数目
#define N_READER 5 //读者数目
#define W_SLEEP  1 //控制写频率
#define R_SLEEP  1 //控制读频率


pthread_t wid[N_WRITER],rid[N_READER];
pthread_mutex_t mutex_write;//同一时间只能一个人写文件,互斥
sem_t sem_read;//同一时间只能有一个人访问 readerCnt
int data = 0;
int readerCnt = 0;
void write()
{
    int rd = rand();
    printf("write %d\n",rd);
    data = rd;
}
void read()
{
    printf("read %d\n",data);
}
void * writer(void * in)
{
//    while(1)
//    {
        pthread_mutex_lock(&mutex_write);
        printf("写线程id%d进入数据集\n",pthread_self());
        write();
        printf("写线程id%d退出数据集\n",pthread_self());
        pthread_mutex_unlock(&mutex_write);
        sleep(W_SLEEP);
//    }
    pthread_exit((void *) 0);
}

void * reader (void * in)
{
//    while(1)
//    {
        sem_wait(&sem_read);
        readerCnt++;
        if(readerCnt == 1){
            pthread_mutex_lock(&mutex_write);
        }
        sem_post(&sem_read);
        printf("读线程id%d进入数据集\n",pthread_self());
        read();
        printf("读线程id%d退出数据集\n",pthread_self());    
        sem_wait(&sem_read);
        readerCnt--;
        if(readerCnt == 0){
            pthread_mutex_unlock(&mutex_write);
        }
        sem_post(&sem_read);
        sleep(R_SLEEP);
//    }
    pthread_exit((void *) 0);
}

int main()
{
    printf("多线程,读者优先\n");    
    pthread_mutex_init(&mutex_write,NULL);
    sem_init(&sem_read,0,1);
    int i = 0;
    for(i = 0; i < N_WRITER; i++)
    {
        pthread_create(&wid[i],NULL,writer,NULL);
    }
        for(i = 0; i < N_READER; i++)
    {
        pthread_create(&rid[i],NULL,reader,NULL);
    }
    sleep(1);
    return 0;
}

```

- 读者优先

为了更明显的看到效果，在main函数中创建了20个写者和5个读者。注意编译时要加上`-lpthread`指定库。

## Code Reference 2:

```c
#include "stdio.h"
#include <stdlib.h>
#include <pthread.h>
#include<semaphore.h>

 

#define N_WRITER 5 //写者数目
#define N_READER 20 //读者数目
#define W_SLEEP 1 //控制写频率
#define R_SLEEP  0.5 //控制读频率

 


pthread_t wid[N_WRITER],rid[N_READER];
int data = 0;
int readerCnt = 0, writerCnt = 0;
pthread_mutex_t sem_read;
pthread_mutex_t sem_write;
pthread_mutex_t mutex_write;
pthread_mutex_t mutex_read;

 

void write()
{
    int rd = rand();
    printf("write %d\n",rd);
    data = rd;
}
void read()
{
    printf("read %d\n",data);
}
void * writer(void * in)
{
//    while(1)
//    {
        sem_wait(&sem_write);
        {//临界区,希望修改 writerCnt,独占 writerCnt
            writerCnt++;
            if(writerCnt == 1){
                //阻止后续的读者加入待读队列
                pthread_mutex_lock(&mutex_read);
            }
        }
        sem_post(&sem_write);
        
        
        pthread_mutex_lock(&mutex_write);
        {//临界区，限制只有一个写者修改数据
            printf("写线程id%d进入数据集\n",pthread_self());
            write();
            printf("写线程id%d退出数据集\n",pthread_self());        
        }
        pthread_mutex_unlock(&mutex_write);
        
        sem_wait(&sem_write);
        {//临界区,希望修改 writerCnt,独占 writerCnt
            writerCnt--;
            if(writerCnt == 0){
                //阻止后续的读者加入待读队列
                pthread_mutex_unlock(&mutex_read);
            }
        }
        sem_post(&sem_write);
        sleep(W_SLEEP);
//    }
    pthread_exit((void *) 0);
}

 

void * reader (void * in)
{
//    while(1)
//    {
        //假如写者锁定了mutex_read,那么成千上万的读者被锁在这里
            pthread_mutex_lock(&mutex_read);//只被一个读者占有
            {//临界区
                sem_wait(&sem_read);//代码段 1
                {//临界区
                    readerCnt++;
                    if(readerCnt == 1){
                        pthread_mutex_lock(&mutex_write);
                    }
                }
                sem_post(&sem_read);
            }
            pthread_mutex_unlock(&mutex_read);//释放时,写者将优先获得mutex_read
        printf("读线程id%d进入数据集\n",pthread_self());
        read();
        printf("读线程id%d退出数据集\n",pthread_self());
        sem_wait(&sem_read);//代码段2
        {//临界区
            readerCnt--;
            if(readerCnt == 0){
                pthread_mutex_unlock(&mutex_write);//在最后一个并发读者读完这里开始禁止写者执行写操作
            }
        }
        sem_post(&sem_read);
        
        sleep(R_SLEEP);
//    }
    pthread_exit((void *) 0);
}

 

int main()
{
    printf("多线程,写者优先\n");
    pthread_mutex_init(&mutex_write,NULL);
    pthread_mutex_init(&mutex_read,NULL);
    sem_init(&sem_write,0,1);
    sem_init(&sem_read,0,1);
int i = 0;
    for(i = 0; i < N_READER; i++)
    {
        pthread_create(&rid[i],NULL,reader,NULL);
    }
    for(i = 0; i < N_WRITER; i++)
    {
        pthread_create(&wid[i],NULL,writer,NULL);
    }
    sleep(1);    
    return 0;
}
```

