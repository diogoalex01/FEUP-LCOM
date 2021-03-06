#include <lcom/lcf.h>

#include <stdint.h>
#include <minix/sysutil.h> 
#include <math.h>

#include "i8042.h"
#include "i8254.h"
#include "interface.h"
#include "cromoparty.h"

//VARIABLES
int res_x, res_y;
unsigned bits_pixel;

uint8_t blueMask, greenMask, redMask;
uint8_t blueMask, greenMask, redMask;

static unsigned char* video_mem;
unsigned char* double_buffer;

//FUNCTIONS
//////////////////////////////////////////////////////////////////

void *(vg_init)(uint16_t mode)
{
    lm_init(true);
    vbe_mode_info_t vmi_p;
    get_mode_info(mode, &vmi_p);

    bits_pixel = vmi_p.BitsPerPixel;
    res_y = vmi_p.YResolution;
    res_x = vmi_p.XResolution;
    int r;

    struct minix_mem_range mr; /* physical memory range */
    unsigned int vram_base = vmi_p.PhysBasePtr; /* VRAM’s physical addresss */
    unsigned int vram_size = res_y * res_x * ceil(bits_pixel / 8); /* VRAM’s size, but you can use the frame-buffer size, instead */

    redMask = vmi_p.RedMaskSize;
    blueMask = vmi_p.BlueMaskSize;
    greenMask = vmi_p.GreenMaskSize;
    /* frame-buffer VM address */
    /* Allow memory mapping */
    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if ( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  
    /* Map memory */
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if (video_mem == MAP_FAILED)
        panic("couldn’t map video memory");

    double_buffer = (unsigned char *) malloc(vram_size);

    if (double_buffer == NULL)
        panic("not enough memory");

    struct reg86u reg;
    memset(&reg, 0, sizeof(reg));

    reg.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
    reg.u.w.bx = (1 << 14) | mode; // set bit 14: linear framebuffer
    reg.u.b.intno = 0x10;
        
    if (sys_int86(&reg) != OK ) 
    {
        printf("set_vbe_mode: sys_int86() failed \n");
        return NULL;
    }

    return video_mem;
}

//////////////////////////////////////////////////////////////////

int get_mode_info(uint16_t mode, vbe_mode_info_t * vmi_p) 
{
    struct reg86u reg;
    memset(&reg, 0, sizeof(reg));
    mmap_t m;
    lm_init(true);

    if (lm_alloc(sizeof(vbe_mode_info_t), &m) == NULL)
    {
        return 1;
    }
  
    reg.u.w.ax = 0x4F01;
    reg.u.w.cx = mode;
    reg.u.b.intno = 0x10;
    reg.u.w.es = PB2BASE(m.phys);
    reg.u.w.di = PB2OFF(m.phys);

    if (sys_int86(&reg) != OK )
    {
        printf("set_vbe_mode: sys_int86() failed \n");
        lm_free(&m);
        return 1;
    }

    if (reg.u.b.ah != 0)
    {
        lm_free(&m);
        return 1;
    }

    *vmi_p = *((vbe_mode_info_t*) m.virt);
    lm_free(&m);

    return 0;
}

//////////////////////////////////////////////////////////////////

void double_buffer_to_video_mem() 
{
    memcpy(video_mem, double_buffer, res_y * res_x * ceil(bits_pixel / 8));
}

//////////////////////////////////////////////////////////////////

int get_horizontal_resolution()
{
	return res_x;
}

//////////////////////////////////////////////////////////////////

int get_vertical_resolution()
{
	return res_y;
}

//////////////////////////////////////////////////////////////////

Images images;

void loadImages()
{
    images.one_s = loadBitmap(appendString("/bitmap/numbers/1h.bmp"));
    images.two_s = loadBitmap(appendString("/bitmap/numbers/2h.bmp"));
    images.three_s = loadBitmap(appendString("/bitmap/numbers/3h.bmp"));
    images.four_s = loadBitmap(appendString("/bitmap/numbers/4h.bmp"));
    images.five_s = loadBitmap(appendString("/bitmap/numbers/5h.bmp"));
    images.six_s = loadBitmap(appendString("/bitmap/numbers/6h.bmp"));
    images.seven_s = loadBitmap(appendString("/bitmap/numbers/7h.bmp"));
    images.eight_s = loadBitmap(appendString("/bitmap/numbers/8h.bmp"));
    images.nine_s = loadBitmap(appendString("/bitmap/numbers/9h.bmp"));
    images.zero_s = loadBitmap(appendString("/bitmap/numbers/0h.bmp"));
    images.one = loadBitmap(appendString("/bitmap/numbers/1.bmp"));
    images.two = loadBitmap(appendString("/bitmap/numbers/2.bmp"));
    images.three = loadBitmap(appendString("/bitmap/numbers/3.bmp"));
    images.four = loadBitmap(appendString("/bitmap/numbers/4.bmp"));
    images.five = loadBitmap(appendString("/bitmap/numbers/5.bmp"));
    images.six = loadBitmap(appendString("/bitmap/numbers/6.bmp"));
    images.seven = loadBitmap(appendString("/bitmap/numbers/7.bmp"));
    images.eight = loadBitmap(appendString("/bitmap/numbers/8.bmp"));
    images.nine = loadBitmap(appendString("/bitmap/numbers/9.bmp"));
    images.zero = loadBitmap(appendString("/bitmap/numbers/0.bmp"));

    images.background = loadBitmap(appendString("/bitmap/sets/discof.bmp"));
    images.pad = loadBitmap(appendString("/bitmap/sprites/pad.bmp"));
    images.arrow_up = loadBitmap(appendString("/bitmap/sprites/arrowup.bmp"));
    images.arrow_left = loadBitmap(appendString("/bitmap/sprites/arrowleft.bmp"));
    images.arrow_down = loadBitmap(appendString("/bitmap/sprites/arrowdown.bmp"));
    images.arrow_right = loadBitmap(appendString("/bitmap/sprites/arrowright.bmp"));
    images.cromossomaY_idle = loadBitmap(appendString("/bitmap/sprites/cromossoma1.bmp"));
    images.cromossomaY_up = loadBitmap(appendString("/bitmap/sprites/cromossomaup.bmp"));
    images.cromossomaY_left = loadBitmap(appendString("/bitmap/sprites/cromossomaleft.bmp"));
    images.cromossomaY_down = loadBitmap(appendString("/bitmap/sprites/cromossomadown.bmp"));
    images.cromossomaY_right = loadBitmap(appendString("/bitmap/sprites/cromossomaright.bmp"));
    images.cromossomaX_idle = loadBitmap(appendString("/bitmap/sprites/cromossomax1.bmp"));
    images.cromossomaX_up = loadBitmap(appendString("/bitmap/sprites/cromossomaxup.bmp"));
    images.cromossomaX_left = loadBitmap(appendString("/bitmap/sprites/cromossomaxleft.bmp"));
    images.cromossomaX_down = loadBitmap(appendString("/bitmap/sprites/cromossomaxdown.bmp"));
    images.cromossomaX_right = loadBitmap(appendString("/bitmap/sprites/cromossomaxright.bmp"));
    images.perfect = loadBitmap(appendString("/bitmap/rating/perfect.bmp"));
    images.great = loadBitmap(appendString("/bitmap/rating/great.bmp"));
    images.okay = loadBitmap(appendString("/bitmap/rating/okay.bmp"));
    images.miss = loadBitmap(appendString("/bitmap/rating/miss.bmp"));
    images.power = loadBitmap(appendString("/bitmap/sprites/power.bmp"));
    images.pointer = loadBitmap(appendString("/bitmap/sprites/pointer.bmp"));

    images.singleplayer_selected = loadBitmap(appendString("/bitmap/buttons/start2.bmp"));
    images.multiplayer_selected = loadBitmap(appendString("/bitmap/buttons/multiplayer2.bmp"));
    images.instructions_selected = loadBitmap(appendString("/bitmap/buttons/instructions2.bmp"));
    images.highscores_selected = loadBitmap(appendString("/bitmap/buttons/highscores2.bmp"));
    images.exit_selected = loadBitmap(appendString("/bitmap/buttons/exit2.bmp"));
    images.singleplayer_not_selected = loadBitmap(appendString("/bitmap/buttons/start1.bmp"));
    images.multiplayer_not_selected = loadBitmap(appendString("/bitmap/buttons/multiplayer1.bmp"));
    images.instructions_not_selected = loadBitmap(appendString("/bitmap/buttons/instructions1.bmp"));
    images.highscores_not_selected = loadBitmap(appendString("/bitmap/buttons/highscores1.bmp"));
    images.exit_not_selected = loadBitmap(appendString("/bitmap/buttons/exit1.bmp"));
    images.menu = loadBitmap(appendString("/bitmap/sets/menu.bmp"));
    images.instructions = loadBitmap(appendString("/bitmap/sets/instructionmenu.bmp"));
    images.highscores = loadBitmap(appendString("/bitmap/sets/highscorepanel.bmp"));
    images.pause = loadBitmap(appendString("/bitmap/sets/pause.bmp"));
    images.end = loadBitmap(appendString("/bitmap/sets/end.bmp"));
    images.extra = loadBitmap(appendString("/bitmap/rating/extra.bmp"));
    images.name_menu = loadBitmap(appendString("/bitmap/sets/namemenu.bmp"));
    //images.name_menu = loadBitmap(appendString("/bitmap/sets/namemenu.bmp"))); example
    images.waiting = loadBitmap(appendString("/bitmap/sets/waiting.bmp"));

    images.a = loadBitmap(appendString("/bitmap/alphabet/a.bmp"));
    images.b = loadBitmap(appendString("/bitmap/alphabet/b.bmp"));
    images.c = loadBitmap(appendString("/bitmap/alphabet/c.bmp"));
    images.d = loadBitmap(appendString("/bitmap/alphabet/d.bmp"));
    images.e = loadBitmap(appendString("/bitmap/alphabet/e.bmp"));
    images.f = loadBitmap(appendString("/bitmap/alphabet/f.bmp"));
    images.g = loadBitmap(appendString("/bitmap/alphabet/g.bmp"));
    images.h = loadBitmap(appendString("/bitmap/alphabet/h.bmp"));
    images.i = loadBitmap(appendString("/bitmap/alphabet/i.bmp"));
    images.j = loadBitmap(appendString("/bitmap/alphabet/j.bmp"));
    images.k = loadBitmap(appendString("/bitmap/alphabet/k.bmp"));
    images.l = loadBitmap(appendString("/bitmap/alphabet/l.bmp"));
    images.m = loadBitmap(appendString("/bitmap/alphabet/m.bmp"));
    images.n = loadBitmap(appendString("/bitmap/alphabet/n.bmp"));
    images.o = loadBitmap(appendString("/bitmap/alphabet/o.bmp"));
    images.p = loadBitmap(appendString("/bitmap/alphabet/p.bmp"));
    images.q = loadBitmap(appendString("/bitmap/alphabet/q.bmp"));
    images.r = loadBitmap(appendString("/bitmap/alphabet/r.bmp"));
    images.s = loadBitmap(appendString("/bitmap/alphabet/s.bmp"));
    images.t = loadBitmap(appendString("/bitmap/alphabet/t.bmp"));
    images.u = loadBitmap(appendString("/bitmap/alphabet/u.bmp"));
    images.v = loadBitmap(appendString("/bitmap/alphabet/v.bmp"));
    images.w = loadBitmap(appendString("/bitmap/alphabet/w.bmp"));
    images.x = loadBitmap(appendString("/bitmap/alphabet/x.bmp"));
    images.y = loadBitmap(appendString("/bitmap/alphabet/y.bmp"));
    images.z = loadBitmap(appendString("/bitmap/alphabet/z.bmp"));
    images.exc = loadBitmap(appendString("/bitmap/alphabet/exc.bmp"));
    images.inter = loadBitmap(appendString("/bitmap/alphabet/int.bmp"));
    images.slash = loadBitmap(appendString("/bitmap/alphabet/slash.bmp"));
    images.colon = loadBitmap(appendString("/bitmap/alphabet/colon.bmp"));

    images.cromossomaY = loadBitmap(appendString("/bitmap/sets/character1.bmp"));
    images.cromossomaX = loadBitmap(appendString("/bitmap/sets/character2.bmp"));
    images.scoreY = loadBitmap(appendString("/bitmap/sprites/yscore.bmp"));
    images.scoreX = loadBitmap(appendString("/bitmap/sprites/xscore.bmp"));
}

//////////////////////////////////////////////////////////////////

Images images;

void deleteImages()
{
    deleteBitmap(images.one_s);
    deleteBitmap(images.two_s);
    deleteBitmap(images.three_s);
    deleteBitmap(images.four_s);
    deleteBitmap(images.five_s);
    deleteBitmap(images.six_s);
    deleteBitmap(images.seven_s);
    deleteBitmap(images.eight_s);
    deleteBitmap(images.nine_s);
    deleteBitmap(images.zero_s);
    deleteBitmap(images.one);
    deleteBitmap(images.two);
    deleteBitmap(images.three);
    deleteBitmap(images.four);
    deleteBitmap(images.five);
    deleteBitmap(images.six);
    deleteBitmap(images.seven);
    deleteBitmap(images.eight);
    deleteBitmap(images.nine);
    deleteBitmap(images.zero);
    deleteBitmap(images.background);
    deleteBitmap(images.pad);
    deleteBitmap(images.arrow_up);
    deleteBitmap(images.arrow_left);
    deleteBitmap(images.arrow_down);
    deleteBitmap(images.arrow_right);
    deleteBitmap(images.cromossomaY_idle);
    deleteBitmap(images.cromossomaY_up);
    deleteBitmap(images.cromossomaY_left);
    deleteBitmap(images.cromossomaY_down);
    deleteBitmap(images.cromossomaY_right);
    deleteBitmap(images.cromossomaX_idle);
    deleteBitmap(images.cromossomaX_up);
    deleteBitmap(images.cromossomaX_left);
    deleteBitmap(images.cromossomaX_down);
    deleteBitmap(images.cromossomaX_right);
    deleteBitmap(images.perfect);
    deleteBitmap(images.great);
    deleteBitmap(images.okay);
    deleteBitmap(images.miss);
    deleteBitmap(images.power);
    deleteBitmap(images.pointer);  
    deleteBitmap(images.singleplayer_selected);
    deleteBitmap(images.multiplayer_selected);
    deleteBitmap(images.instructions_selected);
    deleteBitmap(images.highscores_selected);
    deleteBitmap(images.exit_selected);
    deleteBitmap(images.singleplayer_not_selected);
    deleteBitmap(images.multiplayer_not_selected);
    deleteBitmap(images.instructions_not_selected);
    deleteBitmap(images.highscores_not_selected);
    deleteBitmap(images.exit_not_selected);
    deleteBitmap(images.menu);
    deleteBitmap(images.instructions);
    deleteBitmap(images.highscores);
    deleteBitmap(images.pause);
    deleteBitmap(images.end);
    deleteBitmap(images.extra);
    deleteBitmap(images.name_menu);
    deleteBitmap(images.waiting);
    deleteBitmap(images.a);
    deleteBitmap(images.b);
    deleteBitmap(images.c);
    deleteBitmap(images.d);  
    deleteBitmap(images.e);
    deleteBitmap(images.f);
    deleteBitmap(images.g);
    deleteBitmap(images.h);
    deleteBitmap(images.i);
    deleteBitmap(images.j);
    deleteBitmap(images.k);
    deleteBitmap(images.l);
    deleteBitmap(images.m);
    deleteBitmap(images.n);
    deleteBitmap(images.o);
    deleteBitmap(images.p);
    deleteBitmap(images.q);
    deleteBitmap(images.r);
    deleteBitmap(images.s);
    deleteBitmap(images.t);
    deleteBitmap(images.u);
    deleteBitmap(images.v);
    deleteBitmap(images.w);
    deleteBitmap(images.x);
    deleteBitmap(images.y);
    deleteBitmap(images.z);
    deleteBitmap(images.exc);
    deleteBitmap(images.inter);
    deleteBitmap(images.slash);
    deleteBitmap(images.colon);
    deleteBitmap(images.cromossomaY);
    deleteBitmap(images.cromossomaX);
    deleteBitmap(images.scoreY);
    deleteBitmap(images.scoreX);
}

//////////////////////////////////////////////////////////////////

Bitmap* loadBitmap(const char* filename) 
{
    // allocating necessary size
    Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

    // open filename in read binary mode
    FILE *filePtr;
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return NULL;

    // read the bitmap file header
    BitmapFileHeader bitmapFileHeader;
    fread(&bitmapFileHeader, 2, 1, filePtr);

    // verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.type != 0x4D42) 
    {
        fclose(filePtr);
        return NULL;
    }

    int rd;
    do {
        if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
            break;
    } while (0);

    if (rd != 1) 
    {
        fprintf(stderr, "Error reading file\n");
        exit(-1);
    }

    // read the bitmap info header
    BitmapInfoHeader bitmapInfoHeader;
    fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

    // move file pointer to the begining of bitmap data
    fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

    // allocate enough memory for the bitmap image data
    unsigned char* bitmapImage = (unsigned char*) malloc(bitmapInfoHeader.imageSize);

    // verify memory allocation
    if (!bitmapImage) 
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) 
    {
        fclose(filePtr);
        return NULL;
    }

    // close file and return bitmap image data
    fclose(filePtr);

    bmp->bitmapData = bitmapImage;
    bmp->bitmapInfoHeader = bitmapInfoHeader;
    bmp->colided = false;
    return bmp;
}

