/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include <time.h>
#include <conio.h>
#include <string.h>
#include "display.h"
#include "io.h"
#include "common.h"

clock_t last_press_time = 0;

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION system_pos = { MAP_HEIGHT + 1, 0 };
const POSITION object_info_pos = { 1, MAP_WIDTH + 1 };
const POSITION commands_pos = { MAP_HEIGHT + 1, MAP_WIDTH + 1 };
extern char map[2][MAP_HEIGHT][MAP_WIDTH];


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

//화면 배치
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
OBJECT_SAMPLE* P_unit_push(CURSOR cursor);
void D_push(void);
void G_push(void);
OBJECT_SAMPLE * M_push(CURSOR cursor);
void S_push(CURSOR cursor, RESOURCE* resource);
void Barracks_push(void);
void Building(CURSOR cursor, RESOURCE* resource, BUILD* name, char H_sistem[1][100], char state[5][100]);
void push_building(BUILD_s buildings[], POSITION pos, BUILD unit);
int push_units(OBJECT_SAMPLE units[], POSITION pos, int strength, UNIT unit);
void space_store(int num);
int find_H_positions(POSITION H_positions[]);
POSITION  SANDWORM_find(POSITION worm_position);
void esc_state(void);
void command_esc(void);
void H_number(int num);
POSITION produce_position(CURSOR cursor);
int find_space_positions(POSITION num_positions[]);
POSITION  space_find(POSITION space_position, int ai_player);
void division(POSITION pos1, RESOURCE* resource);
void random_space_letter(int num, RESOURCE* resource);
void initial_ai_H(void);
void AI_Building(RESOURCE* resource, BUILD* name);

// =================== 건물 구조체 ====================
//공통
BUILD P = {
	.symbol = 'P', // 화면 표시
	.spice_cost = 1, // 건설비용
	.population_increase = 0, // 인구 최대치 증가
	.spice_increase = 0,//스파이스 보관 최대치 증가
	.durability = 0,
	.ai = 0 //ai가 아니다

};

BUILD D = {
	.symbol = 'D', // 화면 표시
	.spice_cost = 2, // 건설비용
	.population_increase = 10, // 인구 최대치 증가
	.spice_increase = 0,//스파이스 보관 최대치 증가
	.durability = 10,
	.ai = 0//ai가 아니다
};

BUILD G = {
	.symbol = 'G', // 화면 표시
	.spice_cost = 4, // 건설비용
	.population_increase = 0, // 인구 최대치 증가
	.spice_increase = 10,//스파이스 보관 최대치 증가
	.durability = 10,
	.ai = 0//ai가 아니다
};

// 플레이어
BUILD B = { // 병영
	.symbol = 'b', // 화면 표시
	.spice_cost = 4, // 건설비용
	.population_increase = 0, // 인구 최대치 증가
	.spice_increase = 0,//스파이스 보관 최대치 증가
	.durability = 20,
	.ai = 0//ai가 아니다
};

BUILD S = {
	.symbol = 'S', // 화면 표시
	.spice_cost = 5, // 건설비용
	.population_increase = 0, // 인구 최대치 증가
	.spice_increase = 0,//스파이스 보관 최대치 증가
	.durability = 30,
	.ai = 0//ai가 아니다
};



//// ================= 유닛 구조체 ======================

UNIT Harvest = { //하베스터
	.symbol = 'H',// 화면 표시
	.spice_cost = 5, // 생산비용
	.population = 5, // 현재 인구 수 증가
	.move_period = 2000, // 이동주기
	.aggressive_strength = 0, // 공격력
	.attack_time = 0,// 공격주기
	.view = 0// 시야
};

UNIT Fremen = { //프레멘
	.symbol = 'f',// 화면 표시
	.spice_cost = 5, // 생산비용
	.population = 2, // 현재 인구 수 증가
	.move_period = 400, // 이동주기
	.aggressive_strength = 15, // 공격력
	.attack_time = 200,// 공격주기
	.view = 8// 시야
};

UNIT Soldier = { //보병
	.symbol = 's',// 화면 표시
	.spice_cost = 1, // 생산비용
	.population = 1, // 현재 인구 수 증가
	.move_period = 1000, // 이동주기
	.aggressive_strength = 5, // 공격력
	.attack_time = 800,// 공격주기
	.view = 1// 시야
};

UNIT Fighter = { // 투사
	.symbol = 'f',// 화면 표시
	.spice_cost = 1, // 생산비용
	.population = 1, // 현재 인구 수 증가
	.move_period = 1200, // 이동주기
	.aggressive_strength = 6, // 공격력
	.attack_time = 600,// 공격주기
	.view = 1// 시야
};

