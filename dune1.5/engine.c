#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void Initial_State(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir, int move_distance);
POSITION sample_obj_next_position(OBJECT_SAMPLE* name);
POSITION  SANDWORM_find(POSITION worm_position);
void sample_obj_move(OBJECT_SAMPLE* name);
void cursor_move(DIRECTION dir, int distance);
void sandworm_move();
char over_pay(OBJECT_SAMPLE* name, char ch);
void H_obj_move(OBJECT_SAMPLE* name);
POSITION H_obj_next_position(OBJECT_SAMPLE* name);
void P_obj_move(OBJECT_SAMPLE* name);
POSITION P_obj_next_position(OBJECT_SAMPLE* name);
static clock_t last_key_time = 0;
OBJECT_SAMPLE* ai_H_move(int num);

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 10,
	.spice_max = 30,
	.population = 10, 
	.population_max = 30
};

RESOURCE AI_resource = { // ���ڳ� �ڿ� ����ü
	.spice = 10,
	.spice_max = 30,
	.population = 10,
	.population_max = 30
};

// 3)�縷 ������: �������� ���ƴٴϴ� �߸�����
OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'r',
	.speed = 300,
	.next_move_time = 0
};

//3) ���� �Ϻ����ͷ� ���� ������
OBJECT_SAMPLE sandworm1 = {
	.pos = {2, MAP_WIDTH / 5},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.speed = 2500,
	.next_move_time = 2500
};

//3) ���ڳ� �Ϻ����ͷ� ���� ������
OBJECT_SAMPLE sandworm2 = {
	.pos = {MAP_HEIGHT - 8, 45},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.speed = 2500,
	.next_move_time = 2500
};

BUILD P_ai = { //ai ����
	.symbol = 'P', // ȭ�� ǥ��
	.spice_cost = 1, // �Ǽ����
	.population_increase = 0, // �α� �ִ�ġ ����
	.spice_increase = 0,//�����̽� ���� �ִ�ġ ����
	.ai = 1
};

// ���ڳ�
BUILD A = { // ������
	.symbol = 'A', // ȭ�� ǥ��
	.spice_cost = 3, // �Ǽ����
	.population_increase = 0, // �α� �ִ�ġ ����
	.spice_increase = 0,//�����̽� ���� �ִ�ġ ����
	.durability = 15,
	.ai = 1//ai
};

BUILD F = {
	.symbol = 'F', // ȭ�� ǥ��
	.spice_cost = 5, // �Ǽ����
	.population_increase = 0, // �α� �ִ�ġ ����
	.spice_increase = 0,//�����̽� ���� �ִ�ġ ����
	.durability = 30,
	.ai = 1//ai
};

bool is_double_click = false;
int sandworm_next_move_time = 1000;
OBJECT_SAMPLE* H_ptr[40] = { 0 };
OBJECT_SAMPLE* M_ptr[40] = { 0 }; 
OBJECT_SAMPLE H_ai_units[20] = { 0 };
extern UNIT Harvest;


