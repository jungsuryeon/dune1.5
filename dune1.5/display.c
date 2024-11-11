/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include <time.h>
#include <conio.h>
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
	display_resource(resource); // 자원 상태 표시
	display_map(map);
	clock_t last_time = clock();
	display_cursor(cursor);
}

void display_resource(RESOURCE resource) { // 자원 상태 표시
	set_color(COLOR_RESOURCE);// 컬러 변경
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
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
	case 'W': return 111; break;//샌드윔 황도색
	case 'P': return 14; break;//장판 검은색
	case 'R': return 112; break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':return 175; break;//스파이스 주황색
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
char Base[5][100] = { "나는야~ 본진 아트레이디스(플레이어)" , "하베스터를 생성할 수 있어", " ", " ", " " };
char AI[5][100] = { "나는야~ 하코넨(AI)" , "당신의 적이야", " ", " ", " " };
char AI_harvester[5][100] = { "나는야~ AI 하베스터", " ", " ", " "," " };
char Base_command[5][100] = { "H: Harvest(하베스터 생산)","건설비용 :  없음", "내구도 : 50", " " };
char place[5][100] = { "나는야~ 사막" , "기본 지형이며,", "건물을 지을 수 없어", "", " " }; // 객체당 하나
char Plate[5][100] = { "나는야~ 장판", "지형이며," ,"위에 건물을 지을 수 있어", "건물 짓기 전에 깔기 "," " };
char Rock[5][100] = { "나는야~ 바위", "지형이며," ,"샌드윔이 통과할 수 없어", " "," " };
char harvester[5][100] = { "나는야~ 하베스터", "생산비용: 5 " ,"인구 수 : 5", "이동주기:2000","체력70" };
char harvester_command[5][100] = { "H: Harvest(수확)", "M : move(이동)"," "," "," " };
char Sand[5][100] = { "나는야~ 샌드윔", "천천히 움직일게" ,"하지만 일반 유닛을 만나면 앙 먹어버릴거야", "가끔 배설도 해(생성 주기랑 매장량은 비밀)"," " };
char Space[5][100] = { "나는야~ 스파이스", "매장량표시(1~9)" ,"기본은 2개", "샌드윔이 만들어"," " };
char H_sistem_success[1][100] = { "새로운 하베스터가 생성되었습니다." };
char H_sistem_failure[1][100] = { "스파이스가 없어서 하베스터가 생성이 안됩니다." };
char M_sistem[1][100] = { "하베스터가 움직입니다." };
char random_space[1][100] = { "샌드윔이 스파이스를 생성했습니다." };
char eat_space[1][100] = { "샌드윔이 하베스터를 먹었습니다." };
char sistem[5][100] = { " "," ", " ", " "," " };

//5) 시스템 메시지 메시지 로그 스크롤 올리기
int num = 0;
int esc_letter = 0; //esc를 누르고 줄을 지웠는지 유무
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

//2)커서 & 상태창
//스페이스바를 눌렀을때 
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

//2) 커서 & 상태창
//esc 눌렀을때 상태창 비우기, 명령창도 지움
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

/*=============4) 유닛 1기 생산 ==========*/
// h를 눌렀을때
// 하베스터 추가건 초기 위치
POSITION h_move = { MAP_HEIGHT - 4,2 };
void h_push(CURSOR cursor, RESOURCE* resource){
	if ((*resource).spice - 5 >= 0) {
		if (apace_select == 1) {    //B 구간에서 선택 후 h를 눌렀을때
			sistem_letter(sistem, H_sistem_success);
			map[1][h_move.row][h_move.column] = SYMBOL_HARVESTER;
			apace_select = 0;
			(*resource).spice -= 5;
			(*resource).population += 5;
		}
	}
	else sistem_letter(sistem, H_sistem_failure);
}

// esc를 눌렀을때 선택 취소 , h 추가된 것에서 esc를 누르면 지워지면서 스파이스 인구수 변동
void esc_choice(CURSOR cursor, RESOURCE* resource) { //esc 눌렀을 경우 지우고 H 부분에서 esc를 누를 경우 아예 삭제
	apace_select = 0;
	//sistem_esc_choice();
	POSITION curr = cursor.current;
	if (curr.row == h_move.row && curr.column == h_move.column) {
		map[1][h_move.row][h_move.column] = ' ';
		(*resource).spice += 5; // 스파이스 +5
		(*resource).population -= 5; // 인구수 -5
	}
}

//void sistem_esc_choice(void) { // 시스템 마지막 한줄 지우는 것
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
			if (frontbuf[i][j] != backbuf[i][j]) {//frontbuf 이전 상태 , backbuf 현재상태
				POSITION pos = { i, j };
				printc(padd(map_pos, pos), backbuf[i][j], get_color(backbuf[i][j],i));
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous; // 직전위치
	POSITION curr = cursor.current; // 현재위치

	char ch = frontbuf[prev.row][prev.column];//예전 커서를 지우는 거
	printc(padd(map_pos, prev), ch, get_color(backbuf[prev.row][prev.column], prev.row));
	ch = frontbuf[curr.row][curr.column];//새로운 위치에 커서를 놓는것
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

/*================3) 중립유닛 ===============*/

// 맨해튼 거리 계산 함수
int calculate_distance(POSITION pos1, POSITION pos2) {
	return abs(pos1.row - pos2.row) + abs(pos1.column - pos2.column);
}

// 맵에서 모든 'H'의 위치를 찾는 함수
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
	return count; // 찾은 'H'의 개수 반환
}

POSITION  SANDWORM_find(POSITION worm_position) {
	POSITION H_positions[100]; // 최대 100개의 H 위치 저장 가능
	int num_H = find_H_positions(H_positions); // H의 위치 찾기

	if (num_H == 0) {
		// H가 없으면 기본값 반환
		return (POSITION) { -1, -1 };
	}

	int min_distance = 1000;
	POSITION closest_h = { -1, -1 };

	// 모든 H와의 거리를 계산하여 가장 가까운 H를 찾음
	for (int i = 0; i < num_H; i++) {
		int distance = calculate_distance(worm_position, H_positions[i]);
		if (distance < min_distance) {
			min_distance = distance;
			closest_h = H_positions[i];
		}
	}
	return closest_h;
} 

