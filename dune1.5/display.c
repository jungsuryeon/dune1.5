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
extern char map[2][MAP_HEIGHT][MAP_WIDTH];


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
int get_color(char backbuf, int row);
void mark_esc(void);
void state_letter(char arr[][100]);
void command_letter(char arr[][100]);
void object_info_mark(CURSOR cursor);
void h_push(CURSOR cursor, RESOURCE* resource);
void sistem_letter(char arr[][100], char H_sistem[1][100]);
void esc_choice(CURSOR cursor, RESOURCE* resource);
//void sistem_esc_choice(void);
POSITION  SANDWORM_find(POSITION worm_position);

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

/*================== 1) ȭ���ġ=======================*/
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

// 1) �ʱ���� ���� �ٲٴ� ��
int get_color(char backbuf, int row) { 
	switch (backbuf) {
	case 'B':
	case 'H':
		if (row > MAP_HEIGHT / 2) {
			return 159; break;
		}
		else {
			return 207; break;
		}
	case 'W': return 111; break;//������ Ȳ����
	case 'P': return 14; break;//���� ������
	case 'R': return 112; break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':return 175; break;//�����̽� ��Ȳ��
	default: return COLOR_DEFAULT; break;
	}
}

/*================== 2),4),5) �޽��� ǥ�� =======================*/

//����â �޽���
void state_letter(char arr[][100]) {
	for (int i = 0; i < 5; i++) {
		char buff[100];
		snprintf(buff, 100, arr[i]);
		POSITION pos = { 3 + i, MAP_WIDTH + 3 };
		gotoxy(pos);
		set_color(COLOR_DEFAULT);
		printf("%s", buff);
	}
}

//���â �޽���
void command_letter(char arr[][100]) {
	for (int i = 0; i < 5; i++) {
		char buff[100];
		snprintf(buff, 100, arr[i]);
		POSITION pos = { MAP_HEIGHT+3 + i, MAP_WIDTH + 3 };
		gotoxy(pos);
		set_color(COLOR_DEFAULT);
		printf("%s", buff);
	}
}

/*===========2),4),5) �޽��� ���� =============*/
char Base[5][100] = { "���¾�~ ���� ��Ʈ���̵�(�÷��̾�)" , "�Ϻ����͸� ������ �� �־�", " ", " ", " " };
char AI[5][100] = { "���¾�~ ���ڳ�(AI)" , "����� ���̾�", " ", " ", " " };
char AI_harvester[5][100] = { "���¾�~ AI �Ϻ�����", " ", " ", " "," " };
char Base_command[5][100] = { "H: Harvest(�Ϻ����� ����)","�Ǽ���� :  ����", "������ : 50", " " };
char place[5][100] = { "���¾�~ �縷" , "�⺻ �����̸�,", "�ǹ��� ���� �� ����", "", " " }; // ��ü�� �ϳ�
char Plate[5][100] = { "���¾�~ ����", "�����̸�," ,"���� �ǹ��� ���� �� �־�", "�ǹ� ���� ���� ��� "," " };
char Rock[5][100] = { "���¾�~ ����", "�����̸�," ,"�������� ����� �� ����", " "," " };
char harvester[5][100] = { "���¾�~ �Ϻ�����", "������: 5 " ,"�α� �� : 5", "�̵��ֱ�:2000","ü��70" };
char harvester_command[5][100] = { "H: Harvest(��Ȯ)", "M : move(�̵�)"," "," "," " };
char Sand[5][100] = { "���¾�~ ������", "õõ�� �����ϰ�" ,"������ �Ϲ� ������ ������ �� �Ծ�����ž�", "���� �輳�� ��(���� �ֱ�� ���差�� ���)"," " };
char Space[5][100] = { "���¾�~ �����̽�", "���差ǥ��(1~9)" ,"�⺻�� 2��", "�������� �����"," " };
char H_sistem_success[1][100] = { "���ο� �Ϻ����Ͱ� �����Ǿ����ϴ�." };
char H_sistem_failure[1][100] = { "�����̽��� ��� �Ϻ����Ͱ� ������ �ȵ˴ϴ�." };
char M_sistem[1][100] = { "�Ϻ����Ͱ� �����Դϴ�." };
char random_space[1][100] = { "�������� �����̽��� �����߽��ϴ�." };
char eat_space[1][100] = { "�������� �Ϻ����͸� �Ծ����ϴ�." };
char sistem[5][100] = { " "," ", " ", " "," " };

//5) �ý��� �޽��� �޽��� �α� ��ũ�� �ø���
int num = 0;
int esc_letter = 0; //esc�� ������ ���� �������� ����
void sistem_letter(char arr[][100],char H_sistem[1][100]) {
	for (int y = 0; y < 45; y++) {
		for (int z = 0; z < 5; z++) {
			POSITION pos1 = { MAP_HEIGHT + 3 + z, 2 + y };
			printc(pos1, ' ', COLOR_DEFAULT);
		}
	}

	if (esc_letter != 1) {
		for (int j = 0; j < 4; j++) {
			snprintf(arr[j], 100, arr[j + 1]);
		}
	}
	snprintf(arr[4], 100, H_sistem);

	for (int i = 0; i < 5; i++) {
		char buff[100];
		snprintf(buff, 100, arr[i]);
		POSITION pos = { MAP_HEIGHT + 3 + i, 2 };
		gotoxy(pos);
		set_color(COLOR_DEFAULT);
		printf("%s", buff);
	}
	esc_letter = 0;
}