//////////////////////////////////////////////////////////////////

void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment) 
{
    if (bmp == NULL)
        return;

    int width = bmp->bitmapInfoHeader.width;
    int drawWidth = width;
    int height = bmp->bitmapInfoHeader.height;

    if (alignment == ALIGN_CENTER)
        x -= width / 2;
    else if (alignment == ALIGN_RIGHT)
        x -= width;

    if (x + width < 0 || x > get_horizontal_resolution() || y + height < 0 || y > get_vertical_resolution())
        return;

    int xCorrection = 0;
    if (x < 0) 
    {
        xCorrection = -x;
        drawWidth -= xCorrection;
        x = 0;

        if (drawWidth > get_horizontal_resolution())
            drawWidth = get_horizontal_resolution();
    } 
    else if (x + drawWidth >= get_horizontal_resolution()) 
    {
        drawWidth = get_horizontal_resolution() - x;
    }

    unsigned char* bufferStartPos;
    unsigned char* imgStartPos;

    int i;
    for (i = 0; i < height; i++) 
    {
        int pos = y + height - 1 - i;

        if (pos < 0 || pos >= get_vertical_resolution())
            continue;

        bufferStartPos = double_buffer;

        bufferStartPos += x * 4 + pos * get_horizontal_resolution() * 4;

        imgStartPos = (unsigned char*)(bmp->bitmapData) + xCorrection * 4 + i * width * 4;

        uint32_t *ptr =(uint32_t*) imgStartPos;
        uint32_t *buff =(uint32_t*) bufferStartPos;

        for(int j = 0; j < drawWidth; j ++)
        {

            if(ptr[j] != 0x1f0ff8)
            {
                if(buff[j] == 0x630000 || buff[j] == 0xed5c22)
                {
                    
                    bmp->colided = true;
                }  

                buff[j] = ptr[j];
            } 
        }
    }
}

//////////////////////////////////////////////////////////////////

void deleteBitmap(Bitmap* bmp)
{
    if (bmp == NULL)
        return;

    free(bmp->bitmapData);
    free(bmp);
}

//////////////////////////////////////////////////////////////////

char temp[150];

char* appendString(char* s)
{
    strcpy(temp, path);
    strcat(temp, s);

    return temp;
}
