/*----------------------------------------------------------------------------*|
|*                                                                            *|
|* FC ģ���� (Famicom��Nintendo��˾��1983��7��15�����ձ����۵�8λ��Ϸ��)      *|
|*                                                                            *|
|* $ C++���Եĵ�һ����Ŀ,���������ְ�                                         *|
|* $ è��д��, �����ñ����������ע������                                     *|
|* $ ���߶�ʹ�ñ�������ɵĺ�������κ�����                                   *|
|* $ �಻��Դ���Ĺ���ԭ������һ������,�����ش������벦��119                 *|
|*                                                                            *|
|* > ʹ�� [Code::Block 10.05] ��������                                        *|
|* > ������ʹ�� [MinGW 3.81] [gcc 4.4.1]                                      *|
|* > �ο������� [http://nesdev.parodius.com] ��վ������                       *|
|* > ��л [Flubba,blargg] ��ƵĲ��Գ���, ����������Ч�ʳ�ָ������            *|
|*                                                                            *|
|* ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ | CatfoOD |^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ *|
|*                                           | yanming-sohu@sohu.com          *|
|* [ TXT CHARSET WINDOWS-936 / GBK ]         | https://github.com/yanmingsohu *|
|*                                           | qQ:412475540                   *|
|*----------------------------------------------------------------------------*/
#ifndef PPU_H_INCLUDED
#define PPU_H_INCLUDED

#include "mmc.h"
#include "video.h"

#define PPU_DISPLAY_P_WIDTH      256
#define PPU_DISPLAY_P_HEIGHT     240

#define PPU_DISPLAY_N_WIDTH      256
#define PPU_DISPLAY_N_HEIGHT     240

#define PPU_VMIRROR_VERTICAL     0x01
#define PPU_VMIRROR_HORIZONTAL   0x00
#define PPU_VMIRROR_SINGLE       0x03
#define PPU_VMIRROR_4_LAYOUT     0x08

#define TO_CPU_CC(x)             (x/12.0)
/*---------------------------------------| PAL Info |-------------*/
#define P_VLINE_COUNT                312   /* ÿ֡ɨ����          */
#define P_HLINE_CPU_CYC    TO_CPU_CC(1024) /* ÿ�л�������        */
#define P_HBLANK_CPU_CYC   TO_CPU_CC(338 ) /* ÿ��ˮƽ��������    */
#define P_VBLANK_CPU_CYC   TO_CPU_CC(683 ) /* ��ֱ��������        */
                                           /* ÿ֡����            */
#define P_FRAME_CPU_CYC       \
            ( (P_HLINE_CPU_CYC + P_HBLANK_CPU_CYC) * P_VLINE_COUNT )
                                           /* ÿ��������          */
#define P_PIXEL_CPU_CYC       \
            ( P_HLINE_CPU_CYC / PPU_DISPLAY_P_WIDTH )
#define P_END_CYC             2
/*---------------------------------------| NTSC Info |------------*/
#define N_VLINE_COUNT                262   /* ÿ֡ɨ����          */
#define N_HLINE_CPU_CYC    TO_CPU_CC(1024) /* ÿ�л�������        */
#define N_HBLANK_CPU_CYC   TO_CPU_CC(340 ) /* ÿ��ˮƽ��������    */
#define N_VBLANK_CPU_CYC   TO_CPU_CC(0   ) /* ��ֱ��������        */
                                           /* ÿ֡����            */
#define N_FRAME_CPU_CYC       \
            ( (N_HLINE_CPU_CYC + N_HBLANK_CPU_CYC) * N_VLINE_COUNT )
                                           /* ÿ��������          */
#define N_PIXEL_CPU_CYC       \
            ( N_HLINE_CPU_CYC / PPU_DISPLAY_P_WIDTH )
#define N_END_CYC             4
/*----------------------------------------------------------------*/

struct BackGround {

    byte name      [0x03C0];
    byte attribute [0x0040];

