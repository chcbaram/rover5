#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

int main()
{
        int fbfd = 0;
        struct fb_var_screeninfo vinfo;
        struct fb_fix_screeninfo finfo;
        long int screensize = 0;
        char *fbp = 0;
        int i;
        
        fbfd = open("/dev/fb0", O_RDWR);
        if (!fbfd) {
                printf("Error: cannot open framebuffer device.\n");
                exit(1);
        }
        printf("The framebuffer device was opened successfully.\n");

        if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
                printf("Error reading fixed information.\n");
                exit(2);
        }

        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
                printf("Error reading variable information.\n");
                exit(3);
        }

        screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
        fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
        if ((int)fbp == -1) {
                printf("Error: failed to map framebuffer device to memory.\n");
                exit(4);
        }

        printf("vinfo.xres : %d\n", vinfo.xres);
        printf("vinfo.yres : %d\n", vinfo.yres);
        printf("vinfo.bits_per_pixel : %d\n", vinfo.bits_per_pixel);

        for( i=0; i<screensize; i++ )
        {
        	fbp[i] = 0x11;
        }

        printf("The framebuffer device was mapped to memory successfully.\n");

        munmap(fbp, screensize);
        close(fbfd);
        return 0;
}