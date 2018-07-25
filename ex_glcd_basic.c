#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/dir.h>
#include <linux/fb.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;
int fbfd = 0;
long int screensize;
int pid;

void ex_program(int sig)
{

	munmap(fbp, screensize);
	close(fbfd);
	system("pkill -9 -f X");
	exit(0);
}

int main ()
{


	int i,x,y;
	unsigned long int location = 0;
	pid=getpid();
	// Open the file for reading and writing
	fbfd = open("/dev/fb0", O_RDWR);
	if (!fbfd) {
		printf("Error cannot open framebuffer device.\n");
		exit(1);
	}

	// Get fixed screen information
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
		printf("Error reading fixed information.\n");
		exit(2);
	}

	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf("Error reading variable information.\n");
		exit(3);
	}
	printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );

	screensize = 0;

	// Figure out the size of the screen in bytes
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	// Map the device to memory
	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,fbfd, 0);

	if ((int)fbp == -1) {
		printf("Error failed to map framebuffer device to memory.\n");
		exit(4);
	}

	(void) signal(SIGINT, ex_program);
	y = 239;
	while(y > 159) {
	for(x=0; x < 320 ; x++) {
		location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +(y+vinfo.yoffset) * finfo.line_length;
		*((unsigned long *)(fbp + location)) = 0x880000 ;
					}
		y--;
				}
	y = 159;
	while(y > 79) {
	for(x=0; x < 320 ; x++) {
		location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +(y+vinfo.yoffset) * finfo.line_length;
		*((unsigned long *)(fbp + location)) = 0x008800 ;
					}
		y--;
				}

	y = 79;
	while(y > 0) {
	for(x=0; x < 320 ; x++) {
		location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +(y+vinfo.yoffset) * finfo.line_length;
		*((unsigned long *)(fbp + location)) = 0x000088 ;
					}
		y--;
				}

	munmap(fbp, screensize);
	close(fbfd);
	return 0;
}
