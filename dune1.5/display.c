/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include <time.h>
#include <conio.h>
#include <string.h>
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
int get_color(char backbuf, int row, int col);
void mark_esc(void);
void state_letter(char arr[][100]);
void command_letter(char arr[][100]);
void object_info_mark(CURSOR cursor, RESOURCE* resource);
void h_push(CURSOR cursor, RESOURCE* resource);
void sistem_letter(char arr[][100], char H_sistem[1][100]);
void esc_choice(CURSOR cursor, RESOURCE* resource);
void B_push(void);
void P_push(void);
void D_push(void);
void G_push(void);
OBJECT_SAMPLE* M_push(CURSOR cursor);
void S_push(CURSOR cursor, RESOURCE* resource);
void Barracks_push(void);
void Building(CURSOR cursor, RESOURCE* resource, BUILD* name, char H_sistem[1][100], char state[5][100]);
void push_building(BUILD_s buildings[], POSITION pos, int durability);
void push_units(OBJECT_SAMPLE units[], POSITION pos, int strength, UNIT unit);
void space_store(int num);
int find_H_positions(POSITION H_positions[]);
POSITION  SANDWORM_find(POSITION worm_position);
void esc_state(void);
void H_number(int num);
POSITION produce_position(CURSOR cursor);
int find_space_positions(POSITION num_positions[]);
POSITION  space_find(POSITION space_position);
//void sistem_esc_choice(void);

// =================== �ǹ� ����ü ====================
//����
BUILD P = {
	.symbol = 'P', // ȭ�� ǥ��
	.spice_cost = 1, // �Ǽ����
	.population_increase = 0, // �α� �ִ�ġ ����
	.spice_increase = 0,//�����̽� ���� �ִ�ġ ����

};

BUILD D = {
	.symbol = 'D', // ȭ�� ǥ��
	.spice_cost = 2, // �Ǽ����
	.population_increase = 10, // �α� �ִ�ġ ����
	.spice_increase = 0,//�����̽� ���� �ִ�ġ ����
};

BUILD G = {
	.symbol = 'G', // ȭ�� ǥ��
	.spice_cost = 4, // �Ǽ����
	.population_increase = 0, // �α� �ִ�ġ ����
	.spice_increase = 10,//�����̽� ���� �ִ�ġ ����
};

// �÷��̾�
BUILD B = {
	.symbol = 'b', // ȭ�� ǥ��
	.spice_cost = 4, // �Ǽ����
	.population_increase = 0, // �α� �ִ�ġ ����
	.spice_increase = 0,//�����̽� ���� �ִ�ġ ����
};

BUILD S = {
	.symbol = 'S', // ȭ�� ǥ��
	.spice_cost = 5, // �Ǽ����
	.population_increase = 0, // �α� �ִ�ġ ����
	.spice_increase = 0,//�����̽� ���� �ִ�ġ ����
};

// ���ڳ�
BUILD A = {
	.symbol = 'A', // ȭ�� ǥ��
	.spice_cost = 3, // �Ǽ����
	.population_increase = 0, // �α� �ִ�ġ ����
	.spice_increase = 0,//�����̽� ���� �ִ�ġ ����
};

BUILD F = {
	.symbol = 'F', // ȭ�� ǥ��
	.spice_cost = 5, // �Ǽ����
	.population_increase = 0, // �α� �ִ�ġ ����
	.spice_increase = 0,//�����̽� ���� �ִ�ġ ����
};

//// ================= ���� ����ü ======================

UNIT Harvest = { //�Ϻ�����
	.symbol = 'H',// ȭ�� ǥ��
	.spice_cost = 5, // ������
	.population = 5, // ���� �α� �� ����
	.move_period = 2000, // �̵��ֱ�
	.aggressive_strength = 0, // ���ݷ�
	.attack = 0,// �����ֱ�
	.view = 0// �þ�
};

UNIT Fremen = { //������
	.symbol = 'f',// ȭ�� ǥ��
	.spice_cost = 5, // ������
	.population = 2, // ���� �α� �� ����
	.move_period = 400, // �̵��ֱ�
	.aggressive_strength = 15, // ���ݷ�
	.attack = 200,// �����ֱ�
	.view = 8// �þ�
};