int num1 = 0;
int num2 = 0;
int a = 0; int f =0;
/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));
	init();
	//intro();
	Initial_State();
	display_system_message();
	display_object_info();
	display_commands();
	display(resource, map, cursor);
	initial_H(); // �ʱ� H ��ġ ����
	initial_ai_H();// ai �ʱ� H ��ġ ����
	initial_p();
	int H_move = 0;
	KEY last_key = 0;
	int ai_H_num = 0; // H�� � �ִ���

	//2) Ŀ�� & ����â
	ai_H_move(0);


	while (1) {
		// loop �� ������(��, TICK==10ms����) Ű �Է� Ȯ��
		KEY key = get_key();
		clock_t current_time = clock();
		// Ű �Է��� ������ ó��
		if (is_arrow_key(key)) {
			double time_diff = (double)(current_time - last_key_time) / CLOCKS_PER_SEC;
			if (key == last_key && time_diff < 0.3 && !is_double_click) {
				// �� ��° Ŭ���� ������ �̷���� ��� // �� �� Ŭ�� �ߺ� ����
				is_double_click = true;
				cursor_move(ktod(key), 5); // 5ĭ �̵�
			}
			else {
				// ù ��° Ŭ���̰ų� ������ ���� ���
				is_double_click = false;
				cursor_move(ktod(key), 1); // 1ĭ �̵�

			}
			last_key = key;
			last_key_time = current_time;
		}
		else {
			// ����Ű ���� �Է�
			switch (key) {
			case k_quit: outro();
			case k_space:object_info_mark(cursor, &resource);  break;
			case k_esc:mark_esc();  esc_choice(cursor, &resource); break;
			case k_Hd: h_push(cursor, &resource); break;
			case K_Bd: B_push(); break;
			case K_Pd:  P_push(); M_ptr[num2] = P_unit_push(cursor); num2++; break;
			case K_Dd: D_push(); break;
			case K_Gd: G_push(); break;
			case K_Sd: S_push(cursor, &resource); break;
			case K_Md: H_ptr[num1] = M_push(cursor); num1++; break;
			case K_Fd: F_push(cursor, &resource);
				break;
			case k_none:
			case k_undef:
			default: break;
			}
		}
		for (int i = 0; i < num1; i++) {
			if (H_ptr[i] != NULL && (*H_ptr[i]).M_push == 1) {
				H_obj_move(H_ptr[i]);
			}
		}
		for (int i = 0; i < num2; i++) {
			if (M_ptr[i] != NULL && (*M_ptr[i]).P_push == 1) {
				P_obj_move(M_ptr[i]);
			}
		}

		//if (H_ai_units[0].exist != 0) { //  AI �ʱ� H
		//	H_obj_move(&H_ai_units[0]);
		//}

		POSITION space_positions[50] = { 0 };
		int space_cnt = find_space_positions(space_positions); 
		POSITION h_move = { 3,MAP_WIDTH - 3 };
		if (space_cnt % 5 == 0&& AI_resource.spice-5 >= 0) { // �����̽��� 5�� ��� �϶� �Ϻ����� ����
			int H_it = 0; 
			if (map[1][3][MAP_WIDTH - 2] == 'H' && map[1][3][MAP_WIDTH - 3] == 'H') {
				H_it = 1;
			}
			else if (map[1][3][MAP_WIDTH - 2] == 'H' && map[1][3][MAP_WIDTH - 3] != ' ') {
				POSITION h_move = { 3,MAP_WIDTH -  2};
			}
			if (H_it == 0) {
				int H_num= push_units(H_ai_units, h_move , 70, Harvest);
				map[1][h_move.row][h_move.column] = Harvest.symbol;
				AI_resource.spice -= Harvest.spice_cost;
				AI_resource.population += Harvest.population;
			}
		}

		for (int i = 0; i < 20; i++) {
			if (H_ai_units[i].exist != 0) {
				H_obj_move(&H_ai_units[i]);
			}
		}
		if (sys_clock != 0) {
			if (sys_clock == 10000&& AI_resource.spice > 3 && a == 0) {
				AI_Building(&AI_resource, &A);
				a = 1;
			}
			if (sys_clock == 40000&& AI_resource.spice > 5 && f == 0) {
				AI_Building(&AI_resource, &F);
				f = 1;
			}
			if (sys_clock % 15000 == 0 && AI_resource.spice > 1) {// ���� ���� 15�� ����
				AI_Building(&AI_resource, &P_ai);
			}
			if ( AI_resource.spice >=20 ) {
				for (int i = 0; i < 10; i++) {
					if (P_buildings[i].exist == 1) {
						AI_Building(&AI_resource, &G);
						break;
					}
				}
			}
			if (AI_resource.population>=0) {
				for (int i = 0; i < 10; i++) {
					if (P_buildings[i].exist == 1) {
						AI_Building(&AI_resource, &D);
						break;
					}
				}
			}
		}

		sandworm_move();

		// ���� ������Ʈ ����
		sample_obj_move(&obj);

		// ȭ�� ���
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");
	const char* intro[] = {
	   "  ____    _   _   _   _   _____ ",
	" |  _ \\  | | | | | \\ | | | ____|",
	" | | | | | | | | |  \\| | |  _|  ",
	" | |_| | | |_| | | |\\  | | |___ ",
	" |____/   \\___/  |_| \\_| |_____|",
	"",
	"   The spice must flow...      ",
	"",
	" Welcome to Arrakis, the desert planet",
	""
	};
	for (int i = 0; i < sizeof(intro) / sizeof(intro[0]); i++) {
		printf("%s\n", intro[i]);
		Sleep(1000);;  // 100ms ������
	}
	Sleep(2000);
	system("cls");// �ܼ�â ����� ��ɾ�
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	// layer 0(map[0])�� ���� ����
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// layer 1(map[1])�� ��� �α�(-1�� ä��)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[2][i][j] = -1;
		}
	}
}


