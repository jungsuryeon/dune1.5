//1)~5)���� ��������ϴ�.

// 1 �غ�
// 1-1 ����â, �ý��� �޽���, ���â ǥ��
// 1-2 ǥ��(��Ȳ���� ��� �����̽� �ʷ����� ǥ��), �ʱ��ġ ǥ��

// 2 Ŀ�� & ����â
// 2-1 ����Ű ����Ŭ��, �����̽��� ������ ����, ��� ǥ��
// 2-2 ��� ����â ����( ���â�� ����°� ����ؼ� ���� ����)
// 

// 3 �߸�����
// 3-1 ����ư �Ÿ��� ���� H�� ã�� ����� �Ϻ����ͷ� ������
// 3-2 �������� �Ϻ����͸� ������ ����
// 3-3 �����̽� ���� ����
// 3-4 ���� �ִ� '0'�� 'r'�� ǥ���ؼ� �縷 ������ ǥ�� layer2 ǥ����

// 4 ���� 1�� ����
// B���� �����̽��� ���� ����
// 4-1 ���� �������� ���â, ����â ǥ��
// 4-2 Ű���� H�� ������ �����̽����� ���� �Ϻ����� ���� �� �ý��� �޽��� �α�
// 4-3 Ű�� �߸� ������ ���� ���� ����
// 4-4 esc Ű�� ���� ���( �߰�: �߰��� H ��ġ���� esc�� ������ H�� ������ �����̽�, �α��� ����)

// 5 �ý��� �޽���
// 5-1 �޽����� ���̸� ��ũ�� �ö� 
// 5-2 H�� ���� �� ������ ���Ҷ�, ���������� ����, ������ �����̽� ���� �� �ý��� �޽��� ǥ��




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
void cursor_move(DIRECTION dir, int move_distance );
POSITION sample_obj_next_position(OBJECT_SAMPLE* name);
POSITION  SANDWORM_find(POSITION worm_position);
void sample_obj_move(OBJECT_SAMPLE* name);
void cursor_move(DIRECTION dir, int distance);
void sandworm_move();
char over_pay(char ch);
static clock_t last_key_time =0;

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

// 3)�縷 ������: �������� ���ƴٴϴ� �߸�����
OBJECT_SAMPLE obj = { 
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'r',
	.speed = 300,
	.next_move_time = 0
};

//3) ���� �Ϻ����ͷ� ���� ������
OBJECT_SAMPLE sandworm1= {
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

bool is_double_click = false;
int sandworm_next_move_time = 1000;


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

	OBJECT_SAMPLE * H_ptr = NULL;
	int H_move = 0;
	KEY last_key = 0;
	//2) Ŀ�� & ����â
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
			else{ 
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
			case K_Pd: P_push(); break;
			case K_Dd: D_push(); break;
			case K_Gd: G_push(); break;
			case K_Sd: S_push(cursor, &resource); break;
			case K_Md: H_ptr = M_push(cursor); H_move = 1; break;
			case K_Fd: F_push(cursor, &resource); 
				break;
			case k_none:
			case k_undef:
			default: break;
			}
		}
		if (H_ptr != NULL&& H_move == 1) {
			sample_obj_move(H_ptr);
		};
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

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'r';

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
	map[0][MAP_HEIGHT - 6][1] = '4'; 
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
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2- size)) {
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
int space_number = 0;
POSITION sample_obj_next_position(OBJECT_SAMPLE *name) {
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
		else if ((*name).dest.row == MAP_HEIGHT - 4 && (*name).dest.column == 1|| (*name).dest.column == 2) {
			POSITION new_dest = space_find((*name).dest);
			(*name).dest = new_dest;
		}
		else {
			// bottomright --> topleft�� ������ ����
			POSITION new_dest = { 1, 1 };
			(*name).dest = new_dest;
		}
		return (*name).pos;
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
			random_space_letter(1);
		}
	}
	// validation check
	// next_pos�� ���� ����� �ʰ�, (������ ������)��ֹ��� �ε����� ������ ���� ��ġ�� �̵�
	// ������ �浹 �� �ƹ��͵� �� �ϴµ�, ���߿��� ��ֹ��� ���ذ��ų� ���� ������ �ϰų�... ���
	POSITION next_pos = pmove((*name).pos, dir);
	int next_rock = 0;
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
		if (map[0][next_pos.row][next_pos.column] == 'R' || //���� �տ� ������� ���ذ�
			((*name).repr == 'H' && map[1][next_pos.row][next_pos.column] == 'W')) {
			if (dir == d_down || dir == d_up) {
				dir = (diff.column >= 0) ? d_right : d_left;
			}
			else {
				dir = (diff.row >= 0) ? d_down : d_up;
			}
			next_rock = 1;
			pmove((*name).pos, dir);
		}
		else if (map[1][next_pos.row][next_pos.column] == 'H') {
			return (*name).pos; // ���� �Ϻ����Ͱ� ������ ��ٸ�
		}
		if (next_rock != 1) {
			return next_pos;
		}
	}
	else {
		return (*name).pos;  // ���ڸ�
	}
}

void sample_obj_move(OBJECT_SAMPLE *name) {
	if (sys_clock <= (*name).next_move_time) {
		// ���� �ð��� �� ����
		return;
	}
	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	map[1][(*name).pos.row][(*name).pos.column] = -1;
	(*name).pos = sample_obj_next_position(name);
	if (map[1][(*name).pos.row][(*name).pos.column] == 'H') {
		random_space_letter(2);
		reset((*name).dest);
	}
	else if ((*name).repr=='H'&&map[0][(*name).pos.row][(*name).pos.column] >= '1' &&
		map[0][(*name).pos.row][(*name).pos.column] <= '9') {
		Sleep(2000); // ��Ȯ�� �ɸ��� �ð�
		char result = over_pay(map[0][(*name).pos.row][(*name).pos.column]);
		if (result == '0') {
			map[0][(*name).pos.row][(*name).pos.column] = ' ';
		}
		else {
			map[0][(*name).pos.row][(*name).pos.column] = result;
		}
		random_space_letter(3);
		POSITION new_dest = { MAP_HEIGHT - 4 , 1 };
		(*name).dest = new_dest;
	}
	else if ((*name).pos.row == MAP_HEIGHT - 4 && (*name).pos.column == 1) {
		if (resource.spice_max - (resource.spice+space_number)>= 0) {
			resource.spice += space_number;
		}
		space_number = 0;
	}
	map[1][(*name).pos.row][(*name).pos.column] = (*name).repr;
	(*name).next_move_time = sys_clock + (*name).speed;
}


void sandworm_move() {
	if (sys_clock < sandworm_next_move_time) return;

	sandworm1.dest = SANDWORM_find(sandworm1.pos);
	sandworm2.dest = SANDWORM_find(sandworm2.pos);

	sample_obj_move(&sandworm1);
	sample_obj_move(&sandworm2);
}

char over_pay(char ch) {
	int num = ch - '0';
	if (num + space_number < 5) {
		space_number += num;
		return '0';
	}
	else if (space_number != 4) {
		int remain = 4 - space_number;
		if (remain != 0) {
			num -= remain;
			space_number = 4;
			return '0' + num;  // ���ڷ� ��ȯ
		}
	}
	return ch;  // �⺻������ ���� ���� ��ȯ
}