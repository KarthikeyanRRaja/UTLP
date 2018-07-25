
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define  NODE_EXP	"/dev/fpga_char_lcd"

#define  LCD_ON              1
#define  LCD_CLEAR           2
#define  LCD_SET_HOME        3
#define  LCD_CURSOR_BLINK    4
#define  LCD_DATA_WRITE      5
#define  LCD_OFF	     6

/* Application : To test the functionaity of LCD Driver */

int main ()
{
    int exp_dev=0;

    /* open as blocking mode*/
    exp_dev = open(NODE_EXP, O_RDWR);
    if (exp_dev < 0)
    {
        fprintf(stderr, "Open error: %s\n", NODE_EXP);
	return 1;
    }


	   	if (ioctl(exp_dev,LCD_ON, NULL) < 0)
		{
		    printf("Error in Writing the LCD \r\n");
		    close(exp_dev);
		    return 1;
	        }
		if (ioctl(exp_dev,LCD_CLEAR, NULL) < 0)
		{
	   	    printf("Error in Clearing the LCD \r\n");
	     	    close(exp_dev);
	    	    return 1;
		}

                    if (ioctl(exp_dev,LCD_SET_HOME, NULL) < 0 )
                    {
                        printf("Error in setting the cursor in home\
			position \r\n");
                        close(exp_dev);
                        return 1;
                     }

                if (ioctl(exp_dev,LCD_CURSOR_BLINK, NULL) < 0 )
                {
                    printf("Error in Curosr blinking \r\n");
                    close(exp_dev);
                    return 1;
                }
                if (ioctl(exp_dev,LCD_DATA_WRITE, "UTLP DAY 2") < 0 )
                {
                    printf("Error in writing the LCD \r\n");
                    sleep(2);
                    close(exp_dev);
                    return 1;
                }
               

		printf ("Press any key to exit from the program\n");
		getchar();
                if (ioctl(exp_dev,LCD_OFF, NULL) < 0 )
                {
                    printf("Error in Switching OFF the LCD \r\n");
                    close(exp_dev);
                    return 1;
                }
	    
        close(exp_dev);
	
    
    exit(1);
}
