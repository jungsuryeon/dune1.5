/*
* raw(?) I/O
*/
#include "io.h"
#include "io.h"
int x, y;

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // 행, 열 반대로 전달
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

KEY get_key(void) {
	if (!_kbhit()) {  // 입력된 키가 있는지 확인
		return k_none;
	}
	int byte = _getch();    // 입력된 키를 전달 받기
	switch (byte) {
	case 'q': return k_quit;  // 'q'를 누르면 종료
	case ' ': return k_space; // 스페이스바
	case 27: return k_esc; // esc 삭제
	case 'h':
	case 'H':return k_Hd;//h
	case 'm':
	case 'M':return k_Md;
	case 'B':
	case 'b':return K_Bd; // 건물 목록
	case 'p':
	case 'P': return K_Pd;
	case 'g':
	case 'G': return K_Gd;
	case 's':
	case 'S': return K_Sd;

	case 224:
		byte = _getch();  // MSB 224가 입력 되면 1바이트 더 전달 받기
		switch (byte) {
		case 72: return k_up;
		case 75: return k_left;
		case 77: return k_right;
		case 80: return k_down;
		default: return k_undef;
		}
	default: return k_undef;
	}
}