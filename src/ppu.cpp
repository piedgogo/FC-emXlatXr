#include "ppu.h"
#include <stdio.h>
#include <string.h>

#define DC(b, x)    (b), (b+x), (b+x+x), (b+x+x+x)
#define WHI(x)      (x + (x<<8) +(x<<16))
#define BLK(x)      (x), (x+32), (x+64)

/* 颜色映射, 索引为FC系统颜色, 值为RGB颜色 */
T_COLOR ppu_color_table[0x40] = {
     WHI(0x7F)
    ,DC(0x000080, 0x200000)
    ,DC(0x800000, 0x002000)
    ,DC(0x008000, 0x000020)
    ,BLK(0)

    ,WHI(0xA9)
    ,DC(0x0000FF, 0x200000)
    ,DC(0xFF0000, 0x002000)
    ,DC(0x00FF00, 0x000020)
    ,BLK(8)

    ,WHI(0xD3)
    ,DC(0x4040FF, 0x200000)
    ,DC(0xFF4040, 0x002000)
    ,DC(0x40FF40, 0x000020)
    ,BLK(16)

    ,WHI(0xFF)
    ,DC(0x8080FF, 0x200000)
    ,DC(0xFF8080, 0x002000)
    ,DC(0x80FF80, 0x000020)
    ,BLK(24)
};

#undef DC
#undef WHI
#undef BLK

PPU::PPU(MMC *_mmc, Video *_video)
    : addr_add(1), ppuSW(pH), w2005(wX), mmc(_mmc)
    , video(_video)
{
}

void PPU::controlWrite(word addr, byte data) {
    switch (addr % 0x08) {

    case 0: /* 0x2000                      */
        control_2000(data);
printf("PPU::write addr 2000:%X\n", data);
        break;
    case 1: /* 0x2001                      */
        control_2001(data);
printf("PPU::write addr 2001:%X\n", data);
        break;
    case 3: /* 0x2003 修改卡通工作内存指针 */
        spWorkOffset = data;
printf("PPU::修改精灵指针:%x\n", data);
        break;
    case 4: /* 0x2004 写入卡通工作内存     */
printf("PPU::写入精灵数据: %x = %x\n", spWorkOffset, data);
        spWorkRam[spWorkOffset++] = data;
        break;

    case 5: /* 0x2005 设置窗口坐标         */
        if (w2005==wX) {
            winX &= 0xFF00;
            winX |= data;
            w2005 = wY;
        } else {
            winY &= 0xFF00;
            winY |= data;
            w2005 = wX;
        }
printf("PPU::设置窗口坐标 x:%d, y:%d\n", winX, winY);
        break;

    case 6: /* 0x2006 PPU地址指针           */
        if (ppuSW==pH) {
            ppu_ram_p = (ppu_ram_p & 0x00FF) | (data<<8);
            //ppu_ram_p %= 0x4000;
            ppuSW = pL;
        } else {
            ppu_ram_p = (ppu_ram_p & 0xFF00) | data;
            ppuSW = pH;
        }
printf("PPU::修改PPU指针:%04x\n", ppu_ram_p);
        break;

    case 7: /* 0x2007 写数据寄存器          */
printf("PPU::向PPU写数据:%04X = %04X\n", ppu_ram_p, data);
        write(data);
        break;
    }
}

#define _BIT(x)  (data & (1<<x))

inline void PPU::control_2000(byte data) {
    if (_BIT(0)) {
        winX |= 0x0100;
    } else {
        winX &= 0x00FF;
    }

    if (_BIT(1)) {
        winY |= 0x0100;
    } else {
        winY &= 0x00FF;
    }

    addr_add    = _BIT(2) ? 0x20   : 0x01;
    spRomOffset = _BIT(3) ? 0x1000 : 0;
    bgRomOffset = _BIT(4) ? 0x1000 : 0;
    spriteType  = _BIT(5) ? t8x16  : t8x8;
    sendNMI     = _BIT(7) ? 1      : 0;
    // D6位 PPU 主/从模式, 没有在NES里使用
}

