#include <windows.h>
#include <TCHAR.H>
#include <string>

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND 	hwnd;
	MSG 	msg;
	WNDCLASS	WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = _T("Window Class Name");
	RegisterClass(&WndClass);
	hwnd = CreateWindow(_T("Window Class Name"),
		_T("Window Title Name"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

#define ITEMNUM 10							// 아이템의 갯수 10개

int board[22][22];							// 보드판 22x22 보드판 2차원 공간 할당
int A_wormX[30], A_wormY[30];				// A지렁이의 X좌표 Y좌표 공간 할당
int A_xDirect, A_yDirect, A_len;			// A지렁이의 방향 및 길이 선언

int B_wormX[30], B_wormY[30];				// B지렁이의 X좌표 Y좌표 공간 할당
int B_xDirect, B_yDirect, B_len;			// B지렁이의 방향 및 길이 선언

TCHAR tBuffer[128];							// 남은 시간 출력을 위한 배열 할당
TCHAR result[128];							// A점수와 B점수 출력을 위한 배열
TCHAR winner[128];							// A점수와 B점수의 크기 비교 후 승자 출력을 위한 배열
static int count = 100;						// 100(10초)에서 점점 줄어드는 스톱워치

void ItemGenerator()						// 검은색 아이템 생성 함수
{
	int i, x, y;
	for (i = 0; i < ITEMNUM; i++)			// 아이템의 수는 총 10개
	{
		x = rand() % 20 + 1;				// x의 좌표 범위는 22, rand()%20 결과값은 19이하 이므로 오른쪽 왼쪽 테두리 +1
		y = rand() % 20 + 1;				// y의 좌표 범위는 22, rand()%20 결과값은 19이하 이므로 오른쪽 왼쪽 테두리 +1
		if (board[y][x] == 0)
			board[y][x] = 2;				// board[y][x] 가 0 은 아이템이 없는 상태, 2는 아이템이 있는 상태
		else
		{
			i = i - 1;						// 아이템을 한 개 먹을때마다 -1 만큼 for문이 돌아야함
			continue;
		}
	}
	return;
}


void GameInit() {							// 초기 게임판 설정
	int i;
	for (i = 0; i < 22; i++)				// 보드판 테두리 부분을 -1로 할당하는 반복문
	{
		board[i][0] = -1;
		board[i][21] = -1;
		board[0][i] = -1;
		board[21][i] = -1;
	}

	A_wormX[0] = 2;		A_wormY[0] = 1;		// A지렁이의 머리 초기 위치 (2,1)
	A_wormX[1] = 1;		A_wormY[1] = 1;		// A지렁이의 꼬리 초기 위치 (1,1)
	board[A_wormY[0]][A_wormX[0]] = 3;		// 지렁이의 머리 위치는 보드판 기준 3으로 할당
	board[A_wormY[1]][A_wormX[1]] = 3;		// 지렁이의 몸통 위치는 보드판 기준 3으로 할당

	B_wormX[0] = 19;	B_wormY[0] = 20;	// B지렁이의 머리 초기 위치 (19,20)
	B_wormX[1] = 20;	B_wormY[1] = 20;	// B지렁이의 꼬리 초기 위치 (20,20)
	board[B_wormY[0]][B_wormX[0]] = 3;		// 지렁이의 머리 위치는 보드판 기준 3으로 할당
	board[B_wormY[1]][B_wormX[1]] = 3;		// 지렁이의 몸통 위치는 보드판 기준 3으로 할당

	ItemGenerator();						// 검은색 아이템 생성 함수

	A_len = 2;								// A 지렁이의 총 길이는 2
	B_len = 2;								// B 지렁이의 총 길이는 2

	A_xDirect = 1;	A_yDirect = 0;			// xDirect가 1 yDirect가 0 의 뜻은 시작하자마자 오른쪽으로 움직임
	B_xDirect = -1;	B_yDirect = 0;			// xDirect가 -1 yDirect가 0 의 뜻은 시작하자마자 왼쪽으로 움직임
}

void DirectControl(int DirectKey)						// param 으로 키 입력 받아 방향키 정하는 함수
{
	switch (DirectKey) {								// param 키가 입력 받으면
	case VK_LEFT:										// 왼쪽 방향키 입력 시
		if (A_xDirect == 1)								// 이미 xDirect 가 1이라는 뜻은 오른쪽으로 이동상태
			break;
		if (board[A_wormY[0]][A_wormX[0] - 1] != -1)	// 벽(-1) 이 아니라면
		{
			A_xDirect = -1;								// xDirect = -1 은 왼쪽으로 이동함
			A_yDirect = 0;
		}
		break;
	case VK_RIGHT:										// 오른쪽 방향키 입력 시
		if (A_xDirect == -1)
			break;
		if (board[A_wormY[0]][A_wormX[0] + 1] != -1)	// 벽(-1) 이 아니라면
		{
			A_xDirect = 1;								// xDirect = 1 은 오른쪽으로 이동함
			A_yDirect = 0;
		}
		break;
	case VK_UP:											// 위쪽 방향키 입력 시
		if (A_yDirect == 1)
			break;
		if (board[A_wormY[0] - 1][A_wormX[0]] != -1)	// 벽(-1) 이 아니라면
		{
			A_xDirect = 0;
			A_yDirect = -1;								// yDirect = -1 은 위로 이동함
		}
		break;
	case VK_DOWN:
		if (A_yDirect == -1)							// 아래쪽 방향키 입력 시
			break;
		if (board[A_wormY[0] + 1][A_wormX[0]] != -1)	// 벽(-1) 이 아니라면
		{
			A_xDirect = 0;
			A_yDirect = 1;								// yDirect = 1 은 아래로 이동함
		}
		break;
	case VK_NUMPAD4:
		if (B_xDirect == 1)								// 이미 xDirect 가 1이라는 뜻은 오른쪽으로 이동상태
			break;
		if (board[B_wormY[0]][B_wormX[0] - 1] != -1)	// 벽(-1) 이 아니라면
		{
			B_xDirect = -1;								// xDirect = -1 은 왼쪽으로 이동함
			B_yDirect = 0;
		}
		break;
	case VK_NUMPAD6:
		if (B_xDirect == -1)
			break;
		if (board[B_wormY[0]][B_wormX[0] + 1] != -1)	// 벽(-1) 이 아니라면
		{
			B_xDirect = 1;								// xDirect = 1 은 오른쪽으로 이동함
			B_yDirect = 0;
		}
		break;
	case VK_NUMPAD8:
		if (B_yDirect == 1)
			break;
		if (board[B_wormY[0] - 1][B_wormX[0]] != -1)	// 벽(-1) 이 아니라면
		{
			B_xDirect = 0;
			B_yDirect = -1;								// yDirect = 1 은 위로 이동함
		}
		break;
	case VK_NUMPAD5:
		if (B_yDirect == -1)
			break;
		if (board[B_wormY[0] + 1][B_wormX[0]] != -1)	// 벽(-1) 이 아니라면
		{
			B_xDirect = 0;
			B_yDirect = 1;								// yDirect = 1 은 아래로 이동함
		}
		break;
	}
}			

void MovingWorm() 
{
	int A_tmpx, A_tmpy, i;								// 이전의 A지렁이의 X좌표, Y좌표 저장할 변수 선언
	int B_tmpx, B_tmpy;									// 이전의 B지렁이의 X좌표, Y좌표 저장할 변수 선언

	A_tmpx = A_wormX[0];								// 이전 A지렁이의 X좌표 할당
	A_tmpy = A_wormY[0];								// 이전 A지렁이의 Y좌표 할당

	A_wormX[0] += A_xDirect;							// 이전 A지렁이의 방향(왼쪽:-1, 오른쪽 : +1)을 기존 머리 X좌표에 더함
	A_wormY[0] += A_yDirect;							// 이전 A지렁이의 방향(위쪽:-1, 아래쪽 : +1)을 기존 머리 Y좌표에 더함

	if (board[A_wormY[0]][A_wormX[0]] == -1 || board[A_wormY[0]][A_wormX[0]] == 3)		// 더한 위치가 벽이거나 지렁이가 있다면
	{
		A_wormX[0] = A_tmpx;							// 다시 이전 X좌표로 돌아감
		A_wormY[0] = A_tmpy;							// 다시 이전 Y좌표로 돌아감
	}
	else
	{
		if (board[A_wormY[0]][A_wormX[0]] == 2)				// 보트판에 아이템이 있으면
		{
			A_len = A_len + 1;								// 꼬리 늘리고
			board[A_wormY[0]][A_wormX[0]] = 0;				// 아이템 삭제
		}
		else
			board[A_wormY[A_len - 1]][A_wormX[A_len - 1]] = 0;		// 지렁이의 꼬리기준 -1 위치 아이템 삭제

		for (i = A_len - 1; i > 1; i--)						// 한칸씩 움직여야 하기 때문에 이전 위치 한칸씩 움직이기
		{
			A_wormX[i] = A_wormX[i - 1];
			A_wormY[i] = A_wormY[i - 1];
		}
		A_wormX[1] = A_tmpx;								// 한칸씩 움직이기
		A_wormY[1] = A_tmpy;
		board[A_wormY[0]][A_wormX[0]] = 3;					// 지렁이 있는 위치는 board[][] = 3 으로 바꾸기
	}


	B_tmpx = B_wormX[0];									// 이전 B지렁이의 X좌표 할당
	B_tmpy = B_wormY[0];									// 이전 B지렁이의 Y좌표 할당

	B_wormX[0] += B_xDirect;								// 이전 B지렁이의 방향(왼쪽:-1, 오른쪽 : +1)을 기존 머리 X좌표에 더함
	B_wormY[0] += B_yDirect;								// 이전 B지렁이의 방향(위쪽:-1, 아래쪽 : +1)을 기존 머리 Y좌표에 더함

	if (board[B_wormY[0]][B_wormX[0]] == -1 || board[B_wormY[0]][B_wormX[0]] == 3)		// 더한 위치가 벽이거나 지렁이가 있다면
	{
		B_wormX[0] = B_tmpx;								// 다시 이전 X좌표로 돌아감
		B_wormY[0] = B_tmpy;								// 다시 이전 Y좌표로 돌아감
	}
	else
	{
		if (board[B_wormY[0]][B_wormX[0]] == 2)				// 보트판에 아이템이 있으면
		{
			B_len = B_len + 1;								// 꼬리 늘리고
			board[B_wormY[0]][B_wormX[0]] = 0;				// 아이템 삭제
		}
		else
			board[B_wormY[B_len - 1]][B_wormX[B_len - 1]] = 0;		// 지렁이의 꼬리기준 -1 위치 아이템 삭제

		for (i = B_len - 1; i > 1; i--)						// 한칸씩 움직여야 하기 때문에 이전 위치 한칸씩 움직이기
		{
			B_wormX[i] = B_wormX[i - 1];
			B_wormY[i] = B_wormY[i - 1];
		}
		B_wormX[1] = B_tmpx;								// 한칸씩 움직이기
		B_wormY[1] = B_tmpy;
		board[B_wormY[0]][B_wormX[0]] = 3;					// 지렁이 있는 위치는 board[][] = 3 으로 바꾸기
	}
}



void DrawGameBoard(HDC hdc)
{
	int i, x, y;
	for (y = 0; y < 22; y++) {
		for (x = 0; x < 22; x++)
		{
			switch (board[y][x])
			{
			case -1:
				Rectangle(hdc, x * 20, y * 20, (x + 1) * 20, (y + 1) * 20);					// 테두리 한칸당 20x20 사각형
				break;
			case 2:
				SelectObject(hdc, GetStockObject(BLACK_BRUSH));								// 아이템 board[y][x]==2 검은색 안에 채우기 
				Ellipse(hdc, x * 20, y * 20, (x + 1) * 20, (y + 1) * 20);					// 아이템 타원 그리기
				SelectObject(hdc, GetStockObject(WHITE_BRUSH));								// 지렁이를 그리기 위한 브러쉬
				break;
			}
		}
	}
	SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(255, 0, 0)));								// 지렁이 머리 색 빨간색 그리기 위한 펜
	Ellipse(hdc, A_wormX[0] * 20, A_wormY[0] * 20, (A_wormX[0] + 1) * 20, (A_wormY[0] + 1) * 20);	// 지렁이 머리 그리기
	Ellipse(hdc, B_wormX[0] * 20, B_wormY[0] * 20, (B_wormX[0] + 1) * 20, (B_wormY[0] + 1) * 20);	// 지렁이 머리 그리기
	SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(0, 0, 255)));								// 지렁이 몸통 색 파란색 그리기 위한 팬

	for (i = 1; i < A_len; i++)
		Ellipse(hdc, A_wormX[i] * 20, A_wormY[i] * 20, (A_wormX[i] + 1) * 20, (A_wormY[i] + 1) * 20);	// 지렁이 몸통 len 만큼 그리기

	for (i = 1; i < B_len; i++)
		Ellipse(hdc, B_wormX[i] * 20, B_wormY[i] * 20, (B_wormX[i] + 1) * 20, (B_wormY[i] + 1) * 20);	// 지렁이 몸통 len 만큼 그리기

}

