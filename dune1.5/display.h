/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112

// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

void display_system_message(void);
void display_object_info(void);
void display_commands(void);
void object_info_mark(CURSOR cursor, RESOURCE* resource);
void esc_choice(CURSOR cursor, RESOURCE* resource);
void mark_esc(void);
void h_push(CURSOR cursor, RESOURCE* resource);
void B_push(void);
void P_push(void);
void D_push(void);
void G_push(void);
void S_push(CURSOR cursor, RESOURCE* resource);
void sistem_letter(char arr[][100], char H_sistem[1][100]);
void random_space_letter(int num);
char Build_base[5][100];
void command_letter(char arr[][100]);
void initial_H(void);
int Build_select;

#endif