    /* offset=[0 - 0x400] */
    void write(word offset, byte data) {
#ifdef SHOW_ERR_MEM_OPERATE
        _check(offset);
#endif
        if (offset<0x03C0) {
            name[offset] = data;
        } else {
            attribute[offset-0x03C0] = data;
        }
    }

    /* offset=[0 - 0x400] */
    byte read(word offset) {
#ifdef SHOW_ERR_MEM_OPERATE
        _check(offset);
#endif
        if (offset<0x03C0) {
            return name[offset];
        } else {
            return attribute[offset-0x03C0];
        }
    }

    void _check(word i) {
        if (i>=0x400) printf("BackGround::out of offset %x", i);
    }
};

/*----------------------------------------| vrom ӳ�� |----*-
 * ÿ�� 32�� x 30�� ��ͼ�ε�Ԫ, ����ʾ960����Ԫ            *
 * ÿ��ͼ�ε�Ԫ8x8����,16�ֽ�,ÿ���Ᵽ��256��ͼ�ε�Ԫ      *
 * ÿ��ͬ��64����ͨ��Ԫ(һ��ҳ0x100�ֽ�)                   *
 * ��ͨ�������ڴ���, 4�ֽ�: 1.Y 2.�ֿ���� 3.��״ 4.X      *
 *                                                         *
 * $0000-$0FFF ��ͨͼ�ο�                                  *
 * $1000-$1FFF �����ַ�ͼ��                                *
-*----------------------------------------| vram ӳ�� |----*
 * $2000-$23BF ������һҳӳ�� 960(�ֽ�)��ͼ�ε�Ԫ          *
 * $23C0-$23FF ������һҳ��ɫ�� 64(�ֽ�)����ɫ��Ԫ         *
 *                                            (һ��ӳ��1KB)*
 * $2400-$27BF �����ڶ�ҳӳ��                              *
 * $27C0-$27FF �����ڶ�ҳ��ɫ�� 0x7FF 2KB                  *
 *                                                         *
 * $2800-$2BBF ��������ҳӳ��                              *
 * $2BC0-$2BFF ��������ҳ��ɫ�� 0xBFF 3KB                  *
 *                                                         *
 * $2C00-$2FBF ��������ҳӳ��                              *
 * $2FC0-$2FFF ��������ҳ��ɫ�� 0xFFF 4KB                  *
 *                                                         *
 * $3000-$3EFF $2000 - $2EFF �ľ���                        *
 *                                                         *
 * $3F00-$3F1F ������ͨ��ɫ�������� ��16�ֽ�               *
 * $3F20-$3FFF Ϊ��  $3F00-$3F1F ��7�ξ���                 *
 * $4000-$FFFF $0000 - $3FFF �ľ���                      *
-*----------------------------------------| vram ӳ�� |----*/
struct PPU {

private:
    BackGround  bg[4];
    BackGround *pbg[4];

    byte bkPalette[16];     /* ������ɫ��                          */
    byte spPalette[16];     /* ��ͨ��ɫ��                          */

    byte spWorkRam[256];    /* ��ͨ�����ڴ�                        */
    word spWorkOffset;      /* ��ͨ����ҳ���׵�ַ                  */
    word ppu_ram_p;         /* ppu�Ĵ���ָ��,���ᳬ�� 0x3FFF       */
    byte addr_add;          /* ��ַ�����ۼ�ֵ                      */
    enum { pH, pL } ppuSW;  /* д��ppu�Ĵ�����λ�� $0000-$3FFF     */

    word winX;
    word winY;
    word tmpwinY, tmpwinX;
    enum { wX, wY } w2005;  /* д����һ������                      */

    word spRomOffset;       /* ��ͨ�ֿ��׵�ַ                      */
    word bgRomOffset;       /* �����ֿ��׵�ַ                      */
    enum { t8x8, t8x16 } spriteType;

    byte *NMI;              /* cpu��NMI��ַ�ߣ�������cpu����NMI    */
    byte sendNMI;           /* �Ƿ���ˢ��һ֡����NMI             */
    byte vblankTime;        /* ������ڴ�ֱ����ʱ����Ϊ1           */