void Score(HDC hdc) {													// 스코어 출력하는 함수
	TCHAR A_str[128];													// A지렁이의 길이를 문자열로 바꾸는 배열
	TCHAR B_str[128];													// B지렁이의 길이를 문자열로 바꾸는 배열
	int A_score = A_len;												// A스코어 변수 할당
	int B_score = B_len;												// B스코어 변수 할당

	wsprintf(A_str, TEXT("A 점수는 %d점 입니다."), A_score - 2);		// A 점수 출력
	TextOut(hdc, 50, 450, A_str, lstrlen(A_str));						// (50, 450) 위치에 출력
	wsprintf(B_str, TEXT("B 점수는 %d점 입니다."), B_score - 2);		// B 점수 출력
	TextOut(hdc, 250, 450, B_str, lstrlen(B_str));						// (250, 450) 위치에 출력
}

void Stopwatch(HDC hdc, HWND hwnd) {
	int seconds = 0;													// 시간 할당 변수 선언
	seconds = count;													// 전역변수 count 를 지역변수 seconds에 할당
	if (count > 0)														// 1초에 10씩 줄어드는 count 가 0보다 크다면
	{
		wsprintf(tBuffer, TEXT("남은 시간: %d"), seconds);				// 남은 시간 출력
		TextOut(hdc, 150, 500, tBuffer, lstrlen(tBuffer));				// (150, 500) 위치에 출력

	}
	else
	{																	// count 가 0보다 작다면
		wsprintf(result, TEXT("A기록 : %d점,  B기록 : %d점"), A_len-2, B_len-2);	// A와 B의 점수 출력
		TextOut(hdc, 150, 150, result, lstrlen(result));				// (150, 150) 위치에 출력
		if (A_len > B_len) {											// A 지렁이의 길이와 B 지렁이의 길이 비교해서 A지렁이가 크다면
			wsprintf(winner, TEXT("승자는 A입니다"));					// 승자 A 출력
			TextOut(hdc, 150, 250, winner, lstrlen(winner));			// (150, 250) 위치에 출력
		}
		else if (A_len < B_len) {										// A 지렁이의 길이와 B 지렁이의 길이 비교해서 B지렁이가 크다면
			wsprintf(winner, TEXT("승자는 B입니다"));					// 승자 B 출력
			TextOut(hdc, 150, 250, winner, lstrlen(winner));			// (150, 250) 위치에 출력
		}
		else {															// A 지렁이의 길이와 B 지렁이의 길이 비교해서 같다면
			wsprintf(winner, TEXT("무승부 입니다."));					// 무승부 출력
			TextOut(hdc, 150, 250, winner, lstrlen(winner));			// (150, 250) 위치에 출력
		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (iMsg)
	{
	case WM_CREATE:
		GameInit();
		SetTimer(hwnd, 1, 100, NULL);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		Stopwatch(hdc, hwnd);
		Score(hdc);
		DrawGameBoard(hdc);
		EndPaint(hwnd, &ps);
		break;
	case WM_KEYDOWN:
		DirectControl((int)wParam);
		break;
	case WM_TIMER:
		MovingWorm();
		count--;
		InvalidateRgn(hwnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		KillTimer(hwnd, 1);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}