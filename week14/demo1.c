#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

int g_Flag=0;
void* thread1(*void)
void* thread2(*void)

int main(int argc, char const *argv[])
{
	printf("enter main\n");
	pthread_t tid1,tid2;
	int rc1=0,rc2=0;
	rc2 = pthread_create(&tid2, NULL, thread2, NULL);
	if (rc2 != 0)
	{
		printf("%s: %d\n",__func__,strerror(rc2) );
		rc1 = pthread_create(&tid1,NULL, thread1, NULL);
		if (rc1 != 0)
		{
			printf("%s: %d\n", __func__, strerror(rc1) );
		}
		printf("leave main\n");
		exit(0);
	}

	void* thread1(void* arg)
	{
		printf("enter thread1\n");
		printf("this is thread1,g_Flag : %d,thread is %u\n",g_Flag,(unsigned int)pthrad_self() );
		g_Flag = 1;
		printf("this is thread1,g_Flag : %d,thread is %u\n",g_Flag,(unsigned int)pthrad_self() );
		printf("leave thread1\n");
		pthread_exit(0);
	}

	void* thread2(void* arg)
	{
		printf("enter thread2\n");
		printf("this is thread2,g_Flag : %d,thread is %u\n",g_Flag,(unsigned int)pthrad_self() );		
		g_Flag = 2;
		printf("this is thread2,g_Flag : %d,thread is %u\n",g_Flag,(unsigned int)pthrad_self() );
		printf("leave thread2\n");
		pthread_exit(0);
	}
}
