/*----------------------------------------------------------------------------*|
|*                                                                            *|
|* FC ģ���� (Famicom��Nintendo��˾��1983��7��15�����ձ����۵�8λ��Ϸ��)      *|
|*                                                                            *|
|* $ C++���Եĵ�һ����Ŀ,���������ְ�                                         *|
|* $ è��д��, �����ñ����������ע������                                     *|
|* $ ���߶�ʹ�ñ�������ɵĺ�������κ�����                                   *|
|* $ �಻��Դ���Ĺ���ԭ������һ������,�����ش������벦��119 & 911           *|
|*                                                                            *|
|* > ʹ�� [Code::Block 10.05] ��������                                        *|
|* > ������ʹ�� [MinGW 3.81] [gcc 4.4.1]                                      *|
|* > �ο������� [http://nesdev.parodius.com] ��վ������                       *|
|* > ��л [Flubba] ��ƵĲ��Գ���, ����������Ч�ʳ�ָ������                   *|
|*                                                                            *|
|* ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ | CatfoOD |^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ *|
|*                                           | yanming-sohu@sohu.com          *|
|* [ TXT CHARSET WINDOWS-936 / GBK ]         | https://github.com/yanmingsohu *|
|*                                           | qQ:412475540                   *|
|*----------------------------------------------------------------------------*/
#include "ppu.h"
#include <stdio.h>
#include <string.h>

#define H(a) 0x##a
/* ��ɫӳ��, ����ΪFCϵͳ��ɫ, ֵΪRGB��ɫ */
T_COLOR ppu_color_table[0x40] = {
     H(808080), H(003da6), H(0012b0), H(440096)
    ,H(a1005e), H(c70028), H(ba0600), H(8c1700)
    ,H(5c2f00), H(104500), H(054a00), H(00472e)
    ,H(004166), H(000000), H(050505), H(050505)

    ,H(c7c7c7), H(0077ff), H(2155ff), H(8237fa)
    ,H(eb2fb5), H(ff2950), H(ff2200), H(d63200)
    ,H(c46200), H(358000), H(058f00), H(008a55)
    ,H(0099cc), H(212121), H(090909), H(090909)

    ,H(ffffff), H(0fd7ff), H(69a2ff), H(d480ff)
    ,H(ff45f3), H(ff618b), H(ff8833), H(ff9c12)
    ,H(fabc20), H(9fe30e), H(2bf035), H(0cf0a4)
    ,H(05fbff), H(5e5e5e), H(0d0d0d), H(0d0d0d)

    ,H(ffffff), H(a6fcff), H(b3ecff), H(daabeb)
    ,H(ffa8f9), H(ffabb3), H(ffd2b0), H(ffefa6)
    ,H(fff79c), H(d7e895), H(a6edaf), H(a2f2da)
    ,H(99fffc), H(dddddd), H(111111), H(111111)
};
#undef H

/* ϵͳ����ʱ��Ĭ����ɫ�� */
static byte RESET_PALETTE_TABLE[] = {
     0x09, 0x01, 0x00, 0x01, 0x00, 0x02, 0x02, 0x0D,
     0x08, 0x10, 0x08, 0x24, 0x00, 0x00, 0x04, 0x2C,
     0x09, 0x01, 0x34, 0x03, 0x00, 0x04, 0x00, 0x14,
     0x08, 0x3A, 0x00, 0x02, 0x00, 0x20, 0x2C, 0x08,
};

PPU::PPU(MMC *_mmc, Video *_video)
    : spWorkOffset (0)
    , addr_add     (1)
    , flipflop(Flip1w)
    , bkAllDisp    (0)
    , spAllDisp    (0)
    , mmc       (_mmc)
    , video   (_video)
{
    memset(spWorkRam, 0, sizeof(spWorkRam));
}

void PPU::reset() {
    spOverflow  = 0;
    hit         = 0;
    *NMI        = 0;
    spRomOffset = 0;
    bgRomOffset = 0;
    winX        = 0;
    winY        = 0;
    flipflop    = Flip1w;

    memset(spWorkRam, 0, sizeof(spWorkRam));
    memset(bg,        0, sizeof(bg)       );
    memcpy(bkPalette, RESET_PALETTE_TABLE,    16);
    memcpy(spPalette, RESET_PALETTE_TABLE+16, 16);
}