UNIT Soldier = { //����
	.symbol = 's',// ȭ�� ǥ��
	.spice_cost = 1, // ������
	.population = 1, // ���� �α� �� ����
	.move_period = 1000, // �̵��ֱ�
	.aggressive_strength = 5, // ���ݷ�
	.attack = 800,// �����ֱ�
	.view = 1// �þ�
};

UNIT Fighter = { // ����
	.symbol = 'f',// ȭ�� ǥ��
	.spice_cost = 1, // ������
	.population = 1, // ���� �α� �� ����
	.move_period = 1200, // �̵��ֱ�
	.aggressive_strength = 6, // ���ݷ�
	.attack = 600,// �����ֱ�
	.view = 1// �þ�
};

UNIT heavy_Tank = { // ������
	.symbol = 'T',// ȭ�� ǥ��
	.spice_cost =12, // ������
	.population = 5, // ���� �α� �� ����
	.move_period = 3000, // �̵��ֱ�
	.aggressive_strength = 40, // ���ݷ�
	.attack = 4000,// �����ֱ�
	.view = 4// �þ�
};

//================ ���� ����ü �迭 ���� ==============
OBJECT_SAMPLE H_units[20] = { 0 };
OBJECT_SAMPLE s_units[20] = { 0 };
OBJECT_SAMPLE Fremen_units[20] = { 0 };
OBJECT_SAMPLE Fighter_units[20] = { 0 };
OBJECT_SAMPLE T_units[20] = { 0 };

void push_units(OBJECT_SAMPLE units[], POSITION pos, int strength, UNIT unit) {
	for (int i = 0; i < 10; i++) {
		if (units[i].exist == 0) {
			units[i].exist = 1; // �迭�� ������ ����
			units[i].pos = pos; // ��ġ
			units[i].dest = pos;
			units[i].strength = strength; // ü��
			units[i].repr = unit.symbol;
			units[i].speed = unit.move_period;
			return;
		}
	}
}

void initial_H(void) { // �ʱ� H ��ġ ����
	POSITION pos = { MAP_HEIGHT - 4 , 1 };
	push_units(H_units, pos, 70, Harvest);
}

void reset(POSITION dest) {
	for (int i = 0; i < 10; i++) {
		if (H_units[i].pos.row == dest.row &&
			H_units[i].pos.column == dest.column) {
			// H ���� �ʱ�ȭ
			H_units[i].pos.row = 0;
			H_units[i].pos.column = 0;
			H_units[i].dest.row = 0;
			H_units[i].dest.column = 0;
			H_units[i].repr = 0;
			H_units[i].speed = 0;
			H_units[i].next_move_time = 0;
			break;
		}
	}
}

//================ ���� ����ü �迭 ���� ==============
BUILD_s P_buildings[10] = { 0 };
BUILD_s D_buildings[10] = { 0 }; 
BUILD_s G_buildings[10] = { 0 }; 
BUILD_s B_buildings[10] = { 0 }; 
BUILD_s S_buildings[10] = { 0 }; 
BUILD_s A_buildings[10] = { 0 }; 
BUILD_s F_buildings[10] = { 0 };

void push_building(BUILD_s buildings[], POSITION pos, int durability) {
	for (int i = 0; i < 10; i++) {
		if (buildings[i].exist == 0) {
			buildings[i].exist = 1; // �迭�� ������ ����
			buildings[i].pos = pos; // ��ġ
			buildings[i].durability = durability; // ������
			return;
		}
	}
}

