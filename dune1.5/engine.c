//1)~5)까지 만들었습니다.

// 1 준비
// 1-1 상태창, 시스템 메시지, 명령창 표현
// 1-2 표시(주황색이 없어서 스파이스 초록으로 표현), 초기배치 표현

// 2 커서 & 상태창
// 2-1 방향키 더블클릭, 스페이스바 누르면 상태, 명령 표시
// 2-2 취소 상태창 비우기( 명령창도 지우는게 깔끔해서 같이 지움)
// 

// 3 중립유닛
// 3-1 맨해튼 거리를 통해 H를 찾고 가까운 하베스터로 움직임
// 3-2 샌드윔이 하베스터를 만나면 지움
// 3-3 스파이스 랜덤 생성
// 3-4 원래 있던 '0'을 'r'로 표현해서 사막 독수리 표현 layer2 표현전

// 4 유닛 1기 생산
// B에서 스페이스를 누른 다음
// 4-1 각각 종류마다 명령창, 상태창 표시
// 4-2 키보드 H를 누르면 스파이스량에 따라 하베스터 생산 및 시스템 메시지 로그
// 4-3 키를 잘못 누르면 선택 상태 유지
// 4-4 esc 키로 선택 취소( 추가: 추가된 H 위치에서 esc를 누르면 H가 지워짐 스페이스, 인구수 변경)

// 5 시스템 메시지
// 5-1 메시지가 쌓이면 스크롤 올라감 
// 5-2 H가 생산 및 생산을 못할때, 샌드윔에게 먹힘, 샌드윔 스파이스 생성 때 시스템 메시지 표현




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

// 3)사막 독수리: 목적없이 날아다니는 중립유닛
OBJECT_SAMPLE obj = { 
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'r',
	.speed = 300,
	.next_move_time = 0
};

//3) 본진 하베스터로 가는 샌드윔
OBJECT_SAMPLE sandworm1= {
	.pos = {2, MAP_WIDTH / 5},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 
	.repr = 'W',
	.speed = 2500,
	.next_move_time = 2500
};

//3) 하코넨 하베스터로 가는 샌드윔
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
	initial_H(); // 초기 H 위치 저장

	OBJECT_SAMPLE * H_ptr = NULL;
	int H_move = 0;
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
	map[1][obj.pos.row][obj.pos.column] = 'r';

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
	int size;
	if (Build_select >= 2) size = 1;
	else size = 0;

	for (int i = 0; i < distance; i++) {
		new_pos = pmove(new_pos, dir);
		// 각 단계마다 유효성 검사
		if (!(1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 - size &&
			1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2- size)) {
			// 맵 경계에 도달하면 이동 중단
			return;
		}
	}

	// 최종 위치가 유효하면 커서 업데이트
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 - size &&
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2 - size) {
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
int space_number = 0;
POSITION sample_obj_next_position(OBJECT_SAMPLE *name) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub((*name).dest, (*name).pos);
	DIRECTION dir;
	// 목적지 도착
	if (diff.row == 0 && diff.column == 0) {
		if ((*name).dest.row == 1 && (*name).dest.column == 1) {
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			(*name).dest = new_dest;
			// topleft --> bottomright로 목적지 설정
			new_dest = (*name).dest;
		}
		else if ((*name).dest.row == MAP_HEIGHT - 4 && (*name).dest.column == 1|| (*name).dest.column == 2) {
			POSITION new_dest = space_find((*name).dest);
			(*name).dest = new_dest;
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
	if ((*name).repr == 'W') { //스파이스 랜덤 생성
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
		if (map[0][next_pos.row][next_pos.column] == 'R' || //돌이 앞에 있을경우 피해감
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
			return (*name).pos; // 다음 하베스터가 있으면 기다림
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
	if (map[1][(*name).pos.row][(*name).pos.column] == 'H') {
		random_space_letter(2);
		reset((*name).dest);
	}
	else if ((*name).repr=='H'&&map[0][(*name).pos.row][(*name).pos.column] >= '1' &&
		map[0][(*name).pos.row][(*name).pos.column] <= '9') {
		Sleep(2000); // 수확에 걸리는 시간
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
			return '0' + num;  // 문자로 변환
		}
	}
	return ch;  // 기본값으로 원래 숫자 반환
}