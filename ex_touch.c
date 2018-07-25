#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>


int main()
{

	int fd;
	char buff[256] = "Name";
	int i,rb;
        struct input_event ev;
        struct input_absinfo ab;

	if ((fd = open("/dev/input/touchscreen0", O_RDONLY)) < 0) 
	{
		printf ("Error Opening the Device\n");
                return 1;
    }

	ioctl(fd, EVIOCGNAME(sizeof(buff)), buff);
	printf("Touch Input device name: \"%s\"\n", buff);

        for (i = 0; i < 5; i++)
	{
	
        	rb=read(fd,&ev,sizeof(struct input_event));

		printf ("After read\n");
		if ((ev.type == 1) && (ev.code == 330))
		{
			printf ("Touch Event = %d\n",ev.code);
		}

		if ((ev.type == 3) && (ev.code == 0))
		{
			printf ("X Value  = %d\n",ev.value);
		}
		if ((ev.type == 3) && (ev.code == 1))
		{
			printf ("Y Value  = %d\n",ev.value);
		}
		if ((ev.type == 3) && (ev.code == 24))
		{
			printf ("Touch Pressure  = %d\n",ev.value);
		}
			
	}
	return 0;
}