//============ �ʱ� ���� =============/
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
	printf("spice = %d/%d, population=%d/%d \n",
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
int get_color(char backbuf, int row, int col) { 
	switch (backbuf) {
	case 'B':
		if (row > MAP_HEIGHT / 2) {
		return 159; 
	}
		else {
		return 79; 
	}
	case 'H':
		for (int i = 0; i < 10; i++) {
			if (H_units[i].pos.row == row && H_units[i].pos.column == col 
				|| (row == MAP_HEIGHT - 4 && col == 1)) {
				return 159;
			}
		}
		return 79;
	case 'W': return 111; //������ Ȳ����
	case 'P': return 14; //���� ������
	case 'R': return 112; 
	case 'D': return 176; 
	case 'G': return 95;  // ������� ����
	case 'b': return 59; 
	case 'S': return 31; 
	case 's': return 177;
	case 'F': return 4;
	case 'A':return 108;
	case 'T':return 78;
	case 'f':
		for(int i = 0; i < 10; i++) {
		if (Fighter_units[i].pos.row == row && Fighter_units[i].pos.column == col) {
			return 159;
		}
	} return 89;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':return 175; 
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
char place[5][100] = { "���¾�~ �縷" , "�⺻ �����̸�,", "�ǹ��� ���� �� ����", "", " " };
char Rock[5][100] = { "���¾�~ ����", "�����̸�," ,"�������� ����� �� ����", " "," " };

char Base[5][100] = { "���¾�~ ���� ��Ʈ���̵�(�÷��̾�)" , "�Ϻ����͸� ������ �� �־�", " ", " ", " " };
char Base_command[5][100] = { "��ɾ� : �Ϻ����� ����(H : Harvest)","�Ǽ���� :  ����", "������ : 50"," "};

char Build_base[5][100] = { "B : �ǹ� ���"," ", " "," "," " };
char Build_list[5][100] = { "����(P: Plate)","����(D: Dormitory)", "â��(G: Garage)","����(B: Barracks)","����ó(S: Shelter)" };

char Plate[5][100] = { "���¾�~ ����", "�����̸�," ,"���� �ǹ��� ���� �� �־�", "�ǹ� ���� ���� ��� "," " };
char Plate_account[5][100] = { "����(P: Plate)","�ǹ� ���� ���� ���","�Ǽ���� : 1","������: ����"," " };
char Plate_creation[1][100] = { "[����]�� �����Ǿ����ϴ�." };
char not_Plate[1][100] = { "[����]�� ��� ������ �� �����ϴ�." };

char Dormitory_creation[1][100] = { "[����]�� �����Ǿ����ϴ�." };
char Dormitory_account[5][100] = {"�����̽��� ���� �����ϼ���", "����(D: Dormitory)","�α� �ִ�ġ ����(10)","�Ǽ���� : 2","������: 10" };
char Dormitory1_account[5][100] = { "����(D: Dormitory)","�α� �ִ�ġ ����(10)","�Ǽ���� : 2","������: 10"," "};

char Garage_creation[1][100] = { "[â��]�� �����Ǿ����ϴ�." };
char Garage_account[5][100] = { "�����̽��� ���� �����ϼ���","â��(G: Garage) ","�����̽� ���� �ִ�ġ ����(10)","�Ǽ���� : 4","������: 10" };
char Garage1_account[5][100] = { "â��(G: Garage) ","�����̽� ���� �ִ�ġ ����(10)","�Ǽ���� : 4","������: 10"," "};

char Barracks_creation[1][100] = { "[����]�� �����Ǿ����ϴ�." };
char Barracks_account[5][100] = {"�����̽��� ���� �����ϼ���","����(b: Barracks) ","�Ǽ���� : 4","������: 20"};
char Barracks1_account[5][100] = { "���¾�~ ����(b: Barracks) ","���� �����ϸ�,","�Ǽ���� : 4","������: 20"," "};

char Barracks_command[5][100] = { "��ɾ� : ���� ����(S: Soldier)"," ", " "," "," " };
char Soldier_success[1][100] = { "���ο� [����]�� �����Ǿ����ϴ�." };
char Soldier_failure[1][100] = { "�����̽��� ��� [����] ������ �ȵ˴ϴ�." };

char Shelter_creation[1][100] = { "[����ó]�� �����Ǿ����ϴ�." };
char Shelter_account[5][100] = { "�����̽��� ���� �����ϼ���","����ó(S: Shelter) ","Ư������ ����","�Ǽ���� : 5","������ : 30" };
char Shelter1_account[5][100] = { "���¾�~ ����ó(S: Shelter) ","Ư������ �����ϸ�, ","�Ǽ���� : 5","������ : 30", " "};
char Shelter_command[5][100] = { "��ɾ� : ������ ����(F: Fremen) "," ", " "," "," " };

char Fremen_success[1][100] = { "���ο� [������]�� �����Ǿ����ϴ�." };
char Fremen_failure[1][100] = { "�����̽��� ��� [������] ������ �ȵ˴ϴ�." };
char Fremen_account[5][100] = { "���¾�~ ������ ","������: 5, �α��� : 2"," �̵��ֱ� : 2000","���ݷ�: 15, �����ֱ�:200", "���� ü�� : 25" };

char harvester[5][100] = { "", "������: 5, �α��� : 5" ,"�̵��ֱ�:2000", "���ݷ�: ����, �����ֱ�: ����","���� ü�� : 70" };
char harvester_command[5][100] = { "H: Harvest(��Ȯ)", " "," "," "," " };
char harvester2_command[5][100] = { "���ϴ� �����̽��� Ŀ���� �̵� �� ", "��ɾ ��������","M : move(�̵�)"," "," " };
char H_sistem_success[1][100] = { "���ο� [�Ϻ�����]�� �����Ǿ����ϴ�." };
char H_sistem_push[1][100] = { "[�Ϻ�����]�� ���õǾ����ϴ�." };
char H_sistem_failure[1][100] = { "�����̽��� ��� [�Ϻ�����]�� ������ �ȵ˴ϴ�." };
char M_sistem[1][100] = { "[�Ϻ�����]�� �����Դϴ�." };

char Sand[5][100] = { "���¾�~ ������", "õõ�� �����ϰ�" ,"������ �Ϲ� ������ ������ �� �Ծ�����ž�", "���� �輳�� ��(���� �ֱ�� ���差�� ���)"," " };
char random_space[1][100] = { "�������� �����̽��� �����߽��ϴ�." };
char eat_H[1][100] = { "[������]�� [�Ϻ�����]�� �Ծ����ϴ�." };

char sistem[5][100] = { " "," ", " ", " "," " };

char Space[5][100] = { "���¾�~ �����̽�", "" ,"�⺻�� 2��", "�������� �����"," " };
char eat_space[1][100] = { "[�����̽�]�� �Ծ����ϴ�."};
char Space2[1][100] = { "[�����̽�] ��������� �� á���ϴ�." };


//=====���ڳ�==========
char AI[5][100] = { "���¾�~ ���ڳ�(AI)" , "����� ���̾�", " ", " ", " " };
char AI_harvester[5][100] = { "���¾�~ AI �Ϻ�����", " ", " ", " "," " };
char Arena[5][100]= { "���� ������", "���� ����(f:fighter)�����ϸ� " ,"�Ǽ���� : 3", "������:15"," " };
char factory[5][100] = { "���� ����", "Ư�������� ����(T:hevy Tank)�ϸ� " ,"�Ǽ���� : 5", "������:30"," " };
char fighter[5][100]= { "���� ����(f:fighter)", "������: 1, �α��� : 1" ,"�̵��ֱ�:1200", "���ݷ�: 6, �����ֱ�: 600","���� ü�� : 10" };
char Tank[5][100] = { "���� ����(T:hevy Tank)", "������: 12, �α��� : 5" ,"�̵��ֱ�:3000", "���ݷ�: 40, �����ֱ�: 4000","���� ü�� : 60" };

void space_store(int num) { // �� ���差 ǥ��
	char buff[100];
	snprintf(buff, sizeof(buff),"���差 : %d",num);
	POSITION pos = { 4, MAP_WIDTH + 2 };
	gotoxy(pos);
	set_color(COLOR_DEFAULT);
	printf(" %s", buff);
}

void H_number(int num) { // �� ���差 ǥ��
	char buff[100];
	snprintf(buff, sizeof(buff), "���¾� �Ϻ����� �迭 �ѹ�: %d��", num);
	POSITION pos = { 3, MAP_WIDTH + 2 };
	gotoxy(pos);
	set_color(COLOR_DEFAULT);
	printf(" %s", buff);
}

//5) �ý��� �޽��� �޽��� �α� ��ũ�� �ø���
int num = 0;
int esc_letter = 0; //esc�� ������ ���� �������� ����

void sistem_letter(char arr[][100],char H_sistem[1][100]) {
	for (int y = 0; y < 45; y++) {
		for (int z = 0; z < 7; z++) {
			POSITION pos1 = { MAP_HEIGHT + 3 + z, 2 + y };
			printc(pos1, ' ', COLOR_DEFAULT);
		}
	}
	if (esc_letter != 1) {
		for (int j = 0; j < 6; j++) {
			snprintf(arr[j], 100, arr[j + 1]);
		}
	}
	snprintf(arr[6], 100, H_sistem);
	for (int i = 0; i < 7; i++) {
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
	switch (num)
	{
	case 1: sistem_letter(sistem, random_space); break;
	case 2: sistem_letter(sistem, eat_H); break;
	case 3: sistem_letter(sistem, eat_space); break;
	default:
		break;
	}
}


int apace_select;
int Build_H_select = 0; // �Ϻ����Ϳ��� �����̽��� ��������
/*=============4) ���� 1�� ���� ==========*/
// h�� ��������
// �Ϻ����� �߰��� �ʱ� ��ġ

void h_push(CURSOR cursor, RESOURCE* resource) {
	POSITION curr = cursor.current;
	POSITION h_move = { MAP_HEIGHT - 4,2 };
	if (Build_H_select==1 && backbuf[curr.row][curr.column] == 'H') {
		Build_H_select += 1;
		command_letter(harvester2_command);
		sistem_letter(sistem, H_sistem_push);
	}
	else {
		if ((*resource).spice - 5 >= 0) {
			if (apace_select == 1 && map[0][curr.row][curr.column] == 'B') {    //B �������� ���� �� h�� ��������
				push_units(H_units, h_move, 70, Harvest);
				sistem_letter(sistem, H_sistem_success);
				map[1][h_move.row][h_move.column] = Harvest.symbol;
				apace_select = 0;
				(*resource).spice -= Harvest.spice_cost;
				(*resource).population += Harvest.population;
			}
		}
		else sistem_letter(sistem, H_sistem_failure);
	}
}


//6) �Ǽ�
int Build_select = 0;
void B_push(void) { // B�� ��������
	if (Build_select == 0) {
		command_letter(Build_list);
		Build_select = 1;
	}
	else {
		Barracks_push();
	}
}

void P_push(void) { // ���� P ��������
	if (Build_select == 1) {
		for (int i = 0; i < 45; i++) {
			for (int j = 0; j < 5; j++) {
				POSITION pos1 = { MAP_HEIGHT + 3 + j, MAP_WIDTH + 3 + i };
				printc(pos1, ' ', COLOR_DEFAULT);
			}
		}
		command_letter(Plate_account);
		esc_state();
		Build_select = 2;
	}

}
void D_push(void) { // ���� D ��������
	if (Build_select == 1) {
		for (int i = 0; i < 45; i++) {
			for (int j = 0; j < 5; j++) {
				POSITION pos1 = { MAP_HEIGHT + 3 + j, MAP_WIDTH + 3 + i };
				printc(pos1, ' ', COLOR_DEFAULT);
			}
		}
		command_letter(Dormitory_account);
		esc_state();
		Build_select = 3;
	}
}

void G_push(void) { // â�� G ��������
	if (Build_select == 1) {
		for (int i = 0; i < 45; i++) {
			for (int j = 0; j < 5; j++) {
				POSITION pos1 = { MAP_HEIGHT + 3 + j, MAP_WIDTH + 3 + i };
				printc(pos1, ' ', COLOR_DEFAULT);
			}
		}
		command_letter(Garage_account);
		esc_state();
		Build_select = 4;
	}
}

int Build_B_select = 0;// �������� �����̽��� ��������
int Build_S_select = 0;//����ó���� �����̽��� ��������

void Barracks_push(void) { // ���� B ��������
	if (Build_select == 1) {
		for (int i = 0; i < 45; i++) {
			for (int j = 0; j < 5; j++) {
				POSITION pos1 = { MAP_HEIGHT + 3 + j, MAP_WIDTH + 3 + i };
				printc(pos1, ' ', COLOR_DEFAULT);
			}
		}
		command_letter(Barracks_account);
		esc_state();
		Build_select = 5;
	}
}


void S_push(CURSOR cursor, RESOURCE* resource) { // ����ó S ��������
	if (Build_select == 1 && Build_B_select == 0) {
		command_letter(Shelter_account);
		esc_state();
		Build_select = 6;
	}
	else if (Build_B_select == 1) {
		if ((*resource).spice - 5 >= 0) {
			POSITION produce = produce_position(cursor);
			sistem_letter(sistem, Soldier_success);
			map[1][produce.row][produce.column] = Soldier.symbol;
			Build_B_select = 0;
			(*resource).spice -= Soldier.spice_cost;
			(*resource).population += Soldier.population;
			push_units(s_units, produce, 15, Soldier);
		}
		else sistem_letter(sistem, Soldier_failure);
		
	}
}

void F_push(CURSOR cursor, RESOURCE* resource) {
	POSITION curr = cursor.current;
	if (Build_S_select == 1) {
		POSITION produce = produce_position(cursor);//�������� ��ġ ����
		if ((*resource).spice - 1 >= 0&& (produce.row != curr.row || produce.column != curr.column)) {
			sistem_letter(sistem, Fremen_success);
			map[1][produce.row][produce.column] = Fremen.symbol;
			Build_B_select = 0;
			(*resource).spice -= Fremen.spice_cost;
			(*resource).population += Fremen.population;
			push_units(Fremen_units, produce, 25, Fremen);
		}
		else sistem_letter(sistem, Fremen_failure);
	}
}

int H_num; // ����ü �迭 ��ġ
OBJECT_SAMPLE* M_push(CURSOR cursor) {
	POSITION curr = cursor.current;
	if (Build_H_select == 2 && backbuf[curr.row][curr.column] >= '1' && backbuf[curr.row][curr.column] <= '9') {
		H_units[H_num].dest = curr;
		sistem_letter(sistem, M_sistem);
		mark_esc();
		Build_H_select = 0;
		return &H_units[H_num];
	}
	return NULL; // ������ �������� ���� �� NULL ��ȯ
}

POSITION produce_position(CURSOR cursor) {
	POSITION curr = cursor.current;
	POSITION empty_positions[4];
	int empty_count = 0;

	// 2x2 ���� ���� �� ���� ã��
	int offsets[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} }; // ��, ��, ��, ��
	for (int i = 0; i < 4; i++) {
		POSITION adj_pos = {
			curr.row + offsets[i][0],
			curr.column + offsets[i][1]
		};
		if (backbuf[adj_pos.row][adj_pos.column] == ' ') {
			empty_positions[empty_count++] = adj_pos;
		}
	}

	// �� ������ ���� ���
	if (empty_count == 0) {
		return curr; // ���� ��ġ ��ȯ �Ǵ� ���� ó��
	}

	// �� ������ 1���� ���
	if (empty_count == 1) {
		return empty_positions[0];
	}

	// �� ������ 2�� �̻��� ��� �������� ����
	return empty_positions[rand() % empty_count];
}

void Building(CURSOR cursor, RESOURCE* resource, BUILD* name, char H_sistem[1][100], char state[5][100]) {
	if (resource->spice >= name->spice_cost) {
		POSITION curr = cursor.current;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				POSITION pos = { curr.row + i, curr.column + j };
				map[0][pos.row][pos.column] = (*name).symbol;
			}
		}
		(*resource).spice -= (*name).spice_cost;
		(*resource).spice_max += (*name).spice_increase;
		(*resource).population_max += (*name).population_increase;
		sistem_letter(sistem, H_sistem);
		state_letter(state);
		switch ((*name).symbol)
		{
		case 'P': push_building(P_buildings, curr,0); break;
		case 'D': push_building(D_buildings, curr,10); break;
		case 'G': push_building(G_buildings, curr,10); break;
		case 'B': push_building(B_buildings, curr,20); break;
		case 'S': push_building(S_buildings, curr,30); break;
		case 'A': push_building(A_buildings, curr, 15); break;
		case 'F': push_building(F_buildings, curr, 30); break;
		default:
			break;
		}
		Build_select = 0;
	}
}


//2)Ŀ�� & ����â
//�����̽��ٸ� �������� 
void object_info_mark(CURSOR cursor, RESOURCE* resource) {
	POSITION curr = cursor.current;
	mark_esc();
	if (Build_select == 2 && backbuf[curr.row][curr.column] == ' ') {
		Building(cursor, resource, &P, Plate_creation, Plate);
	}
	else if (backbuf[curr.row][curr.column] == 'P' &&
		backbuf[curr.row + 1][curr.column] == 'P' &&
		backbuf[curr.row][curr.column + 1] == 'P' &&
		backbuf[curr.row + 1][curr.column + 1] == 'P') {
		switch (Build_select) {
		case 3: // ���� �Ǽ� : D
			Building(cursor, resource, &D, Dormitory_creation, Dormitory1_account);
			break;
		case 4: // â�� �Ǽ� : G
			Building(cursor, resource, &G, Garage_creation, Garage1_account);
			break;
		case 5: // ���� �Ǽ� : B
			Building(cursor, resource, &B, Barracks_creation, Barracks1_account);
			break;
		case 6: // ����ó : S
			Building(cursor, resource, &S, Shelter_creation, Shelter1_account);
			break;
		}
	}
	else if(Build_select >1 && backbuf[curr.row][curr.column] != 'P' &&
		backbuf[curr.row + 1][curr.column] != 'P' &&
		backbuf[curr.row][curr.column + 1] != 'P' &&
		backbuf[curr.row + 1][curr.column + 1] != 'P'){
		sistem_letter(sistem, not_Plate);
		switch (Build_select){
		case 3: // ���� �Ǽ� : D
			command_letter(Dormitory_account);
			break;
		case 4: // â�� �Ǽ� : G
			command_letter(Garage_account);
			break;
		case 5: // ���� �Ǽ� : B
			command_letter(Barracks_account);
			break;
		case 6: // ����ó : S
			command_letter(Shelter_account);
			break;
		}
	}
	else {
		switch (backbuf[curr.row][curr.column])
		{
		case ' ':state_letter(place); break;
		case 'P':state_letter(Plate); break;
		case 'R':state_letter(Rock); break;
		case 'W':state_letter(Sand); break;
		case 'H':
			for (int i = 0; i < 10; i++) {
				if (H_units[i].pos.row == curr.row
					&& H_units[i].pos.column == curr.column) {
					H_num = i;
					state_letter(harvester); H_number(H_num); command_letter(harvester_command);
					Build_H_select = 1;
					break;
				}
				else {
					state_letter(AI_harvester);
				}
			}
			break;
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
		case 'D':state_letter(Dormitory_account); break;
		case 'G':state_letter(Garage1_account); break;
		case 'b': state_letter(Barracks1_account); command_letter(Barracks_command); Build_B_select = 1; break;//����
		case 'S': state_letter(Shelter1_account); command_letter(Shelter_command); Build_S_select = 1; break;//����ó
		case 'F':state_letter(Garage1_account); break;//����
		case 'A':state_letter(Garage1_account); break;//������
		case 'T':state_letter(Garage1_account); break;//������
		case 'f':
			for (int i = 0; i < 10; i++) {
				if (Fremen_units[i].pos.row == curr.row
					&& Fremen_units[i].pos.column == curr.column) {
					num = i;
					state_letter(harvester); command_letter(harvester_command);
					Build_H_select = 1;
					break;
				}
				else {
					state_letter(AI_harvester); //����
					break;
				}
			}
		 break;
		case '1':space_store(1); state_letter(Space); break;
		case '2':space_store(2); state_letter(Space); break;
		case '3':space_store(3); state_letter(Space); break;
		case '4':space_store(4); state_letter(Space); break;
		case '5':space_store(5); state_letter(Space); break;
		case '6':space_store(6); state_letter(Space); break;
		case '7':space_store(7); state_letter(Space); break;
		case '8':space_store(8); state_letter(Space); break;
		case '9':space_store(9); state_letter(Space); break;
		default:
			break;
		}
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
			command_letter(Build_base);
		}
	}
}