//1) �ʱ���� ǥ��
void Initial_State(void) { // �⺻ �� ���
	//��
	map[0][MAP_HEIGHT - 3][1] = SYMBOL_BASE;
	map[0][MAP_HEIGHT - 3][2] = SYMBOL_BASE;
	map[0][MAP_HEIGHT - 2][1] = SYMBOL_BASE;
	map[0][MAP_HEIGHT - 2][2] = SYMBOL_BASE;


	map[0][MAP_HEIGHT - 3][3] = SYMBOL_PLATE;
	map[0][MAP_HEIGHT - 2][3] = SYMBOL_PLATE;
	map[0][MAP_HEIGHT - 3][4] = SYMBOL_PLATE;
	map[0][MAP_HEIGHT - 2][4] = SYMBOL_PLATE;
	map[1][MAP_HEIGHT - 4][1] = SYMBOL_HARVESTER;

	// Spice fields
	map[0][MAP_HEIGHT - 6][1] = '5';
	map[0][5][MAP_WIDTH - 2] = '5';

	// Top-right base and harvester
	map[0][1][MAP_WIDTH - 5] = SYMBOL_PLATE;
	map[0][1][MAP_WIDTH - 4] = SYMBOL_PLATE;
	map[0][1][MAP_WIDTH - 3] = SYMBOL_BASE;
	map[0][1][MAP_WIDTH - 2] = SYMBOL_BASE;
	map[0][2][MAP_WIDTH - 5] = SYMBOL_PLATE;
	map[0][2][MAP_WIDTH - 4] = SYMBOL_PLATE;
	map[0][2][MAP_WIDTH - 3] = SYMBOL_BASE;
	map[0][2][MAP_WIDTH - 2] = SYMBOL_BASE;
	map[1][3][MAP_WIDTH - 2] = SYMBOL_HARVESTER;

	// Sandworms
	map[1][2][MAP_WIDTH / 5] = SYMBOL_SANDWORM; // w
	map[1][MAP_HEIGHT - 8][45] = SYMBOL_SANDWORM; // w

	// Rocks
	map[0][MAP_HEIGHT - 6][MAP_WIDTH / 5 + 1] = SYMBOL_ROCK;//R

	map[0][3][MAP_WIDTH / 3] = SYMBOL_ROCK;//R
	map[0][3][MAP_WIDTH / 3 + 1] = SYMBOL_ROCK;//R
	map[0][4][MAP_WIDTH / 3] = SYMBOL_ROCK;//R
	map[0][4][MAP_WIDTH / 3 + 1] = SYMBOL_ROCK;//R

	map[0][MAP_HEIGHT - 3][MAP_WIDTH / 2 - 1] = SYMBOL_ROCK;//R
	map[0][MAP_HEIGHT - 3][MAP_WIDTH / 2] = SYMBOL_ROCK;//R
	map[0][MAP_HEIGHT - 4][MAP_WIDTH / 2 - 1] = SYMBOL_ROCK;//R
	map[0][MAP_HEIGHT - 4][MAP_WIDTH / 2] = SYMBOL_ROCK;//R

	map[0][MAP_HEIGHT - 4][MAP_WIDTH - 5] = SYMBOL_ROCK;//R

	map[0][4][MAP_WIDTH - (MAP_WIDTH / 5)] = SYMBOL_ROCK;//R

}