void PPU::setNMI(byte* cpu_nmi) {
    NMI = cpu_nmi;
}

void PPU::controlWrite(word addr, byte data) {

    switch (addr % 0x08) {

    case 0: /* 0x2000 */
        control_2000(data);
#ifdef SHOW_PPU_REGISTER
        printf("PPU::write addr 2000:%X\n", data);
#endif
        break;

    case 1: /* 0x2001 */
        control_2001(data);
#ifdef SHOW_PPU_REGISTER
        printf("PPU::write addr 2001:%X\n", data);
#endif
        break;

    case 3: /* 0x2003 �޸Ŀ�ͨ�����ڴ�ָ�� */
        spWorkOffset = data;
#ifdef SHOW_PPU_REGISTER
        printf("PPU::�޸ľ���ָ��:%x\n", data);
#endif
        break;

    case 4: /* 0x2004 д�뿨ͨ�����ڴ� */
#ifdef SHOW_PPU_REGISTER
        printf("PPU::д�뾫������: %x = %x\n", spWorkOffset, data);
#endif
#ifdef SHOW_ERR_MEM_OPERATE
        if (spWorkOffset>=256) {
            printf("PPU::д�뾫���ڴ泬��256�ֽ��ж�д��\n");
            break;
        }
#endif
        spWorkRam[spWorkOffset++] = data;
        break;

    case 5: /* 0x2005 ���ô������� */
        if (flipflop == Flip1w) {
            flipflop = Flip2w;
            _setTmpaddr(0x001F, data >> 3);
            tileXoffset = data & 0x07;
        } else {
            flipflop = Flip1w;
            _setTmpaddr(0x03E0, (data & 0x00F8) << 2 );
            _setTmpaddr(0x7000, (data & 0x0007) << 12);
        }
        break;

    case 6: /* 0x2006 PPU��ַָ�� */
        if (flipflop == Flip1w) {
            flipflop = Flip2w;
            _setTmpaddr(0x3F00, (data & 0x003F) << 8);
        } else {
            flipflop = Flip1w;
            _setTmpaddr(0x00FF, data);
            ppu_ram_p = tmp_addr;
#ifdef SHOW_PPU_REGISTER
            printf("PPU::�޸�PPUָ��:%04x\n", ppu_ram_p);
#endif
        }
        break;

    case 7: /* 0x2007 д���ݼĴ��� */
#ifdef SHOW_PPU_REGISTER
        printf("PPU::��PPUд����:%04X = %04X\n", ppu_ram_p, data);
#endif
        write(data);
        ppu_ram_p = (ppu_ram_p + addr_add) & 0x3FFF;
        break;
    }
}

#define _BIT(x)  (data & (1<<x))

inline void PPU::control_2000(byte data) {
    _setTmpaddr(0x0C00, (data & 0x3) << 10);

    addr_add    = _BIT(2) ? 0x20   : 0x01;
    spRomOffset = _BIT(3) ? 0x1000 : 0;
    bgRomOffset = _BIT(4) ? 0x1000 : 0;
    spriteType  = _BIT(5) ? t8x16  : t8x8;
    sendNMI     = _BIT(7) ? 1      : 0;
    // D6λ PPU ��/��ģʽ, û����NES��ʹ��, ˫PPU??!!
}

inline void PPU::control_2001(byte data) {
    hasColor    = _BIT(0) ? 0 : 1;
    bkleftCol   = _BIT(1) ? 1 : 0;
    spleftCol   = _BIT(2) ? 1 : 0;
    bkAllDisp   = _BIT(3) ? 1 : 0;
    spAllDisp   = _BIT(4) ? 1 : 0;

    if (_BIT(5)) {
        bkColor = ppu_color_table[0x02];
    } else if (_BIT(6)) {
        bkColor = ppu_color_table[0x2A];
    } else if (_BIT(7)) {
        bkColor = ppu_color_table[0x17];
    } else {
        bkColor = 0;
    }
}

#undef _BIT