inline void PPU::control_2001(byte data) {
    hasColor    = _BIT(0) ? 0 : 1;
    bkleftCol   = _BIT(1) ? 0 : 1;
    spleftCol   = _BIT(2) ? 0 : 1;
    bkAllDisp   = _BIT(3) ? 0 : 1;
    spAllDisp   = _BIT(4) ? 0 : 1;
    red         = _BIT(5) ? 1 : 0;
    green       = _BIT(6) ? 1 : 0;
    blue        = _BIT(7) ? 1 : 0;
}

#undef _BIT

byte PPU::readState(word addr) {
    byte r = 0xFF;

    switch (addr % 0x08) {

    case 2: /* 0x2002                       */
        r = 0;
        if (preheating) {
            preheating >>= 1;
            vblankTime = 1;
        }
        if ( spOverflow ) r |= ( 1<<5 );
        if ( spClash    ) r |= ( 1<<6 );
        if ( vblankTime ) r |= ( 1<<7 );
        vblankTime = 0;
        ppuSW      = pH;
        w2005      = wX;
        break;

    case 7: /* 0x2007 读数据寄存器          */
        r = read();
        break;
    }
printf("PPU::read:%X, return:%X\n", addr, r);
    return r;
}

BackGround* PPU::swBg() {
    word offs = (ppu_ram_p % 0x3000 - 0x2000) / 0x400;
    switch (offs) {
    case 0:
        return bg0;
    case 1:
        return bg1;
    case 2:
        return bg2;
    case 3:
        return bg3;
    }
    return NULL;
}

inline byte PPU::read() {
    byte res = 0xFF;

    if (ppu_ram_p<0x2000) {
        res = mmc->readVRom(ppu_ram_p);
    } else

    if (ppu_ram_p<0x3EFF) {
        BackGround* pBg = swBg();
        res = pBg->read(ppu_ram_p);
    } else

    if (ppu_ram_p<0x3FFF) {
        word off = ppu_ram_p % 0x20;
        if (off<0x10) {
            res = bkPalette[off     ];
        } else {
            res = spPalette[off-0x10];
        }
    }

    ppu_ram_p += addr_add;
    return res;
}

inline void PPU::write(byte data) {
    if (ppu_ram_p<0x2000) {
#ifdef SHOW_ERR_MEM_OPERATE
        printf("PPU::can't write VROM $%04x=%02x.\n", ppu_ram_p, data);
//        return;
#endif
    } else

    if (ppu_ram_p<0x3EFF) {
        BackGround* pBg = swBg();
        pBg->write(ppu_ram_p, data);
    } else

    if (ppu_ram_p<0x3FFF) {
        word off = ppu_ram_p % 0x20;
        if (off<0x10) {
            bkPalette[off     ] = data;
        } else {
            spPalette[off-0x10] = data;
        }
    }

    ppu_ram_p += addr_add;
}

void PPU::swithMirror(byte type) {
    switch (type) {

    case PPU_VMIRROR_VERTICAL:
        bg0 = bg2 = &bg[0];
        bg1 = bg3 = &bg[1];
    break;

    case PPU_VMIRROR_HORIZONTAL:
        bg0 = bg1 = &bg[0];
        bg2 = bg3 = &bg[1];
    break;

    case PPU_VMIRROR_SINGLE:
        bg0 = bg1 = bg2 = bg3 = &bg[0];
    break;

    case PPU_VMIRROR_4_LAYOUT:
        bg0 = &bg[0];
        bg1 = &bg[1];
        bg2 = &bg[2];
        bg3 = &bg[3];
    break;
    }
}

void PPU::reset() {
    spOverflow = 0;
    spClash    = 0;
    *NMI       = 0;
    preheating = 3;
}

void PPU::setNMI(byte* cpu_nmi) {
    NMI = cpu_nmi;
}

void PPU::drawTileTable() {
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
                video->drawPixel(x++, y, ppu_color_table[colorIdx]);
            }
            x-= 8;
            y++;
        }
        x+= 8;
        y-= 8;
        if (p && p%32==0) {
            x = 0;
            y+= 8;
        }
    }
}

