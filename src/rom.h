#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

#include "type.h"
#include "string"

using std::string;

/**
 * ��ȡfilename�ļ���rom�ṹ��,�ɹ�����0������rom����
 * rom������������Ч��, ʧ�ܷ���-1
 */
int load_rom(nes_file* rom, string* filename);

#endif // ROM_H_INCLUDED