byte PPU::readState(word addr) {
    byte r;

    switch (addr % 0x08) {

    case 2: /* 0x2002 */
        r = 0;
        if ( spOverflow ) r |= ( 1<<5 );
        if ( hit        ) r |= ( 1<<6 );
        if ( vblankTime ) r |= ( 1<<7 );
        vblankTime = 0;
        flipflop   = Flip1w;
        break;

    case 4: /* 0x2004 ��ȡ��ͨ�����ڴ� */
#ifdef SHOW_ERR_MEM_OPERATE
        if (spWorkOffset>=256) {
            printf("PPU::��ȡ�����ڴ泬��256�ֽ��ж϶�ȡ\n");
            break;
        }
#endif
        /* Address should not increment on $2004 read */
        r = spWorkRam[spWorkOffset];
#ifdef SHOW_PPU_REGISTER
        printf("PPU::��ȡ��������: %X = %x\n", spWorkOffset, r);
#endif
        break;

    case 7: /* 0x2007 ��ȡVRAM */
        if (ppu_ram_p < 0x3F00) {
            r = readBuf;
            readBuf = read();
        } else {
            r = read();
        }
        ppu_ram_p = (ppu_ram_p + addr_add) & 0x3FFF;
        break;

#ifdef SHOW_ERR_MEM_OPERATE
    default:
        printf("PPU::��Ч���Դ��ȡ�˿ں� %X(%x).\n", addr, addr%0x8);
        break;
#endif
    }

#ifdef SHOW_PPU_REGISTER
    printf("PPU::read:%X, return:%X\n", addr, r);
#endif
    return r;
}

/* offs => [0x2000 - 0x4000] */
BackGround* PPU::swBg(word offs) {
    if (offs >= 0x3000) {
        offs -= 0x3000;
    }

    if (offs<0x2400) {
        return pbg[0];
    } else
    if (offs<0x2800) {
        return pbg[1];
    } else
    if (offs<0x2C00) {
        return pbg[2];
    } else
    if (offs<0x3000) {
        return pbg[3];
    }

#ifdef SHOW_ERR_MEM_OPERATE
    printf("PPU::��Ч���Դ����%X.\n", offs);
    return NULL;
#endif
}

byte PPU::read() {
    byte res = 0xFF;
    word &offset = ppu_ram_p;

    if (offset<0x2000) {
        res = mmc->readVRom(offset);
    }
    else if (offset<0x3F00) {
        BackGround* pBg = swBg(offset);
        res = pBg->read(offset % 0x0400);
    }
    else {
        word off = offset % 0x20;
        if (off<0x10) {
            res = bkPalette[off     ];
        } else {
            res = spPalette[off-0x10];
        }
        /* In monochrome mode (Port 2001h/Bit0=1)
           the returned lower 4bit are zero */
        if (!hasColor) {
            res &= 0xF0;
        }
    }

    return res;
}

void PPU::write(byte data) {
    word offset = ppu_ram_p;

    if (offset<0x2000) {
        mmc->writeVRom(offset, data);
    }
    else if (offset<0x3F00) {
        BackGround* pBg = swBg(offset);
        pBg->write(offset % 0x0400, data);
    }
    else {
        /* д��ʱȥ����2λ */
        data &= 0x3F;
        if (offset % 4 == 0) {
            bkPalette[  0] = data;
            bkPalette[  4] = data;
            bkPalette[  8] = data;
            bkPalette[0xC] = data;
            spPalette[  0] = data;
            spPalette[  4] = data;
            spPalette[  8] = data;
            spPalette[0xC] = data;
        }
        else {
            word off = offset % 0x20;
            if (off<0x10) {
                bkPalette[off     ] = data;
            } else {
                spPalette[off-0x10] = data;
            }
        }
    }
}

void PPU::switchMirror(byte type) {
    switch (type) {

    case PPU_VMIRROR_VERTICAL:
        pbg[0] = pbg[2] = &bg[0];
        pbg[1] = pbg[3] = &bg[1];
    break;

    case PPU_VMIRROR_HORIZONTAL:
        pbg[0] = pbg[1] = &bg[0];
        pbg[2] = pbg[3] = &bg[1];
    break;

    case PPU_VMIRROR_SINGLE:
        pbg[0] = pbg[1] = pbg[2] = pbg[3] = &bg[0];
    break;

    case PPU_VMIRROR_4_LAYOUT:
        pbg[0] = &bg[0];
        pbg[1] = &bg[1];
        pbg[2] = &bg[2];
        pbg[3] = &bg[3];
    break;

    default:
        printf("PPU::��Ч����Ļ������%d\n", type);
        pbg[0] = pbg[1] = pbg[2] = pbg[3] = &bg[0];
    }
}