// (�����ϴٸ�) ������ �������� Ŀ�� �̵�
void cursor_move(DIRECTION dir, int distance) {
	POSITION curr = cursor.current;
	POSITION new_pos = curr;
	int size;
	if (Build_select >= 2) size = 1;
	else size = 0;

	for (int i = 0; i < distance; i++) {
		new_pos = pmove(new_pos, dir);
		// �� �ܰ踶�� ��ȿ�� �˻�
		if (!(1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 - size &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2 - size)) {
			// �� ��迡 �����ϸ� �̵� �ߴ�
			return;
		}
	}

	// ���� ��ġ�� ��ȿ�ϸ� Ŀ�� ������Ʈ
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 - size &&
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2 - size) {
		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
}

//3) �߸�����
/* ================= sample object movement =================== */
//������ �����̴� �ڵ�
POSITION sandworm_pos1 = { 2, MAP_WIDTH / 5 }; // �ʱ� ����� ��ġ
POSITION sandworm_pos2 = { MAP_HEIGHT - 8, 45 }; // �ʱ� ����� ��ġ

OBJECT_SAMPLE name = {
	.pos = {0,0}
};
//r,w�����̴� ��
POSITION sample_obj_next_position(OBJECT_SAMPLE* name) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub((*name).dest, (*name).pos);
	DIRECTION dir;
	// ������ ����
	if (diff.row == 0 && diff.column == 0) {
		if ((*name).dest.row == 1 && (*name).dest.column == 1) {
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			(*name).dest = new_dest;
			// topleft --> bottomright�� ������ ����
			new_dest = (*name).dest;
		}
		else {
			// bottomright --> topleft�� ������ ����
			POSITION new_dest = { 1, 1 };
			(*name).dest = new_dest;
		}
	}

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}
	if ((*name).repr == 'W') { //�����̽� ���� ����
		int cycle = rand() % 9;
		if (cycle < 1) {
			int spice_amount = rand() % 9 + 1;
			map[0][(*name).pos.row][(*name).pos.column] = '0' + spice_amount;
			random_space_letter(1, &resource);
		}
	}
	// validation check
	// next_pos�� ���� ����� �ʰ�, (������ ������)��ֹ��� �ε����� ������ ���� ��ġ�� �̵�
	// ������ �浹 �� �ƹ��͵� �� �ϴµ�, ���߿��� ��ֹ��� ���ذ��ų� ���� ������ �ϰų�... ���
	POSITION next_pos = pmove((*name).pos, dir);
	int next_rock = 0;
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
		if (map[0][next_pos.row][next_pos.column] == 'R' 
			|| map[1][next_pos.row][next_pos.column] == 'W') { //���� �տ� ������� ���ذ�
			if (dir == d_down || dir == d_up) {
				dir = (diff.column >= 0) ? d_right : d_left;
			}
			else {
				dir = (diff.row >= 0) ? d_down : d_up;
			}
			next_rock = 1;
			next_pos = pmove((*name).pos, dir);
		}
		return next_pos;
	}
	else {
		return (*name).pos;  // ���ڸ�
	}
}