void esc_state(void) {
	for (int i = 0; i < 45; i++) {
		for (int j = 0; j < 5; j++) {
			POSITION pos = { 3 + j, MAP_WIDTH + 3 + i };
			printc(pos, ' ', COLOR_DEFAULT);
		}
	}
}

// esc�� �������� ���� ��� , h �߰��� �Ϳ��� esc�� ������ �������鼭 �����̽� �α��� ����
void esc_choice(CURSOR cursor, RESOURCE* resource) { //esc ������ ��� ����� H �κп��� esc�� ���� ��� �ƿ� ����
	apace_select = 0; // H ���� ���
	Build_H_select = 0; // H ��Ȯ ���
	Build_S_select = 0;
	POSITION curr = cursor.current;
	if (Build_select >= 2) {
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				POSITION pos = { curr.row + i, curr.column + j };
				char ch = frontbuf[pos.row][pos.column];
				printc(padd(map_pos, pos), ch, get_color(backbuf[pos.row][pos.column], i,j));
			}
		}
	}
	Build_select = 0; // B ������ ���� ���
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
				printc(padd(map_pos, pos), backbuf[i][j], get_color(backbuf[i][j],i,j));
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous; // ������ġ
	POSITION curr = cursor.current; // ������ġ

	// ���� Ŀ�� ��ġ �����
	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, get_color(backbuf[prev.row][prev.column], prev.row, prev.column));

	if (Build_select >= 2) {
		// 4x4 ���� ó��
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				POSITION pos = { prev.row + i, prev.column + j };
				ch = frontbuf[pos.row][pos.column];
				printc(padd(map_pos, pos), ch, get_color(backbuf[pos.row][pos.column], pos.row, pos.column));
			}
		}

		// �� Ŀ�� ��ġ�� 4x4 ���� �׸���
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				POSITION pos = { curr.row + i, curr.column + j };
				ch = frontbuf[pos.row][pos.column];
				printc(padd(map_pos, pos), ch, COLOR_CURSOR);
			}
		}
	}
	else {
		// ���� Ŀ�� ��ġ �׸���
		ch = frontbuf[curr.row][curr.column];
		printc(padd(map_pos, curr), ch, COLOR_CURSOR);
	}
}