UNIT heavy_Tank = { // 중전차
	.symbol = 'T',// 화면 표시
	.spice_cost =12, // 생산비용
	.population = 5, // 현재 인구 수 증가
	.move_period = 3000, // 이동주기
	.aggressive_strength = 40, // 공격력
	.attack_time = 4000,// 공격주기
	.view = 4// 시야
};

//================ 유닛 구조체 배열 저장 ==============
OBJECT_SAMPLE H_units[20] = { 0 };
OBJECT_SAMPLE s_units[20] = { 0 };
OBJECT_SAMPLE Fremen_units[20] = { 0 };
OBJECT_SAMPLE Fighter_units[20] = { 0 };
OBJECT_SAMPLE T_units[20] = { 0 };



int push_units(OBJECT_SAMPLE units[], POSITION pos, int strength, UNIT unit) {
	for (int i = 0; i < 20; i++) {
		if (units[i].exist == 0) {
			units[i].exist = 1; // 배열에 들어갔는지 여부
			units[i].pos = pos; // 위치
			units[i].dest = pos;
			units[i].originally_pos = pos;
			units[i].strength = strength; // 체력
			units[i].repr = unit.symbol;
			units[i].speed = unit.move_period;
			units[i].next_move_time = unit.move_period;
			units[i].space_number = 0;
			units[i].M_push = 0;
			units[i].aggressive_strength = unit.aggressive_strength;
			units[i].attack_time = unit.attack_time;
			if (units[i].pos.row == 3 &&
				units[i].pos.column == MAP_WIDTH - 2 || units[i].pos.column == MAP_WIDTH - 3) {
				return i;
			}
			return 0;
		}
	}
	return 0;
}

void initial_H(void) { // 초기 H 위치 저장
	POSITION pos = { MAP_HEIGHT - 4, 1};
	push_units(H_units, pos, 70, Harvest);
}


void reset(POSITION dest) {
	OBJECT_SAMPLE* eat = NULL;
	for (int i = 0; i < 10; i++) {
		if (H_units[i].pos.row == dest.row &&
			H_units[i].pos.column == dest.column) {
			eat = &H_units[i];
		}
		else if (H_ai_units[i].pos.row == dest.row &&
			H_ai_units[i].pos.column == dest.column) {
			eat = &H_ai_units[i];
		}
		else if (s_units[i].pos.row == dest.row &&
			s_units[i].pos.column == dest.column) {
			eat = &s_units[i];
		}
		else if (Fremen_units[i].pos.row == dest.row &&
			Fremen_units[i].pos.column == dest.column) {
			eat = &Fremen_units[i];
		}
		else if (Fighter_units[i].pos.row == dest.row &&
			Fighter_units[i].pos.column == dest.column) {
			eat = &Fighter_units[i];
		}
		else if (T_units[i].pos.row == dest.row &&
			T_units[i].pos.column == dest.column) {
			eat = &T_units[i];
		}

		if (eat != NULL) {
			// 찾은 유닛 초기화
			eat->pos.row = 0;
			eat->pos.column = 0;
			eat->dest.row = 0;
			eat->dest.column = 0;
			eat->repr = 0;
			eat->speed = 0;
			eat->next_move_time = 0;

			// 초기화 후 루프 종료
			break;
		}
	}
}

//================ 유닛 구조체 배열 저장 ==============
BUILD_s P_buildings[10] = { 0 };
BUILD_s D_buildings[10] = { 0 }; 
BUILD_s G_buildings[10] = { 0 }; 
BUILD_s B_buildings[10] = { 0 }; 
BUILD_s S_buildings[10] = { 0 }; 
BUILD_s A_buildings[10] = { 0 }; 
BUILD_s F_buildings[10] = { 0 };

void push_building(BUILD_s buildings[], POSITION pos, BUILD unit) {
	for (int i = 0; i < 20; i++) {
		if (buildings[i].exist == 0) {
			buildings[i].exist = 1; // 배열에 들어갔는지 여부
			buildings[i].pos = pos; // 위치
			buildings[i].durability = unit.durability; // 내구도
			buildings[i].ai = unit.ai;// ai 인지
			buildings[i].symbol = unit.symbol; // 표시
			return;
		}
	}
}


void initial_p(void) {
	POSITION pos = { 1 ,MAP_WIDTH - 5 };
	push_building(P_buildings, pos, P);
}

//============ 초기 상태 =============/
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{
	display_resource(resource); // 자원 상태 표시
	display_map(map);
	clock_t last_time = clock();
	display_cursor(cursor);
}