void PPU::drawTileTable(Video *v) {
    byte dataH, dataL;
    byte colorIdx;
    int x=0, y=0;

    for (int p=0; p<512; ++p) {
        for (int i=0; i<8; ++i) {
            dataL = mmc->readVRom(p * 16 + i);
            dataH = mmc->readVRom(p * 16 + i + 8);

            for (int d=7; d>=0; --d) {
                colorIdx = 0;
                if (dataL>>d & 1) {
                    colorIdx |= 1;
                }
                if (dataH>>d & 1) {
                    colorIdx |= 2;
                }
                v->drawPixel(x++, y, ppu_color_table[bkPalette[colorIdx]]);
            }
            x-= 8;
            y++;
        }
        x+= 8;
        y-= 8;
        if (p%16==15) {
            x = 0;
            y+= 8;
        }
    }
}

inline byte PPU::gtLBit(int x, int y, byte tileIdx, word vromOffset) {
    byte paletteIdx = 0;
    word tileAddr = vromOffset + (tileIdx << 4) + y % 8;

    byte tile0 = mmc->readVRom(tileAddr    );
    byte tile1 = mmc->readVRom(tileAddr + 8);
    byte tileX = 1<<(7 - x % 8);

    if (tile0 & tileX) paletteIdx |= 0x01;
    if (tile1 & tileX) paletteIdx |= 0x02;

    return paletteIdx;
}

inline byte PPU::bgHBit(int x, int y, byte *attrTable) {
    byte attrIdx = DIV32(x)    + (DIV32(y)    << 3);
    byte attrBit = DIV16(x%32) | (DIV16(y%32) << 1);
    byte attr    = attrTable[attrIdx] >> (attrBit<<1);
    return (attr & 0x3) << 2;
}

void PPU::drawBackGround(Video *panel) {
    int x  = 0, y  = 0;
    int bx = 0, by = 0;
    int bgIdx = 0;

    for (;;) {
        word nameIdx = (x/8) + (y/8*32);
        word tileIdx = pbg[bgIdx]->name[nameIdx];
        byte paletteIdx = gtLBit(x, y, tileIdx, bgRomOffset);

        paletteIdx |= bgHBit(x, y, pbg[bgIdx]->attribute);
        paletteIdx  = bkPalette[paletteIdx];

        if (paletteIdx) {
            panel->drawPixel(bx + x, by + y, ppu_color_table[ paletteIdx ]);
        }

        if (++x>=256) {
            x = 0;
            if (++y>=240) {
                y = 0;
                ++bgIdx;

                if (bgIdx==1) {
                    bx = 256;
                    by = 0;
                }
                else if (bgIdx==2) {
                    bx = 0;
                    by = 240;
                }
                else if (bgIdx==3) {
                    bx = 256;
                    by = 240;
                }
                else {
                    break;
                }
            }
        }
    }
}

void PPU::_drawSprite(byte i, byte ctrl) {
    if (!spAllDisp) return;

    int x, y, dx, dy;

    byte by   = spWorkRam[i  ]+1;
    if (by > 0xEF) return;

    byte tile = spWorkRam[i+1];
    if (!ctrl)
         ctrl = spWorkRam[i+2];
    byte bx   = spWorkRam[i+3];

    /* 8x8 �����ض���Ҫ�жϣ�������ǰ��� */
    bool h    = ctrl & (1<<6);
    bool v    = ctrl & (1<<7);
    byte hiC  = (ctrl & 0x03) << 2;

    if (!i) { sp0x = bx;
              sp0y = by; }

    for (bool notOver = true;;) {
        for (x=0, y=0;;) {
            byte paletteIdx = gtLBit(x, y, tile, spRomOffset);

            if (paletteIdx) {
                if (h) dx = 8 - x + bx;
                else   dx =     x + bx;
                if (v) dy = 8 - y + by;
                else   dy =     y + by;

                video->drawPixel(dx, dy, ppu_color_table[ spPalette[paletteIdx | hiC] ]);

                /* ��¼0�ž�������Ļ�ϻ��Ƶ�����,��0�ž���ִ����ײ���? */
                if (!i) sp0hit[dx%8][dy%8] = 1;
                // else    _checkHit(dx, dy);
            }

            if (++x >= 8) {
                x = 0;
                if (++y >= 8) {
                    break;
                }
            }
        }

        if (spriteType==t8x16 && notOver) {
            tile++;
            by += 8;
            notOver = false;
        } else {
            break;
        }
    }
}

