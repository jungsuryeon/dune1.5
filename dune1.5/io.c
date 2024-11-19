/*
* raw(?) I/O
*/
#include "io.h"
#include "io.h"
int x, y;

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // ��, �� �ݴ�� ����
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
	if (!_kbhit()) {  // �Էµ� Ű�� �ִ��� Ȯ��
		return k_none;
	}
	int byte = _getch();    // �Էµ� Ű�� ���� �ޱ�
	switch (byte) {
	case 'q': return k_quit;  // 'q'�� ������ ����
	case ' ': return k_space; // �����̽���
	case 27: return k_esc; // esc ����
	case 'h':
	case 'H':return k_Hd;//h
	case 'm':
	case 'M':return k_Md;
	case 'B':
	case 'b':return K_Bd; // �ǹ� ���
	case 'p':
	case 'P': return K_Pd;
	case 'g':
	case 'G': return K_Gd;
	case 's':
	case 'S': return K_Sd;

	case 224:
		byte = _getch();  // MSB 224�� �Է� �Ǹ� 1����Ʈ �� ���� �ޱ�
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