void display_resource(RESOURCE resource) { // 자원 상태 표시
	set_color(COLOR_RESOURCE);// 컬러 변경
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d \n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

/*================== 1) 화면배치=======================*/
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

// 1) 초기상태 배경색 바꾸는 것
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
	case 'W': return 111; //샌드윔 황도색
	case 'P': return 14; //장판 검은색
	case 'R': return 112; 
	case 'D': return 176; 
	case 'G': return 95;  // 여기까지 공통
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

/*================== 2),4),5) 메시지 표시 =======================*/

//상태창 메시지
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

//명령창 메시지
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

/*===========2),4),5) 메시지 종류 =============*/
char place[5][100] = { "나는야~ 사막" , "기본 지형이며,", "건물을 지을 수 없어", "", " " };
char Rock[5][100] = { "나는야~ 바위", "지형이며," ,"샌드윔이 통과할 수 없어", " "," " };

char Base[5][100] = { "나는야~ 본진 아트레이디스(플레이어)" , "하베스터를 생성할 수 있어", " ", " ", " " };
char Base_command[5][100] = { "명령어 : 하베스터 생산(H : Harvest)","건설비용 :  없음", "내구도 : 50"," "};

char Build_base[5][100] = { "B : 건물 목록"," ", " "," "," " };
char Build_list[5][100] = { "장판(P: Plate)","숙소(D: Dormitory)", "창고(G: Garage)","병영(B: Barracks)","은신처(S: Shelter)" };

char Plate[5][100] = { "나는야~ 장판", "지형이며," ,"위에 건물을 지을 수 있어", "건물 짓기 전에 깔기 "," " };
char Plate_account[5][100] = { "장판(P: Plate)","건물 짓기 전에 깔기","건설비용 : 1","내구도: 없음"," " };
char Plate_creation[1][100] = { "[장판]이 생성되었습니다." };
char not_Plate[1][100] = { "[장판]이 없어서 생성될 수 없습니다." };

char Dormitory_creation[1][100] = { "[숙소]가 생성되었습니다." };
char Dormitory_account[5][100] = {"스페이스를 눌러 생성하세요", "숙소(D: Dormitory)","인구 최대치 증가(10)","건설비용 : 2","내구도: 10" };
char Dormitory1_account[5][100] = { "숙소(D: Dormitory)","인구 최대치 증가(10)","건설비용 : 2","내구도: 10"," "};

char Garage_creation[1][100] = { "[창고]가 생성되었습니다." };
char Garage_account[5][100] = { "스페이스를 눌러 생성하세요","창고(G: Garage) ","스파이스 보관 최대치 증가(10)","건설비용 : 4","내구도: 10" };
char Garage1_account[5][100] = { "창고(G: Garage) ","스파이스 보관 최대치 증가(10)","건설비용 : 4","내구도: 10"," "};

char Barracks_creation[1][100] = { "[병영]이 생성되었습니다." };
char Barracks_account[5][100] = {"스페이스를 눌러 생성하세요","병영(b: Barracks) ","건설비용 : 4","내구도: 20"};
char Barracks1_account[5][100] = { "나는야~ 병영(b: Barracks) ","보병 생산하며,","건설비용 : 4","내구도: 20"," "};

char Barracks_command[5][100] = { "명령어 : 보병 생산(S: Soldier)"," ", " "," "," " };
char Soldier_success[1][100] = { "새로운 [보병]이 생성되었습니다." };
char Soldier_failure[1][100] = { "스파이스가 없어서 [보병] 생성이 안됩니다." };
char Soldier_eat[1][100] = { "[샌드윔]이 [보병]을 먹었습니다." };
char Soldier_account[5][100] = { "나는야~ 보병 ","생산비용: 1, 인구수 : 1"," 이동주기 : 400","공격력: 5, 공격주기:800", "원래 체력 : 15" };
char units_command[5][100] = { "명령어 1 : 이동(M)","명령어 2 : 순찰(P)", " "," "," " };
char Soldier_sistem[1][100] = { "[보병]이 [이동]합니다." };
char Soldier_sistem1[1][100] = { "[보병]이 [순찰]합니다." };

char Shelter_creation[1][100] = { "[은신처]가 생성되었습니다." };
char Shelter_account[5][100] = { "스페이스를 눌러 생성하세요","은신처(S: Shelter) ","특수유닛 생산","건설비용 : 5","내구도 : 30" };
char Shelter1_account[5][100] = { "나는야~ 은신처(S: Shelter) ","특수유닛 생산하며, ","건설비용 : 5","내구도 : 30", " "};
char Shelter_command[5][100] = { "명령어 : 프레멘 생산(F: Fremen) "," ", " "," "," " };

char Fremen_success[1][100] = { "새로운 [프레멘]이 생성되었습니다." };
char Fremen_failure[1][100] = { "스파이스가 없어서 [프레멘] 생성이 안됩니다." };
char Fremen_account[5][100] = { "나는야~ 프레멘 ","생산비용: 5, 인구수 : 2"," 이동주기 : 2000","공격력: 15, 공격주기:200", "원래 체력 : 25" };
char Fremen_eat[1][100] = { "[샌드윔]이 [프레멘]을 먹었습니다." };
char Fremen_sistem[1][100] = { "[프레멘]가 [이동]합니다" };
char  Fremen_sistem1[1][100] = { "[프레멘]이 [순찰]합니다." };

char harvester[5][100] = { "", "생산비용: 5, 인구수 : 5" ,"이동주기 : 2000", "공격력: 없음, 공격주기: 없음","원래 체력 : 70" };
char harvester_command[5][100] = { "H: Harvest(수확)", " "," "," "," " };
char harvester2_command[5][100] = { "원하는 스파이스로 커서를 이동 후 ", "명령어를 누르세요","M : move(이동)"," "," " };
char H_system_success[1][100] = { "새로운 [하베스터]가 생성되었습니다." };
char H_system_push[1][100] = { "[하베스터]가 선택되었습니다." };
char H_system_failure[1][100] = { "스파이스가 없어서 [하베스터]가 생성이 안됩니다." };
char H_system_failure_stay[1][100] = { "H를 움직인 후 생성해주세요." };
char M_system[1][100] = { "[하베스터]가 움직입니다." };

char Sand[5][100] = { "나는야~ 샌드윔", "천천히 움직일게" ,"하지만 일반 유닛을 만나면 앙 먹어버릴거야", "가끔 배설도 해(생성 주기랑 매장량은 비밀)"," " };
char random_space[1][100] = { "샌드윔이 스파이스를 생성했습니다." };
char eat_H[1][100] = { "[샌드윔]이 [하베스터]를 먹었습니다." };

char sistem[7][100] = { " "," ", " ", " "," "," "," "};

char Space[5][100] = { "나는야~ 스파이스", "" ,"기본은 2개", "샌드윔이 만들어"," " };
char eat_space[1][100] = { "[스파이스]를 먹었습니다."};
char Space2[1][100] = { "[스파이스] 저장공간이 다 찼습니다." };


//=====하코넨==========
char AI[5][100] = { "나는야~ 하코넨(AI)" , "당신의 적이야", " ", " ", " " };
char AI_harvester[5][100] = { "나는야~ AI 하베스터", " ", " ", " "," " };
char Arena[5][100]= { "나는 투기장", "투사 생산(f:fighter)가능 " ,"건설비용 : 3", "내구도:15"," " };
char factory[5][100] = { "나는 공장", "특수유닛을 생산(T:hevy Tank) " ,"건설비용 : 5", "내구도:30"," " };
char fighter[5][100]= { "나는 투사(f:fighter)", "생산비용: 1, 인구수 : 1" ,"이동주기 : 1200  ", "공격력: 6, 공격주기: 600","원래 체력 : 10" };
char Tank[5][100] = { "나는 중전차(T:hevy Tank)", "생산비용: 12, 인구수 : 5" ,"이동주기:3000", "공격력: 40, 공격주기: 4000","원래 체력 : 60" };

//======= 공격 당했을때 ==========
char attack_buildings[1][100] = { "하코넨의 건물을 파괴했습니다." };
char attack_unit[1][100] = { "하코넨의 유닛을 파괴했습니다." };

void space_store(int num) { // 각 매장량 표시
	char buff[100];
	snprintf(buff, sizeof(buff),"매장량 : %d",num);
	POSITION pos = { 4, MAP_WIDTH + 2 };
	gotoxy(pos);
	set_color(COLOR_DEFAULT);
	printf(" %s", buff);
}

void H_number(int num) { // 각 매장량 표시
	char buff[100];
	snprintf(buff, sizeof(buff), "나는야 하베스터 배열 넘버: %d번", num);
	POSITION pos = { 3, MAP_WIDTH + 2 };
	gotoxy(pos);
	set_color(COLOR_DEFAULT);
	printf(" %s", buff);
}

//5) 시스템 메시지 메시지 로그 스크롤 올리기
int num = 0;
int esc_letter = 0; //esc를 누르고 줄을 지웠는지 유무

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

void random_space_letter(int num, RESOURCE* resource) {
	switch (num)
	{
	case 1: sistem_letter(sistem, random_space); break;
	case 2: sistem_letter(sistem, eat_H); (*resource).population -= Harvest.population; break;
	case 3: sistem_letter(sistem, eat_space); break;
	case 4: sistem_letter(sistem, Soldier_eat); (*resource).population -= Soldier.population;  break;
	default:
		break;
	}
}

void division(POSITION pos1, RESOURCE* resource) {
	for (int i = 0; i < 10; i++) {
		if (Fremen_units[i].pos.row == pos1.row
			&& Fremen_units[i].pos.column == pos1.column) {
			sistem_letter(sistem, Fremen_eat);
			(*resource).population -= Fremen.population;
			break;
		}
	}
}

int apace_select;
int Build_H_select = 0; // 하베스터에서 스페이스를 눌렀는지
/*=============4) 유닛 1기 생산 ==========*/
// h를 눌렀을때
// 하베스터 추가건 초기 위치

void h_push(CURSOR cursor, RESOURCE* resource) {
	POSITION curr = cursor.current;
	POSITION h_move = { MAP_HEIGHT - 4,2 };
	if (Build_H_select==1 && backbuf[curr.row][curr.column] == 'H') {
		Build_H_select += 1;
		command_letter(harvester2_command);
		sistem_letter(sistem, H_system_push);
	}
	else {
		if ((*resource).spice - 5 >= 0) {
			int H_it = 0;
			if (apace_select == 1 && map[0][curr.row][curr.column] == 'B') {    //B 구간에서 선택 후 h를 눌렀을때
				if (map[1][MAP_HEIGHT - 4][2] == 'H' && map[1][MAP_HEIGHT - 4][1] == 'H') {
					sistem_letter(sistem, H_system_failure_stay);
					H_it = 1;
				}
				else if (map[1][MAP_HEIGHT - 4][2] == 'H' && map[1][MAP_HEIGHT - 4][1] != ' ') {
					POSITION h_move = { MAP_HEIGHT - 4,1 };
				}
				if(H_it ==0){
					push_units(H_units, h_move, 70, Harvest);
					sistem_letter(sistem, H_system_success);
					map[1][h_move.row][h_move.column] = Harvest.symbol;
					apace_select = 0;
					(*resource).spice -= Harvest.spice_cost;
					(*resource).population += Harvest.population;
				}
			}
		}
		else sistem_letter(sistem, H_system_failure);
	}
}


//6) 건설
int Build_select = 0;
void B_push(void) { // B를 눌렀을때
	if (Build_select == 0) {
		command_letter(Build_list);
		Build_select = 1;
	}
	else {
		Barracks_push();
	}
}

void P_push(void) { // 장판 P 눌렀을때
	if (Build_select == 1) {
		command_esc();
		command_letter(Plate_account);
		esc_state();
		Build_select = 2;
	}
}

int Build_B_select = 0;// 병영에서 스페이스를 눌렀는지
int Build_S_select = 0;//은신처에서 스페이스를 눌렀는지
int Build_s_select = 0;// 보병에서 스페이스를 눌렀는지
int Build_f_select = 0;//프레멘에서 스페이스를 눌렀는지

int H_num; // 구조체 배열 위치
int s_num;
int f_num;
POSITION unitpos = { 0 };
OBJECT_SAMPLE * P_unit_push(CURSOR cursor) {
	POSITION curr = cursor.current;
	if (Build_s_select == 1) {
		s_units[s_num].dest = curr; // s 위치
		sistem_letter(sistem, Soldier_sistem1);
		mark_esc();
		Build_s_select = 0;
		s_units->originally_pos = unitpos;
		s_units->P_push = 1;
		return &s_units[s_num];
	}
	else if (Build_f_select == 1) {
		Fremen_units[f_num].dest = curr; // f 위치
		sistem_letter(sistem, Fremen_sistem1);
		mark_esc();
		Build_f_select = 0;
		Fremen_units->originally_pos = unitpos;
		Fremen_units->P_push = 1;
		return &Fremen_units[f_num];
	}
	return 0;
}

void D_push(void) { // 숙소 D 눌렀을때
	if (Build_select == 1) {
		command_esc();
		command_letter(Dormitory_account);
		esc_state();
		Build_select = 3;
	}
}

void G_push(void) { // 창고 G 눌렀을때
	if (Build_select == 1) {
		command_esc();
		command_letter(Garage_account);
		esc_state();
		Build_select = 4;
	}
}


void Barracks_push(void) { // 병영 B 눌렀을때
	if (Build_select == 1) {
		command_esc();
		command_letter(Barracks_account);
		esc_state();
		Build_select = 5;
	}
}


void S_push(CURSOR cursor, RESOURCE* resource) { // 은신처 S 눌렀을때
	command_esc();
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
	command_esc();
	POSITION curr = cursor.current;
	if (Build_S_select == 1) {
		POSITION produce = produce_position(cursor);//랜덤으로 위치 생성
		if ((*resource).spice - Fremen.spice_cost >= 0&& (produce.row != curr.row || produce.column != curr.column)) {
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

OBJECT_SAMPLE* M_push(CURSOR cursor) {
	POSITION curr = cursor.current;
	if (Build_H_select == 2 && backbuf[curr.row][curr.column] >= '1' && backbuf[curr.row][curr.column] <= '9') {
		H_units[H_num].dest = curr;
		sistem_letter(sistem, M_system);
		mark_esc();
		H_units[H_num].M_push = 1;
		Build_H_select = 0; 
		return &H_units[H_num];
	}
	else if (Build_s_select == 1) {
		
		if (map[0][curr.row][curr.column] == map[0][curr.row][curr.column - 1]
			&& map[0][curr.row][curr.column] == map[0][curr.row + 1][curr.column]
			) {
			s_units[s_num].dest.row = curr.row + 1;
			s_units[s_num].dest.column = curr.column;
		}
		else {
			s_units[s_num].dest = curr; // s 위치
		}
		sistem_letter(sistem, Soldier_sistem);
		mark_esc();
		s_units[s_num].M_push = 1;
		Build_s_select = 0;
		return &s_units[s_num];
	}
	else if (Build_f_select == 1) {
		if (map[0][curr.row][curr.column] == map[0][curr.row][curr.column - 1]
			&& map[0][curr.row][curr.column] == map[0][curr.row + 1][curr.column]
			) {
			Fremen_units[f_num].dest.row = curr.row + 1;
			Fremen_units[f_num].dest.column = curr.column;
		}
		else {
			Fremen_units[f_num].dest = curr; // f 위치
		}
		sistem_letter(sistem, Fremen_sistem);
		mark_esc();
		Fremen_units[f_num].M_push = 1;
		Build_f_select = 0;
		return &Fremen_units[f_num];
	}
	return NULL; // 조건을 만족하지 않을 때 NULL 반환
}

POSITION produce_position(CURSOR cursor) {
	POSITION curr = cursor.current;
	POSITION empty_positions[4] = { 0 };
	int empty_count = 0;

	// 2x2 영역 내의 빈 공간 찾기
	int offsets[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} }; // 상, 우, 하, 좌
	for (int i = 0; i < 4; i++) {
		POSITION adj_pos = {
			curr.row + offsets[i][0],
			curr.column + offsets[i][1]
		};
		if (backbuf[adj_pos.row][adj_pos.column] == ' ') {
			empty_positions[empty_count++] = adj_pos;
		}
	}

	// 빈 공간이 없는 경우
	if (empty_count == 0) {
		return curr; // 현재 위치 반환 또는 에러 처리
	}

	// 빈 공간이 1개인 경우
	if (empty_count == 1) {
		return empty_positions[0];
	}

	// 빈 공간이 2개 이상인 경우 무작위로 선택
	return empty_positions[rand() % empty_count];
}

void Building(CURSOR cursor, RESOURCE* resource, BUILD* name, char H_sistem[1][100], char state[5][100]) {
	POSITION curr = cursor.current;
	if (resource->spice >= name->spice_cost) {
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
		case 'P': push_building(P_buildings, curr, *name); break;
		case 'D': push_building(D_buildings, curr, *name); break;
		case 'G': push_building(G_buildings, curr, *name); break;
		case 'B': push_building(B_buildings, curr, *name); break;
		case 'S': push_building(S_buildings, curr, *name); break;
		default:
			break;
		}
		Build_select = 0;
	}
}

void AI_Building(RESOURCE* resource, BUILD* name) {
	int rand_row = 0; int rand_col =0;
	if (name->symbol == 'P') {
		while (1) {
			int row = MAP_WIDTH / 2;
			// 맵의 오른쪽 절반에만 생성되도록 수정
			rand_col = rand() % (row - 4) + row + 2;  // 중앙보다 오른쪽으로 2칸 이상 떨어지게
			int col = MAP_HEIGHT / 2;
			rand_row = rand() % (col - 4) + 2;  // 위아래 여유 공간 확보

			// 건물 공간이 모두 비어있는지 확인
			int cnt = 0;
			int can_build = 1;
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					if (backbuf[rand_row + i][rand_col + j] != ' ') {
						can_build = 0;
						break;
					}
				}
				if (!can_build) break;
			}

			// 모든 공간이 비어있을 때만 건설 진행
			if (can_build) {
				if (rand_row == 3 &&rand_col != MAP_HEIGHT - 2 && rand_col != MAP_HEIGHT - 3){
					break;
				}
			}
		}
	}
	else {
		for (int i = 0; i < 10; i++) {
			if (P_buildings[i].exist == 1) {
				rand_row = P_buildings[i].pos.row;
				rand_col = P_buildings[i].pos.column;
				P_buildings[i].exist = 0;
				P_buildings[i].pos.row = 0;
				P_buildings[i].pos.column = 0;
				break;
			}
		}
	}

	if (resource->spice >= name->spice_cost) {
		POSITION pos_building = { rand_row,rand_col };
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				POSITION pos = { pos_building.row + i, pos_building.column + j };
				map[0][pos.row][pos.column] = (*name).symbol;
			}
		}
		(*resource).spice -= (*name).spice_cost;
		(*resource).spice_max += (*name).spice_increase;
		(*resource).population_max += (*name).population_increase;
		switch ((*name).symbol)
		{
		case 'P': push_building(P_buildings, pos_building, *name); break;
		case 'D': push_building(D_buildings, pos_building, *name); break;
		case 'G': push_building(G_buildings, pos_building, *name); break;
		case 'A': push_building(A_buildings, pos_building, *name); break;
		case 'F': push_building(F_buildings, pos_building, *name); break;
		default:
			break;
		}
		Build_select = 0;
	}
}