    byte bkleftCol;         /* ������ʾ��һ��                      */
    byte spleftCol;         /* ��ͨ��ʾ��һ��                      */
    byte bkAllDisp;         /* ����ȫ��ʾ                          */
    byte spAllDisp;         /* ��ͨȫ��ʾ                          */

    byte hasColor;          /* ����ɫ��                            */
    T_COLOR bkColor;        /* ������ɫ                            */

    byte spOverflow;        /* ��ͨ8�����                         */
    byte hit;               /* ��ͨ��ײ                            */

    int  sp0x, sp0y;        /* ��¼0�ſ�ͨ��λ��                   */
    byte sp0hit[8][8];      /* ��������ײ���                      */
    byte readBuf;           /* PPU���Ƿ�����һ�ζ�ȡ������,��ÿ��
                             * �޸�ָ��ʱ<0x3F00��û��Ԥ��ȡ����bug*/
    word tmp_addr;          /* �����޸ĵ�ַ��λ                    */
    int  currentDrawLine;   /* ��ǰ������Ⱦ����, ����ʱʹ��        */

    MMC   *mmc;
    Video *video;

    void control_2000(byte data);
    void control_2001(byte data);

    void write(byte);       /* д����                              */
    byte read();            /* ������                              */
    BackGround* swBg(word); /* ����word��ֵ�õ���Ӧ�ı���ָ��      */

    /* ����x,y��λ�ô�attr���Ա���ȡ����ɫ�ĸ���λ                 */
    byte bgHBit(int x, int y, byte *attr);
    /* ����x,y��λ�ô�attr���Ա���ȡ����ɫ�ĵ���λ                 */
    byte gtLBit(int x, int y, byte tileIdx, word vromOffset);

    void _drawSprite(byte spriteIdx, byte);
    void _checkHit(int x, int y);
    /* ʹ��mask(1)���tmp_addr,����d��0λ�������tmp_addr */
    void _setTmpaddr(uint mask, uint d);
    /* ʹ��tmp_addr���õ�ǰppuramָ������Ļƫ�� */
    void _resetScreenOffset(bool newFrame);

public:
    enum bgPriority {bpFront, bpBehind};

    PPU(MMC *mmc, Video *video);

    void reset();
    /* cpuͨ��д0x2000~0x2007(0x3FFF)����PPU                       */
    void controlWrite(word addr, byte data);
    /* cpuͨ����0x2000~0x2007(0x3FFF)�õ�PPU״̬                   */
    byte readState(word addr);
    /* �л���Ļ����                                                */
    void switchMirror(byte type);
    /* ����cpu��NMI��ַ��                                          */
    void setNMI(byte* cpu_nmi);
    /* ��һ֡�������ʱ�����Է����ж�, ϵͳԤ��ʱҲ��Ҫ��������    */
    void oneFrameOver();
    /* ����ʼ����һ���µ�֡ʱ,�÷���������                         */
    void startNewFrame();
    /* ��cpu�����ж��ź�                                           */
    void sendingNMI();
    /* 2270 cpu ���ں����VBL                                      */
    void clearVBL();
    /* ����256�ֽڵ����ݵ�����Ram,��Ҫ512��CPU����                 */
    void copySprite(byte *data);
    /* ȡ�ô�������                                                */
    void getWindowPos(int *x, int *y);
    /* ���ص�ǰppu�Դ�ָ��                                         */
    word getVRamPoint();
    /* ��ʼ�����µ�ɨ����                                          */
    void startNewLine();

    /* ����һ֡�еľ���                                            */
    void drawSprite(bgPriority);
    /* ��video�ϻ����ĸ����� 512*480                               */
    void drawBackGround(Video *v);
    /* �������Ʊ����ֿ�                                            */
    void drawTileTable(Video *v);
    /* ����ָ��λ�õ�����                                          */
    void drawPixel(int x, int y);
};

#endif // PPU_H_INCLUDED
