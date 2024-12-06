#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>

/* ================= system parameters =================== */
#define TICK 10		// time unit(ms)
#define DOUBLE_CLICK_INTERVAL 1000

#define N_LAYER 3
#define MAP_WIDTH	60
#define MAP_HEIGHT	18

// Define symbols for elements
#define SYMBOL_BASE 'B' // 플레이어의 시작 위치
#define SYMBOL_SANDWORM 'W' // 샌드윔,중립유닛
#define SYMBOL_HARVESTER 'H'//하베스터,자원을 채취하는 유닛
#define SYMBOL_PLATE 'P' // 장판, 건설 가능한 지역
#define SYMBOL_ROCK 'R' // 바위, 샌드윔이 통과할 수 없는 지형

/* ================= 위치와 방향 =================== */
// 맵에서 위치를 나타내는 구조체
typedef struct {
	int row, column;
} POSITION;

// 커서 위치
typedef struct {
	POSITION previous;  // 직전 위치
	POSITION current;   // 현재 위치
} CURSOR;

typedef struct {
	POSITION SAND_previous;  // 직전 위치
	POSITION SAND_current;   // 현재 위치
} SAND_SPICE;

// 입력 가능한 키 종류.
// 수업에서 enum은 생략했는데, 크게 어렵지 않으니 예제 검색
typedef enum {
	// k_none: 입력된 키가 없음. d_stay(안 움직이는 경우)에 대응
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_space,
	k_esc,
	k_Hd,
	k_Md,
	K_Bd,
	K_Pd,
	K_Dd,
	K_Gd, 
	K_Fd,
	K_Sd,
	K_Md,
	k_undef, // 정의되지 않은 키 입력	
} KEY;


// DIRECTION은 KEY의 부분집합이지만, 의미를 명확하게 하기 위해서 다른 타입으로 정의
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;


/* ================= 위치와 방향(2) =================== */
// 편의성을 위한 함수들. KEY, POSITION, DIRECTION 구조체들을 유기적으로 변환

// 편의성 함수
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// 방향키인지 확인하는 함수
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// 화살표 '키'(KEY)를 '방향'(DIRECTION)으로 변환. 정수 값은 똑같으니 타입만 바꿔주면 됨
#define ktod(k)		(DIRECTION)(k)

// DIRECTION을 POSITION 벡터로 변환하는 함수
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p를 d 방향으로 이동시킨 POSITION
#define pmove(p, d)		(padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;		// 현재 보유한 스파이스
	int spice_max;  // 스파이스 최대 저장량
	int population; // 현재 인구 수
	int population_max;  // 수용 가능한 인구 수
} RESOURCE;


// 대강 만들어 봤음. 기능 추가하면서 각자 수정할 것
typedef struct {
	bool exist;
	POSITION pos;		// 현재 위치(position)
	POSITION dest;		// 목적지(destination)
	char repr;			// 화면에 표시할 문자(representation)
	int move_period;	// '몇 ms마다 한 칸 움직이는지'를 뜻함
	int next_move_time;	// 다음에 움직일 시간
	int speed;
	int strength; // 체력
	int space_number; // 각 스파이스 채취량
	int M_push;
	int P_push;
	POSITION originally_pos;
}OBJECT_SAMPLE;

typedef struct {
	bool exist;
	char symbol; // 화면 표시
	POSITION pos; // 현재 위치(좌상단)
	int spice_cost; // 건설비용
	int population_increase; // 인구 최대치 증가
	int spice_increase;//스파이스 보관 최대치 증가
	int durability; // 내구도
	int ai;
}BUILD;

typedef struct {
	bool exist;
	POSITION pos; // 현재 위치(좌상단)
	int durability; // 내구도
	int ai;
}BUILD_s;

typedef struct {
	char symbol; // 화면 표시
	int spice_cost; // 생산비용
	int population; // 현재 인구 수 증가
	int move_period; // 이동주기
	int aggressive_strength; // 공격력
	int attack;// 공격주기
	int view;// 시야
}UNIT;


#endif