//2)커서 & 상태창
//스페이스바를 눌렀을때 
void object_info_mark(CURSOR cursor, RESOURCE* resource) {
	POSITION curr = cursor.current;
	mark_esc();
	if (Build_select == 2 && backbuf[curr.row][curr.column] == ' ') {
		Building(cursor, resource, &P, Plate_creation, Plate);
	}
	else if (Build_select > 1&&backbuf[curr.row][curr.column] == 'P' &&
		backbuf[curr.row + 1][curr.column] == 'P' &&
		backbuf[curr.row][curr.column + 1] == 'P' &&
		backbuf[curr.row + 1][curr.column + 1] == 'P') {
		switch (Build_select) {
		case 3: // 숙소 건설 : D
			Building(cursor, resource, &D, Dormitory_creation, Dormitory1_account);
			break;
		case 4: // 창고 건설 : G
			Building(cursor, resource, &G, Garage_creation, Garage1_account);
			break;
		case 5: // 병영 건설 : B
			Building(cursor, resource, &B, Barracks_creation, Barracks1_account);
			break;
		case 6: // 은신처 : S
			Building(cursor, resource, &S, Shelter_creation, Shelter1_account);
			break;
		}
	}
	else if(Build_select > 1 && backbuf[curr.row][curr.column] != 'P' &&
		backbuf[curr.row + 1][curr.column] != 'P' &&
		backbuf[curr.row][curr.column + 1] != 'P' &&
		backbuf[curr.row + 1][curr.column + 1] != 'P'){
		sistem_letter(sistem, not_Plate);
		switch (Build_select){
		case 3: // 숙소 건설 : D
			command_letter(Dormitory_account);
			break;
		case 4: // 창고 건설 : G
			command_letter(Garage_account);
			break;
		case 5: // 병영 건설 : B
			command_letter(Barracks_account);
			break;
		case 6: // 은신처 : S
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
			for (int i = 0; i < 20; i++) {
				if (H_units[i].pos.row == curr.row
					&& H_units[i].pos.column == curr.column) {
					H_num = i;
					state_letter(harvester); H_number(H_num); command_letter(harvester_command);
					Build_H_select = 1;
					break;
				}
				else if(H_ai_units[i].pos.row == curr.row && H_ai_units[i].pos.column == curr.column) {
					state_letter(AI_harvester);
					break;
				}
			}
			break;
		case 'B':
			if (curr.row > MAP_HEIGHT / 2) {
				state_letter(Base); command_letter(Base_command);
				apace_select = 1;
				break;
			}
			else {
				state_letter(AI);
				break;
			}
		case 'D':state_letter(Dormitory_account); break;//숙소
		case 'G':state_letter(Garage1_account); break;//창고
		case 'b': state_letter(Barracks1_account); command_letter(Barracks_command); Build_B_select = 1; break;//병영
		case 'S': state_letter(Shelter1_account); command_letter(Shelter_command); Build_S_select = 1; break;//은신처
		case 'F':state_letter(fighter); break;//공장
		case 'A':state_letter(Arena); break;//투기장
		case 'T':state_letter(Tank); break;//중전차
		case 'f': // 프레멘, 투사
			for (int i = 0; i < 20; i++) {
				if (Fremen_units[i].pos.row == curr.row
					&& Fremen_units[i].pos.column == curr.column) {
					f_num = i;
					state_letter(Fremen_account); command_letter(units_command);
					Build_f_select = 1;
					unitpos = curr;
					break;
				}
			}
			if(Build_H_select ==0){
					state_letter(fighter); //투사
					break;
			}
		 break;
		case 's':
			for (int i = 0; i < 20; i++) {
				if (s_units[i].pos.row == curr.row
					&& s_units[i].pos.column == curr.column) {
					s_num = i;
					state_letter(Soldier_account); command_letter(units_command);
					Build_s_select = 1;
					unitpos = curr;
					break;
				}
				
			}//보병
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

//2) 커서 & 상태창
//esc 눌렀을때 상태창 비우기, 명령창도 지움
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

void command_esc(void) {
	for (int i = 0; i < 45; i++) {
		for (int j = 0; j < 5; j++) {
			POSITION pos1 = { MAP_HEIGHT + 3 + j, MAP_WIDTH + 3 + i };
			printc(pos1, ' ', COLOR_DEFAULT);
		}
	}
}

// esc를 눌렀을때 선택 취소 , h 추가된 것에서 esc를 누르면 지워지면서 스파이스 인구수 변동
void esc_choice(CURSOR cursor, RESOURCE* resource) { //esc 눌렀을 경우 지우고 H 부분에서 esc를 누를 경우 아예 삭제
	apace_select = 0; // H 생성 취소
	Build_H_select = 0; // H 수확 취소
	Build_S_select = 0;
	Build_s_select = 0;
	Build_f_select = 0;
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
	Build_select = 0; // B 누른거 선택 취소
}

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
			if (frontbuf[i][j] != backbuf[i][j]) {//frontbuf 이전 상태 , backbuf 현재상태
				POSITION pos = { i, j };
				printc(padd(map_pos, pos), backbuf[i][j], get_color(backbuf[i][j],i,j));
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous; // 직전위치
	POSITION curr = cursor.current; // 현재위치

	// 이전 커서 위치 지우기
	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, get_color(backbuf[prev.row][prev.column], prev.row, prev.column));

	if (Build_select >= 2) {
		// 4x4 영역 처리
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				POSITION pos = { prev.row + i, prev.column + j };
				ch = frontbuf[pos.row][pos.column];
				printc(padd(map_pos, pos), ch, get_color(backbuf[pos.row][pos.column], pos.row, pos.column));
			}
		}

		// 새 커서 위치에 4x4 영역 그리기
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				POSITION pos = { curr.row + i, curr.column + j };
				ch = frontbuf[pos.row][pos.column];
				printc(padd(map_pos, pos), ch, COLOR_CURSOR);
			}
		}
	}
	else {
		// 단일 커서 위치 그리기
		ch = frontbuf[curr.row][curr.column];
		printc(padd(map_pos, curr), ch, COLOR_CURSOR);
	}
}