/*================3) �߸����� ===============*/

// ����ư �Ÿ� ��� �Լ�
int calculate_distance(POSITION pos1, POSITION pos2) {
	return abs(pos1.row - pos2.row) + abs(pos1.column - pos2.column);
}

// �ʿ��� ��� ����,�����̽� ��ġ�� ã�� �Լ�
int find_H_positions(POSITION H_positions[]) {
	int count = 0;
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (backbuf[i][j] == 'H'|| backbuf[i][j] == 'f'||
				backbuf[i][j] == 's' || backbuf[i][j] == 'T') {
				H_positions[count].row = i;
				H_positions[count].column = j;
				count++;
			}
		}
	}
	return count; // ã�� ���� ��ȯ
}

POSITION  SANDWORM_find(POSITION worm_position) {
	POSITION H_positions[50]; //H ��ġ����
	int num_H = find_H_positions(H_positions); // H ��ġ ã��

	if (num_H == 0) {
		// ������ �⺻�� ��ȯ
		return (POSITION) { -1, -1 };
	}

	int min_distance = 1000;
	POSITION closest_h = { -1, -1 };

	// ��� H���� �Ÿ��� ����Ͽ� ���� ����� ������ ã��
	for (int i = 0; i < num_H; i++) {
		int distance = calculate_distance(worm_position, H_positions[i]);
		if (distance < min_distance) {
			min_distance = distance;
			closest_h = H_positions[i];
		}
	}
	return closest_h;
}


int find_space_positions(POSITION num_positions[]) {
	int count = 0;
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (backbuf[i][j] >= '1'&& backbuf[i][j] <= '9') {
				num_positions[count].row = i;
				num_positions[count].column = j;
				count++;
			}
		}
	}
	return count; // ã�� ���� ��ȯ
}


POSITION  space_find(POSITION space_position) {
	POSITION space_positions[50]; //�����̽� ��ġ
	int num_H = find_space_positions(space_positions); // �����̽� ��ġ

	if (num_H == 0) {
		// ������ �⺻�� ��ȯ
		if (backbuf[MAP_HEIGHT - 4][1] == ' ') {
			return (POSITION) { MAP_HEIGHT - 4, 1 };
		}
		else {
			return (POSITION) { MAP_HEIGHT - 4, 2 };
		}
	}

	int min_distance = 1000;
	POSITION closest_h = { -1, -1 };

	// ��� H���� �Ÿ��� ����Ͽ� ���� ����� ������ ã��
	for (int i = 0; i < num_H; i++) {
		int distance = calculate_distance(space_position, space_positions[i]);
		if (distance < min_distance) {
			min_distance = distance;
			closest_h = space_positions[i];
		}
	}
	return closest_h;
}