//1)~5)까지 만들었습니다.
//1)


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
POSITION sample_obj_next_position(OBJECT_SAMPLE);
POSITION  SANDWORM_find( POSITION worm_position);
void sample_obj_move(OBJECT_SAMPLE* name);
void cursor_move(DIRECTION dir, int distance);
void sandworm_move();
static clock_t last_key_time =0;

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 10,
	.spice_max = 300,
	.population = 10,
	.population_max = 300
};

// 3)사막 독수리: 목적없이 날아다니는 중립유닛
OBJECT_SAMPLE obj = { 
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'r',
	.speed = 300,
	.next_move_time = 300
};

//3) 본진 하베스터로 가는 샌드윔
OBJECT_SAMPLE sandworm1= {
	.pos = {2, MAP_WIDTH / 5},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 
	.repr = 'W',
	.speed = 2500,
	.next_move_time = 500
};

//3) 하코넨 하베스터로 가는 샌드윔
OBJECT_SAMPLE sandworm2 = {
	.pos = {MAP_HEIGHT - 8, 45},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.speed = 2500,
	.next_move_time = 500
};

bool is_double_click = false;
int sandworm_next_move_time = 1000;


/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	Initial_State();
	display_system_message();
	display_object_info();
	display_commands();
	display(resource, map, cursor);
	KEY last_key = 0;

	//2) 커서 & 상태창
	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();
		clock_t current_time = clock();
		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			 double time_diff = (double)(current_time - last_key_time) / CLOCKS_PER_SEC;
			if (key == last_key && time_diff < 0.3 && !is_double_click) {
				// 두 번째 클릭이 빠르게 이루어진 경우 // 두 번 클릭 중복 방지
				is_double_click = true;
				cursor_move(ktod(key), 5); // 5칸 이동
			}
			else{
				// 첫 번째 클릭이거나 느리게 눌린 경우
				is_double_click = false;
				cursor_move(ktod(key), 1); // 1칸 이동
				
			}
			 last_key = key;
			 last_key_time = current_time;
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro();
			case k_space:object_info_mark(cursor, &resource);  break;
			case k_esc:mark_esc();  esc_choice(cursor, &resource); break;
			case k_Hd: h_push(cursor, &resource); break;
			case k_none:
			case k_undef:
			default: break;
			}
		}

		sandworm_move();
		
		// 샘플 오브젝트 동작
		sample_obj_move(&obj);

		// 화면 출력
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
		Sleep(1000);;  // 100ms 딜레이
	}
	Sleep(2000);
	system("cls");// 콘솔창 지우는 명령어
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	// layer 0(map[0])에 지형 생성
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

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';

}

//1) 초기상태 표시
void Initial_State(void) { // 기본 맵 출력
	//밑
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


// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir, int distance) {
	POSITION curr = cursor.current;
	POSITION new_pos = curr;

	for (int i = 0; i < distance; i++) {
		new_pos = pmove(new_pos, dir);
		// 각 단계마다 유효성 검사
		if (!(1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2)) {
			// 맵 경계에 도달하면 이동 중단
			return;
		}
	}

	// 최종 위치가 유효하면 커서 업데이트
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
}



//3) 중립유닛
/* ================= sample object movement =================== */ 
//샌드윔 움직이는 코드
POSITION sandworm_pos1 = { 2, MAP_WIDTH / 5 }; // 초기 샌드웜 위치
POSITION sandworm_pos2 = { MAP_HEIGHT - 8, 45 }; // 초기 샌드웜 위치

OBJECT_SAMPLE name = {
	.pos = {0,0}
};

POSITION sample_obj_next_position(OBJECT_SAMPLE *name) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub((*name).dest, (*name).pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if ((*name).dest.row == 1 && (*name).dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = (*name).dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			(*name).dest = new_dest;
		}
		return (*name).pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}
	if ((*name).repr != 'r') { //스파이스 랜덤 생성
		int cycle = rand() % 9;
		if (cycle < 1) {
			int spice_amount = rand() % 9 + 1;
			map[0][(*name).pos.row][(*name).pos.column] = '0' + spice_amount;
			random_space_letter(1);
		}
	}
	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove((*name).pos, dir);
	int next_rock = 0;
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
		if (map[0][next_pos.row][next_pos.column] == 'R') { //돌이 앞에 있을경우 피해감
			if (dir == d_down || dir == d_up) {
				dir = (diff.column >= 0) ? d_right : d_left;
			}
			else {
				dir = (diff.row >= 0) ? d_down : d_up;
			}
			next_rock = 1;
			pmove((*name).pos, dir);
		}
		if (next_rock != 1) {
			return next_pos;
		}
	}
	else {
		return (*name).pos;  // 제자리
	}
}

void sample_obj_move(OBJECT_SAMPLE *name) {
	if (sys_clock <= (*name).next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][(*name).pos.row][(*name).pos.column] = -1;
	(*name).pos = sample_obj_next_position(name);
	if (map[1][(*name).pos.row][(*name).pos.column] == 'H')
		random_space_letter(2);
	map[1][(*name).pos.row][(*name).pos.column] = (*name).repr;
	(*name).next_move_time = sys_clock + (*name).speed;
}


void sandworm_move() {
	if (sys_clock < sandworm_next_move_time) return;
	/*sandworm_next_move_time += 2000;*/

	sandworm1.dest = SANDWORM_find(sandworm1.pos);
	sandworm2.dest = SANDWORM_find(sandworm2.pos);

	sample_obj_move(&sandworm1);
	sample_obj_move(&sandworm2);

	/*POSITION prev1 = sandworm1.pos;
	POSITION prev2 = sandworm2.pos;*/
	//POSITION map_pos = { 1, 0 };


	/*
	char buff1[100];
	snprintf(buff1, 100, "%d, %d ", h_pos1.row, h_pos1.column);
	for (int i = 0; i < 5; i++) {
		POSITION pos = { 22, 1 };
		POSITION pos2 = { 0, i };
		printc(padd(pos, pos2), buff1[i], COLOR_DEFAULT);
	}


	char buff2[100];
	snprintf(buff2, 100, "%d, %d ", h_pos2.row, h_pos2.column);
	for (int i = 0; i < 5; i++) {
		POSITION pos = { 23, 1 };
		POSITION pos2 = { 0, i };
		printc(padd(pos, pos2), buff2[i], COLOR_DEFAULT);
	}
	*/
}





