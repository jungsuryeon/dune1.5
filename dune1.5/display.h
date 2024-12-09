/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 표시할 색상 정의. 대충 맞춰 뒀는데, 취향껏 추가하거나 변경하기
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112

// 지금은 자원, 맵, 커서만 표시
// 앞으로 화면에 표시할 내용들 여기에 추가하기
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	CURSOR cursor
);

extern OBJECT_SAMPLE H_ai_units[20];
extern OBJECT_SAMPLE H_units[20];
extern  BUILD_s P_buildings[10];
extern char attack_buildings[1][100];
extern char sistem[5][100];

extern UNIT Harvest;
extern UNIT Fremen;
extern UNIT Soldier;
extern UNIT Fighter;
extern UNIT heavy_Tank;

extern BUILD_s P_buildings[10];
extern BUILD_s D_buildings[10];
extern BUILD_s G_buildings[10];
extern BUILD_s B_buildings[10];
extern BUILD_s S_buildings[10];
extern BUILD_s A_buildings[10];
extern BUILD_s F_buildings[10];

void display_system_message(void);
void display_object_info(void);
void display_commands(void);
void object_info_mark(CURSOR cursor, RESOURCE* resource);
void esc_choice(CURSOR cursor, RESOURCE* resource);
void mark_esc(void);
void h_push(CURSOR cursor, RESOURCE* resource);
void B_push(void);
void P_push(void);
OBJECT_SAMPLE* P_unit_push(CURSOR cursor);
void D_push(void);
void G_push(void);
void S_push(CURSOR cursor, RESOURCE* resource);
void F_push(CURSOR cursor, RESOURCE* resource);
OBJECT_SAMPLE* M_push(CURSOR cursor);
void sistem_letter(char arr[][100], char H_sistem[1][100]);
void random_space_letter(int num, RESOURCE* resource);
char Build_base[5][100];
void command_letter(char arr[][100]);
void initial_H(void);
void initial_ai_H(void);
int Build_select;
void reset(POSITION dest);
int find_H_positions(POSITION H_positions[]);
POSITION  SANDWORM_find(POSITION worm_position);
POSITION  space_find(POSITION space_position, int ai_player);
void division(POSITION pos1, RESOURCE* resource);
int push_units(OBJECT_SAMPLE units[], POSITION pos, int strength, UNIT unit);
int find_space_positions(POSITION num_positions[]); 
void Building(CURSOR cursor, RESOURCE* resource, BUILD* name, char H_sistem[1][100], char state[5][100]);
UNIT Soldier;
void AI_Building(RESOURCE* resource, BUILD* name);
void initial_p(void);
#endif