void PPU::drawBackGround(int id) {
    byte dataH, dataL;
    byte colorIdx;
    int x=0, y=0;

    for (int j=0; j<960; ++j) {
        byte p = bg[id].name[j];

        for (int i=0; i<8; ++i) {
            dataL = mmc->readVRom(p * 16 + i + bgRomOffset);
            dataH = mmc->readVRom(p * 16 + i + bgRomOffset + 8);

            for (int d=7; d>=0; --d) {
                colorIdx = 0;
                if (dataL>>d & 1) {
                    colorIdx |= 1;
                }
                if (dataH>>d & 1) {
                    colorIdx |= 2;
                }

                if (colorIdx) {
                    byte attrIdx = x/8 + y/8;// 问题的关键,溢出
                    colorIdx |= bg[id].attribute[attrIdx] << 2;
                    colorIdx = bkPalette[colorIdx];
                    video->drawPixel(x++, y, ppu_color_table[colorIdx]);
            printf("%x\t", colorIdx);
                }
            }
            x-= 8;
            y++;
        }
        x+= 8;
        y-= 8;
        if (x>=256) {
            x = 0;
            y+= 8;
        }
    }
}

void PPU::drawSprite() {
    byte dataH, dataL;
    byte colorIdx;
    int  i;
    for (int sp=63; sp>=0; --sp) {
        i = sp * 4;
        byte x    = spWorkRam[i  ];
        byte y    = spWorkRam[i+3] - 1;
        byte tile = spWorkRam[i+1];
        byte ctrl = spWorkRam[i+2];
        word tileAddr =  tile*16;

        for (int r=0; r<8; ++r) {
            dataL   = mmc->readVRom(tileAddr + r    );
            dataH   = mmc->readVRom(tileAddr + r + 8);

            for (int d=7; d>=0; --d) {
                colorIdx = 0;
                if (dataL>>d & 1) {
                    colorIdx |= 1;
                }
                if (dataH>>d & 1) {
                    colorIdx |= 2;
                }
                //if (colorIdx) {
                    colorIdx |= ((ctrl & 0x03) << 2);
                    video->drawPixel(x++, y, ppu_color_table[colorIdx]);
                //}
            }
            x-=8;
            y++;
        }
    }
}

/* 这个方法的实现还是有问题 */
void PPU::drawPixel(int X, int Y) {
/*-----------------| 绘制背景 |-------------------*/
    word nameIdx = (X/8) * (Y/8);
    word tileIdx = bg0->name[nameIdx];
    word tileAddr= bgRomOffset + tileIdx*16 + Y%8;

    byte tile0 = mmc->readVRom(tileAddr    );
    byte tile1 = mmc->readVRom(tileAddr + 8);
    byte tileX = 1<<(7 - X%8);
/*
if (tileIdx&&0)
printf("x:%03d \t y:%03d \t nameIdx:%d \t tileIdx:%d \t bgoff:%d\n"
       , X, Y, nameIdx, tileIdx, bgRomOffset);
*/
    byte paletteIdx = 0;
    if (tile0 & tileX) paletteIdx |= 0x01;
    if (tile1 & tileX) paletteIdx |= 0x02;

    if (paletteIdx) {
        byte attrIdx = X/8 + Y/8;
        paletteIdx |= bg0->attribute[attrIdx] << 2;
        byte colorIdx = bkPalette[paletteIdx];
        video->drawPixel(X, Y, ppu_color_table[colorIdx]);
    }
/*----------------| end |-------------------------*/
}

void PPU::oneFrameOver() {
    if (sendNMI) {
        *NMI = 1;
#ifdef NMI_DEBUG
        printf("PPU::发送中断到CPU\n");
#endif
    }
    vblankTime = 1;
}

void PPU::startNewFrame() {
    vblankTime = 0;
}

void PPU::copySprite(byte *data) {
    memcpy(spWorkRam, data, 256);
}