/*================3) 중립유닛 ===============*/

// 맨해튼 거리 계산 함수
int calculate_distance(POSITION pos1, POSITION pos2) {
	return abs(pos1.row - pos2.row) + abs(pos1.column - pos2.column);
}

// 맵에서 모든 유닛
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
	return count; // 찾은 개수 반환
}

POSITION  SANDWORM_find(POSITION worm_position) {
	POSITION H_positions[50]; //H 위치저장
	int num_H = find_H_positions(H_positions); // H 위치 찾기

	if (num_H == 0) {
		// 없으면 기본값 반환
		return (POSITION) { -1, -1 };
	}

	int min_distance = 1000;
	POSITION closest_h = { -1, -1 };

	// 모든 H와의 거리를 계산하여 가장 가까운 유닛을 찾음
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
	return count; // 찾은 개수 반환
}


POSITION  space_find(POSITION space_position, int ai_player) {
	POSITION space_positions[50]; //스페이스 위치
	int num_H = find_space_positions(space_positions); // 스페이스 위치

	if (num_H == 0) {
		// 없으면 기본값 반환
		if (ai_player == 1) {
			if (backbuf[MAP_HEIGHT - 4][1] == ' ') {
				return (POSITION) { MAP_HEIGHT - 4, 1 };
			}
			else {
				return (POSITION) { MAP_HEIGHT - 4, 2 };
			}
		}
		else {
			if (backbuf[3][MAP_WIDTH - 2] == ' ') {
				return (POSITION) { 3, MAP_WIDTH - 2 };
			}
			else {
				return (POSITION) { 3, MAP_WIDTH - 3 };
			}
		}
	}

	int min_distance = 1000;
	POSITION closest_h = { -1, -1 };

	// 모든 H와의 거리를 계산하여 가장 가까운 유닛을 찾음
	for (int i = 0; i < num_H; i++) {
		int distance = calculate_distance(space_position, space_positions[i]);
		if (distance < min_distance) {
			min_distance = distance;
			closest_h = space_positions[i];
		}
	}
	return closest_h;
}


