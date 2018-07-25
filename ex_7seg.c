#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define  NODE_EXP                  "/dev/7seg_led"

#define  SEG_7_LED_ON              1
#define  SEG_7_LED_WRITE           2
#define  SEG_7_LED_CLEAR           3
#define  SEG_7_LED_OFF             4


/* Application : To test the functionaity of LCD Driver */

int main ()
{
    int exp_dev= 0;
    int res=0,i;

    /* open as blocking mode */
    exp_dev = open(NODE_EXP, O_RDWR);
    if (exp_dev < 0){
	fprintf(stderr, "Open error: %s\n", NODE_EXP);
	return 1;
    }
 
                if (res = ioctl(exp_dev,SEG_7_LED_ON, NULL) < 0 ){
                    printf("%d---> Error in switching OFF the LED \r\n",res);
                    close(exp_dev);
                    return 1;
                }

	   	if (res=ioctl(exp_dev,SEG_7_LED_WRITE, 0x1234) < 0 ){
		    printf("%d---> Error in writing to the 7-Seg LED \r\n",res);
		    close(exp_dev);
		    return 1;
		}
		sleep(5);
		if (res=ioctl(exp_dev,SEG_7_LED_WRITE, 1234) < 0 ){
		    printf("%d---> Error in writing to the 7-Seg LED \r\n",res);
		    close(exp_dev);
		    return 1;
		}
		printf ("Press any key to exit from the program\n");
		getchar();
                if (res=ioctl(exp_dev,SEG_7_LED_CLEAR, NULL) < 0 ){
                    printf("%d---> Error in Clearing the LED \r\n",res);
                    close(exp_dev);
                    return 1;
                }
                if (res=ioctl(exp_dev,SEG_7_LED_OFF, NULL) < 0 ){
                    printf("%d---> Error in switching off  the LED \r\n",res);
                    close(exp_dev);
                    return 1;
                }

    close(exp_dev);
    exit(1);
}