//r,w�����̴� ��
void sample_obj_move(OBJECT_SAMPLE* name) { 
	if (sys_clock <= (*name).next_move_time) {
		// ���� �ð��� �� ����
		return;
	}
	int layer;
	if ((*name).repr == 'r') layer = 2;
	else layer = 1;
	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	map[layer][(*name).pos.row][(*name).pos.column] = -1;
	(*name).pos = sample_obj_next_position(name);
	switch (map[1][(*name).pos.row][(*name).pos.column])
	{
	case 'H': random_space_letter(2, &resource); reset((*name).dest); break;
	case 's': random_space_letter(4, &resource); reset((*name).dest); break;
	case 'f': division((*name).pos, &resource); reset((*name).dest); break;
	case 'T': reset((*name).dest); break; // ������
	default:
		break;
	}
	map[layer][(*name).pos.row][(*name).pos.column] = (*name).repr;
	(*name).next_move_time = sys_clock + (*name).speed;
}


void sandworm_move() {
	if (sys_clock < sandworm_next_move_time) return;

	sandworm1.dest = SANDWORM_find(sandworm1.pos);
	sandworm2.dest = SANDWORM_find(sandworm2.pos);

	sample_obj_move(&sandworm1);
	sample_obj_move(&sandworm2);
}

char over_pay(OBJECT_SAMPLE* name, char ch) {
	int num = ch - '0';
	int spice_num = rand() % 3 + 2;
	if (num - spice_num > 0) {
		(*name).space_number += spice_num;
		num -= spice_num;
		return '0' + num;
	}
	else {
		(*name).space_number = num;
		return '0';
	}
}


//�Ϻ����� �����̴� ��
POSITION H_obj_next_position(OBJECT_SAMPLE* name) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub((*name).dest, (*name).pos);
	DIRECTION dir;
	// ������ ����
	if (diff.row == 0 && diff.column == 0) {
		if ((*name).M_push==1&&(*name).dest.row == MAP_HEIGHT - 4 && ((*name).dest.column == 1
			|| (*name).dest.column == 2)) {
			POSITION new_dest = space_find((*name).dest, (*name).M_push);
			(*name).dest = new_dest;
			return (*name).pos;
		}
		else if ((*name).pos.row == 3 && ((*name).pos.column == MAP_WIDTH - 2 || (*name).pos.column == MAP_WIDTH - 3)) {
			POSITION new_dest = space_find((*name).dest, (*name).M_push);
			(*name).dest = new_dest;
			return (*name).pos;
		}
		else if (map[0][(*name).pos.row][(*name).pos.column] == ' ') { // �̹� �����̽��� �� �Ծ ���ٸ�
			// ���ο� �����̽� ��ġ ã��
			POSITION new_dest = space_find((*name).pos, (*name).M_push);
			(*name).dest = new_dest;
			return (*name).pos;
		}
	}

	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	POSITION next_pos = pmove((*name).pos, dir);

	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
		if ((*name).repr == 'H') {
			if (map[1][next_pos.row][next_pos.column] == 'H' || map[1][next_pos.row][next_pos.column] == 'W') {
				if (MAP_HEIGHT - 4 == next_pos.row && 1 == next_pos.column &&map[1][MAP_HEIGHT - 4][1] == 'H') {
					POSITION new_dest = { MAP_HEIGHT - 4, 2 };
					(*name).dest = new_dest;
				}
				else if (map[1][3][MAP_WIDTH - 2] == 'H' && 3 == next_pos.row && MAP_WIDTH - 2 == next_pos.column) {
					POSITION new_dest = { 3, MAP_WIDTH - 3 };
					(*name).dest = new_dest;
				}
				else if (map[1][(*name).dest.row][(*name).dest.column] == 'H') {
					return (*name).pos;
				}
				if (dir == d_down || dir == d_up) {
					dir = (diff.column >= 0) ? d_right : d_left;
				}
				else {
					dir = (diff.row >= 0) ? d_down : d_up;
				}
				next_pos = pmove((*name).pos, dir);
			}
		}
		else {
			if(map[1][next_pos.row][next_pos.column] == 'B'
				|| map[1][next_pos.row][next_pos.column] == 'P'
				|| map[1][next_pos.row][next_pos.column] == 'D'
				|| map[1][next_pos.row][next_pos.column] == 'G'
				|| map[1][next_pos.row][next_pos.column] == 'A'
				|| map[1][next_pos.row][next_pos.column] == 'F')
				return (*name).pos;
		}
		return next_pos;
	}
	else {
		return (*name).pos;  // ���ڸ�
	}
}