inline void PPU::_checkHit(int x, int y) {
    if (hit) return;
    if (x>=sp0x && x<sp0x+8 && y>=sp0y && y<sp0y+8) {
        hit = sp0hit[x%8][y%8];
    }
}

void PPU::drawSprite(bgPriority bp) {
    for (int i=63<<2; i>=0; i-=4) { // spriteType
        byte ctrl = spWorkRam[i+2];
        if ( ((ctrl>>5) & 1)==bp ) {
            _drawSprite(i, ctrl);
        }
    }
}

void PPU::drawPixel(int X, int Y) {
    if (!bkAllDisp) return;

    int x = (winX + X) % 512;
    int y = (winY + Y) % 480;
    BackGround *bgs;

    if (x<256) {
        if (y<240) {
            bgs = pbg[0];
        } else {
            bgs = pbg[2];
            y   = (winY + Y) % 240;
        }
    } else {
        if (y<240) {
            bgs = pbg[1];
            x   = (winX + X) % 256;
        } else {
            bgs = pbg[3];
            x   = (winX + X) % 256;
            y   = (winY + Y) % 240;
        }
    }

    word nameIdx = DIV8(x) + (DIV8(y)<<5);
    word tileIdx = bgs->name[nameIdx];

    byte paletteIdx = gtLBit(x, y, tileIdx, bgRomOffset)
                    | bgHBit(x, y, bgs->attribute);

    /* ͸��ɫ */
    if (paletteIdx%4==0) return;

    byte colorIdx = bkPalette[paletteIdx];
    video->drawPixel(X, Y, ppu_color_table[colorIdx]);
    _checkHit(X, Y);
}

void PPU::oneFrameOver() {
    vblankTime = 1;
    spWorkOffset = 0;
}

void PPU::clearVBL() {
    vblankTime = 0;
}

void PPU::startNewFrame() {
    hit = 0;

    _resetScreenOffset(true);

    video->clear( ppu_color_table[bkPalette[0]] );
    memset(sp0hit, 0, sizeof(sp0hit));
    _drawSprite(0,0);

#ifdef SHOW_PPU_REGISTER
    currentDrawLine = 0;
#endif
}

void PPU::sendingNMI() {
    if (sendNMI) {
        *NMI = 1;
#ifdef NMI_DEBUG
        printf("PPU::�����жϵ�CPU\n");
#endif
    }
}

void PPU::startNewLine() {
    _resetScreenOffset(false);
#ifdef SHOW_PPU_REGISTER
    currentDrawLine++;
#endif
}

void PPU::copySprite(byte *data) {
    memcpy(spWorkRam, data, 256);
}

void PPU::_resetScreenOffset(bool newFrame) {
    if (!bkAllDisp) return;

    winX = ((tmp_addr & 0x001F) << 3) + tileXoffset;

    if (tmp_addr & 0x0400) {
        winX += 256;
    }

    if (newFrame) {
        ppu_ram_p = tmp_addr;

        winY = ((tmp_addr & 0x03E0) >> 2) | ((tmp_addr & 0x7000) >> 12);
        if (tmp_addr & 0x0800) {
            winY += 256;
        }
    } else {
        ppu_ram_p = (ppu_ram_p & (~0x041F)) | (tmp_addr & 0x041F);
    }
#ifdef SHOW_PPU_REGISTER
    printf("PPU::�޸Ĵ������� x:%d, y:%d ��ǰ��:%d\n", winX, winY, currentDrawLine);
#endif
}

void PPU::getWindowPos(int *x, int *y) {
    *x = winX;
    *y = winY;
}

word PPU::getVRamPoint() {
    return ppu_ram_p;
}

inline void PPU::_setTmpaddr(word mask, word d) {
    tmp_addr &= mask ^ 0xFFFF;
    tmp_addr |= d;
}