void random_space_letter(int num) {
	if (num == 1)
		sistem_letter(sistem, random_space);
	else sistem_letter(sistem, eat_space);
}

//2)Ŀ�� & ����â
//�����̽��ٸ� �������� 
int apace_select;
void object_info_mark(CURSOR cursor) {
	POSITION curr = cursor.current;
	mark_esc();
	switch (backbuf[curr.row][curr.column])
	{
	case ' ':state_letter(place); break;
	case 'P':state_letter(Plate); break;
	case 'R':state_letter(Rock); break;
	case 'W':state_letter(Sand); break;
	case 'H':
		if (curr.row > MAP_HEIGHT / 2) {
		state_letter(harvester); command_letter(harvester_command); break;
		}
		else {
			state_letter(AI_harvester);
			break;
		}
	case 'B':
		if (curr.row > MAP_HEIGHT / 2) {
		state_letter(Base); command_letter(Base_command);
		apace_select = 1;
		/*else if (key == 'M' || key == 'm') {
			sistem_letter(M_sistem);
			int up_down = rand() % 12;
			if (up_down == 0) {
				h_move.column++;
				map[1][h_move.row][h_move.column] = SYMBOL_HARVESTER;
			}
			else {
				h_move.row--;
				map[1][h_move.row][h_move.column] = SYMBOL_HARVESTER;
			}
			}*/
		break;
		}
		else {
			state_letter(AI);
			break;
		}
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':state_letter(Space); break;
	default:
		break;
	}

}

//2) Ŀ�� & ����â
//esc �������� ����â ����, ���â�� ����
void mark_esc(void) {
	for (int i = 0; i < 45; i++) {
		for (int j = 0; j < 5; j++) {
			POSITION pos = { 3 + j, MAP_WIDTH + 3 + i };
			printc(pos, ' ', COLOR_DEFAULT);

			POSITION pos1 = { MAP_HEIGHT + 3 + j, MAP_WIDTH + 3 + i };
			printc(pos1, ' ', COLOR_DEFAULT);
		}
	}
}

/*=============4) ���� 1�� ���� ==========*/
// h�� ��������
// �Ϻ����� �߰��� �ʱ� ��ġ
POSITION h_move = { MAP_HEIGHT - 4,2 };
void h_push(CURSOR cursor, RESOURCE* resource){
	if ((*resource).spice - 5 >= 0) {
		if (apace_select == 1) {    //B �������� ���� �� h�� ��������
			sistem_letter(sistem, H_sistem_success);
			map[1][h_move.row][h_move.column] = SYMBOL_HARVESTER;
			apace_select = 0;
			(*resource).spice -= 5;
			(*resource).population += 5;
		}
	}
	else sistem_letter(sistem, H_sistem_failure);
}

// esc�� �������� ���� ��� , h �߰��� �Ϳ��� esc�� ������ �������鼭 �����̽� �α��� ����
void esc_choice(CURSOR cursor, RESOURCE* resource) { //esc ������ ��� ����� H �κп��� esc�� ���� ��� �ƿ� ����
	apace_select = 0;
	//sistem_esc_choice();
	POSITION curr = cursor.current;
	if (curr.row == h_move.row && curr.column == h_move.column) {
		map[1][h_move.row][h_move.column] = ' ';
		(*resource).spice += 5; // �����̽� +5
		(*resource).population -= 5; // �α��� -5
	}
}

//void sistem_esc_choice(void) { // �ý��� ������ ���� ����� ��
//	for (int i = 0; i < 45; i++) {
//		POSITION pos = { MAP_HEIGHT + 7, 2 + i };
//		printc(pos, ' ', COLOR_DEFAULT);
//		esc_letter = 1;
//	}
//}

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
				printc(padd(map_pos, pos), backbuf[i][j], get_color(backbuf[i][j],i));
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous; // ������ġ
	POSITION curr = cursor.current; // ������ġ

	char ch = frontbuf[prev.row][prev.column];//���� Ŀ���� ����� ��
	printc(padd(map_pos, prev), ch, get_color(backbuf[prev.row][prev.column], prev.row));
	ch = frontbuf[curr.row][curr.column];//���ο� ��ġ�� Ŀ���� ���°�
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

/*================3) �߸����� ===============*/

// ����ư �Ÿ� ��� �Լ�
int calculate_distance(POSITION pos1, POSITION pos2) {
	return abs(pos1.row - pos2.row) + abs(pos1.column - pos2.column);
}

// �ʿ��� ��� 'H'�� ��ġ�� ã�� �Լ�
int find_H_positions(POSITION H_positions[]) {
	int count = 0;
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (backbuf[i][j] == 'H') {
				H_positions[count].row = i;
				H_positions[count].column = j;
				count++;
			}
		}
	}
	return count; // ã�� 'H'�� ���� ��ȯ
}

POSITION  SANDWORM_find(POSITION worm_position) {
	POSITION H_positions[100]; // �ִ� 100���� H ��ġ ���� ����
	int num_H = find_H_positions(H_positions); // H�� ��ġ ã��

	if (num_H == 0) {
		// H�� ������ �⺻�� ��ȯ
		return (POSITION) { -1, -1 };
	}

	int min_distance = 1000;
	POSITION closest_h = { -1, -1 };

	// ��� H���� �Ÿ��� ����Ͽ� ���� ����� H�� ã��
	for (int i = 0; i < num_H; i++) {
		int distance = calculate_distance(worm_position, H_positions[i]);
		if (distance < min_distance) {
			min_distance = distance;
			closest_h = H_positions[i];
		}
	}
	return closest_h;
} 