//�Ϻ����� �����̴� ��
void H_obj_move(OBJECT_SAMPLE* name) {
	if (sys_clock <= (*name).next_move_time) {
		// ���� �ð��� �� ����
		return;
	}
	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	map[1][(*name).pos.row][(*name).pos.column] = -1;
	(*name).pos = H_obj_next_position(name);
	if (map[0][(*name).pos.row][(*name).pos.column] >= '1' &&
		map[0][(*name).pos.row][(*name).pos.column] <= '9') { // �����̽��� �Դ°�
		char result = over_pay(name, map[0][(*name).pos.row][(*name).pos.column]);
		if (result == '0') {
			map[0][(*name).pos.row][(*name).pos.column] = ' ';
		}
		else {
			map[0][(*name).pos.row][(*name).pos.column] = result;
		}
		if ((*name).M_push == 1) {
			random_space_letter(3, &resource);
			POSITION new_dest = { MAP_HEIGHT - 4 , 1 };
			(*name).dest = new_dest;
		}
		else {
			POSITION new_dest = { 3 ,MAP_WIDTH - 2 };
			(*name).dest = new_dest;
		}
	}
	else if ((*name).pos.row == MAP_HEIGHT - 4 && ((*name).pos.column == 1|| (*name).pos.column == 2)){
		if (resource.spice_max - ((*name).space_number + (*name).space_number) >= 0) { // �����̽��� ������ �ȳ־���
			resource.spice += (*name).space_number;
		}
		(*name).space_number = 0;
	}
	else if ((*name).pos.row == 3 && ((*name).pos.column == MAP_WIDTH - 2 || (*name).pos.column == MAP_WIDTH - 3)){
		if (AI_resource.spice_max - ((*name).space_number + (*name).space_number) >= 0) {
			AI_resource.spice += (*name).space_number;
		}
		(*name).space_number = 0;
		
	}
	map[1][(*name).pos.row][(*name).pos.column] = (*name).repr;
	(*name).next_move_time = sys_clock + (*name).speed;
}


//P�� �������� ���� ������
POSITION P_obj_next_position(OBJECT_SAMPLE* name) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub((*name).dest, (*name).pos);
	DIRECTION dir;
	// ������ ����
	if (diff.row == 0 && diff.column == 0) {
		if ((*name).originally_pos.row != (*name).pos.row && (*name).originally_pos.column != (*name).pos.column) {
			POSITION new_dest = (*name).originally_pos;
			(*name).originally_pos = (*name).dest;
			(*name).dest = new_dest;
		}
		else {
			POSITION new_dest = (*name).dest;
			(*name).dest = (*name).originally_pos;
			(*name).originally_pos = new_dest;
		}
	}

	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	POSITION next_pos = pmove((*name).pos, dir);

	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && 
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
		return next_pos;
	}
	else {
		return (*name).pos;  // ���ڸ�
	}
}


void P_obj_move(OBJECT_SAMPLE* name) {
	if (sys_clock <= (*name).next_move_time) {
		// ���� �ð��� �� ����
		return;
	}
	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	map[1][(*name).pos.row][(*name).pos.column] = -1;
	(*name).pos = P_obj_next_position(name);
	map[1][(*name).pos.row][(*name).pos.column] = (*name).repr;
	(*name).next_move_time = sys_clock + (*name).speed;
}




//============ai=======

void initial_ai_H(void) { // �ʱ� H ��ġ ����
	POSITION pos = { 3 ,MAP_WIDTH - 2 };
	push_units(H_ai_units, pos, 70, Harvest);
}

OBJECT_SAMPLE* ai_H_move(int num) {
	H_ai_units[num].dest = space_find(H_ai_units[num].pos, H_ai_units[num].M_push);
	return &H_ai_units[num];
}