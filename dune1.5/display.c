/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include <time.h>
#include <conio.h>
#include "display.h"
#include "io.h"
#include "common.h"

clock_t last_press_time = 0;

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION system_pos = { MAP_HEIGHT + 1, 0 };
const POSITION object_info_pos = { 1, MAP_WIDTH + 1 };
const POSITION commands_pos = { MAP_HEIGHT + 1, MAP_WIDTH + 1 };


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

//ȭ�� ��ġ
char object_info[MAP_HEIGHT][MAP_WIDTH - 10] = { 0 };
char system_message[10][MAP_WIDTH] = { 0 };
char commands[10][MAP_WIDTH - 10] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_system_message(void);
void display_object_info(void);
void display_commands(void);
int get_color(char backbuf[MAP_HEIGHT][MAP_WIDTH]);


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{
	display_resource(resource); // �ڿ� ���� ǥ��
	display_map(map);
	clock_t last_time = clock();
	display_cursor(cursor);
}

void display_resource(RESOURCE resource) { // �ڿ� ���� ǥ��
	set_color(COLOR_RESOURCE);// �÷� ����
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

void display_system_message(void) {

	for (int j = 0; j < MAP_WIDTH; j++) {
		system_message[0][j] = '#';
		system_message[9][j] = '#';
	}

	for (int i = 1; i < 9; i++) {
		system_message[i][0] = '#';
		system_message[i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			system_message[i][j] = ' ';
		}
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			POSITION pos = { i, j };
			printc(padd(system_pos, pos), system_message[i][j], COLOR_DEFAULT);
		}
	}
}

void display_object_info(void) {

	for (int j = 0; j < MAP_WIDTH - 10; j++) {
		object_info[0][j] = '#';
		object_info[MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		object_info[i][0] = '#';
		object_info[i][MAP_WIDTH - 11] = '#';
		for (int j = 1; j < MAP_WIDTH - 11; j++) {
			object_info[i][j] = ' ';
		}
	}

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH - 10; j++) {
			POSITION pos = { i, j };
			printc(padd(object_info_pos, pos), object_info[i][j], COLOR_DEFAULT);
		}
	}
}


void display_commands(void) {

	for (int j = 0; j < MAP_WIDTH - 10; j++) {
		commands[0][j] = '#';
		commands[9][j] = '#';
	}

	for (int i = 1; i < 9; i++) {
		commands[i][0] = '#';
		commands[i][MAP_WIDTH - 11] = '#';
		for (int j = 1; j < MAP_WIDTH - 11; j++) {
			commands[i][j] = ' ';
		}
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < MAP_WIDTH - 10; j++) {
			POSITION pos = { i, j };
			printc(padd(commands_pos, pos), commands[i][j], COLOR_DEFAULT);
		}
	}
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {//frontbuf ���� ���� , backbuf �������
				POSITION pos = { i, j };
				printc(padd(map_pos, pos), backbuf[i][j], get_color(backbuf[i][j]));
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

int get_color(char backbuf) { // ���� �ٲٴ� ��
	switch (backbuf) {
	case 'B': return 159; break;
	case 'H': return 207; break;
	case 'W': return 111; break;//������ Ȳ����
	case 'P': return 14; break;//���� ������
	case 'S': return 175; break;//�����̽� ��Ȳ��
	case 'R': return 112; break;
	default: return COLOR_DEFAULT; break;
	}
}

// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous; // ������ġ
	POSITION curr = cursor.current; // ������ġ

	char ch = frontbuf[prev.row][prev.column];//���� Ŀ���� ����� ��
	printc(padd(map_pos, prev), ch, get_color(backbuf[prev.row][prev.column]));
	ch = frontbuf[curr.row][curr.column];//���ο� ��ġ�� Ŀ���� ���°�
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}


