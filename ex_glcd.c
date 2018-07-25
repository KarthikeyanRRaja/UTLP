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

unsigned short Type;
unsigned long Size; /* file size in bytes */

struct FileHeader {
	unsigned short Reserved1; /* 0 */
	unsigned short Reserved2; /* 0 */
	unsigned long OffBits; /* offset to bitmap */
	unsigned long StructSize; /* size of this struct (40) */
	unsigned long Width; /* bmap width in pixels */
	unsigned long Height; /* bmap height in pixels */
	unsigned short Planes; /* num planes - always 1 */
	unsigned short BitCount; /* bits per pixel */
	unsigned long Compression; /* compression flag */
	unsigned long SizeImage; /* image size in bytes */
	long XPelsPerMeter; /* horz resolution */
	long YPelsPerMeter; /* vert resolution */
	unsigned long ClrUsed; /* 0 -> color table size */
	unsigned long ClrImportant; /* important color count */
};

struct RGBQUAD {
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
};

extern  int alphasort();

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;
int fbfd = 0;
long int screensize;
char *fname[100];
int pid;

void ex_program(int sig)
{

	munmap(fbp, screensize);
	close(fbfd);
	system("pkill -9 -f X");
	exit(0);
}


/******************************************************************************
*
******************************************************************************/
int show_bmp(int count,int delay)
{
	FILE *fp;
	unsigned long int location = 0, BytesPerLine = 0;
	unsigned long pixel, p1;
	struct FileHeader *Header;
	unsigned int t,x,y;
	unsigned long size, bytes_read;
	unsigned char Bmp, dummy, red, blue, green;
	int i,hindex,index,j;
	struct RGBQUAD Palette[256];
	unsigned long  *bgr_buff;
	char buff[50];

	do {
		j=0;
		(void) signal(SIGINT, ex_program);
		while(j < count) {
			fp = fopen(fname[j],"rb");
			Header = (struct FileHeader *) malloc (sizeof(struct FileHeader));
			if (!Header) {
				perror("Error:");

				exit(1);
			}
			if (!fp) {
				printf ("Error opening source file\r\n");
				perror ("Error");
				exit (1);
			}
			printf("\nfilename: %s\n\n",fname[j]);
			fread(&Type, sizeof(Type), 1, fp);
			bytes_read = sizeof(Type);
			fread(&Size, sizeof(Size), 1, fp);
			bytes_read += sizeof(Size);

			if ((fread(Header, sizeof(struct FileHeader), 1, fp)) == -1) {
				printf ("Error: Unable to read File header.\r\n");
				exit (1);
			}

			bytes_read += sizeof(struct FileHeader);
			while (bytes_read < Header->OffBits) {
				if (fread(&dummy,sizeof(dummy),1,fp)!=1) {
					printf("Error seeking to bitmap data\n");
					exit(-1);
				}
				++bytes_read;
			}

			size = Header->Width * Header->Height;
			
			printf ("BMP Width = %d\tBMP Height = %d\n", Header->Width, Header->Height);

			printf ("Bit Count = %d\n", Header->BitCount);
			index=0;
			if (Header->BitCount == 24) {
				bgr_buff = (unsigned long *) malloc (size * sizeof(unsigned long));
				for (i = 0; i < size; i++) {
					blue = fgetc(fp);
					green = fgetc(fp);
					red  = fgetc(fp);
				
					p1 = 0;
					p1 |= red;
					p1 = p1 << 16;
					pixel = p1;
					p1 = 0;
					p1 |= green;
					p1 = p1 << 8;
					pixel |= p1;
					p1 = 0;
					p1 |= blue;
					pixel |= p1;

					bgr_buff [index] = pixel;
					index ++;
				}
				
				// At this point bgr_buff contains the RGB values for the pixels defined by height & width of the BMP file.


				hindex=0;
				y = Header->Height-1;
				while(y > 0) {
					for(x=0; x < Header->Width ; x++) {

						location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +(y+vinfo.yoffset) * finfo.line_length;

						*((unsigned long *)(fbp + location)) = bgr_buff [hindex] ;

						hindex++;
					}
					--y;hindex+=240;
				}
				free(bgr_buff);
			}
			free(Header);
			printf ("\n[Done]\n\r");
			j++;
			fclose (fp);
			sleep(delay);
		}
	} while(1);

	return( 0 );
}


/******************************************************************************
*
******************************************************************************/
int file_select(const struct direct   *entry)

{
	char *ptr;
	if ((strcmp(entry->d_name, ".")== 0) ||
	    (strcmp(entry->d_name, "..") == 0))
		return (0);

	/* Check for filename extensions */
	ptr = rindex(entry->d_name, '.');
	if ((ptr != NULL) &&((strcmp(ptr, ".bmp")==0)))
		return (1);
	else
		return(0);
}
/******************************************************************************
*
******************************************************************************/
int main ( int argc, char *argv[] )
{


	unsigned long size, bytes_read;
	struct direct **files;
	char *pathname;
	int count=0,delay =0,i;
	pid=getpid();

	if (argc <= 2) {
		printf ("Usage:./ex_glcd <source bitmap directory> <delay>\r\n");
		exit (2);
	}

	pathname=argv[1];
	delay=atoi(argv[2]);
	size =pathconf(pathname, _PC_PATH_MAX);
	if (!(getcwd(pathname,(size_t)size))) {
		printf("Error getting path\n");
		return 0;
	}

	if(!(chdir(pathname)))
		count = scandir(pathname, &files, file_select, alphasort);

	if (count <= 0) {
		printf("No files in this directory\n");
		return 0;
	}

	printf("Number of files = %d\n",count);

	printf("*****Files are*****\n");
	for (i=1; i<count+1; ++i) {

		printf("%s\n",files[i-1]->d_name);
		fname[i-1]=files[i-1]->d_name;
	}
	printf("\n");

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

	printf("into show_bmp \n");

	show_bmp(count,delay);

	munmap(fbp, screensize);
	close(fbfd);
	return 0;
}
