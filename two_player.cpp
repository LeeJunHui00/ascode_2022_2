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

#define ITEMNUM 10

int board[22][22];
int A_wormX[30], A_wormY[30];
int A_xDirect, A_yDirect, A_len;

int B_wormX[30], B_wormY[30];
int B_xDirect, B_yDirect, B_len;

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


void GameInit() {
	int i;
	for (i = 0; i < 22; i++) 
	{
		board[i][0] = -1;
		board[i][21] = -1;
		board[0][i] = -1;
		board[21][i] = -1;
	}

	A_wormX[0] = 2;		A_wormY[0] = 1;
	A_wormX[1] = 1;		A_wormY[1] = 1;
	board[A_wormY[0]][A_wormX[0]] = 3;
	board[A_wormY[1]][A_wormX[1]] = 3;

	B_wormX[0] = 19;	B_wormY[0] = 20;
	B_wormX[1] = 20;	B_wormY[1] = 20;
	board[B_wormY[0]][B_wormX[0]] = 3;
	board[B_wormY[1]][B_wormX[1]] = 3;

	ItemGenerator();

	A_len = 2;
	B_len = 2;

	A_xDirect = 1;	A_yDirect = 0;
	B_xDirect = -1;	B_yDirect = 0;
}

void DirectControl(int DirectKey)		// param 으로 키 입력 받아 방향키 정하는 함수
{
	switch (DirectKey) {							// param 키가 입력 받으면
	case VK_LEFT:									// 왼쪽키 입력 시
		if (A_xDirect == 1)							// 이미 xDirect 가 1이라는 뜻은 오른쪽으로 이동상태
			break;
		if (board[A_wormY[0]][A_wormX[0] - 1] != -1)	// 벽에 닿는거 아니면 왼쪽으로
		{
			A_xDirect = -1;
			A_yDirect = 0;
		}
		break;
	case VK_RIGHT:
		if (A_xDirect == -1)
			break;
		if (board[A_wormY[0]][A_wormX[0] + 1] != -1)
		{
			A_xDirect = 1;
			A_yDirect = 0;
		}
		break;
	case VK_UP:
		if (A_yDirect == 1)
			break;
		if (board[A_wormY[0] - 1][A_wormX[0]] != -1)
		{
			A_xDirect = 0;
			A_yDirect = -1;
		}
		break;
	case VK_DOWN:
		if (A_yDirect == -1)
			break;
		if (board[A_wormY[0] + 1][A_wormX[0]] != -1)
		{
			A_xDirect = 0;
			A_yDirect = 1;
		}
		break;
	case VK_NUMPAD4:
		if (B_xDirect == 1)							// 이미 xDirect 가 1이라는 뜻은 오른쪽으로 이동상태
			break;
		if (board[B_wormY[0]][B_wormX[0] - 1] != -1)	// 벽에 닿는거 아니면 왼쪽으로
		{
			B_xDirect = -1;
			B_yDirect = 0;
		}
		break;
	case VK_NUMPAD6:
		if (B_xDirect == -1)
			break;
		if (board[B_wormY[0]][B_wormX[0] + 1] != -1)
		{
			B_xDirect = 1;
			B_yDirect = 0;
		}
		break;
	case VK_NUMPAD8:
		if (B_yDirect == 1)
			break;
		if (board[B_wormY[0] - 1][B_wormX[0]] != -1)
		{
			B_xDirect = 0;
			B_yDirect = -1;
		}
		break;
	case VK_NUMPAD5:
		if (B_yDirect == -1)
			break;
		if (board[B_wormY[0] + 1][B_wormX[0]] != -1)
		{
			B_xDirect = 0;
			B_yDirect = 1;
		}
		break;
	}
}			

void MovingWorm() 
{
	int A_tmpx, A_tmpy, i;
	int B_tmpx, B_tmpy;

	A_tmpx = A_wormX[0];
	A_tmpy = A_wormY[0];

	A_wormX[0] += A_xDirect;
	A_wormY[0] += A_yDirect;

	if (board[A_wormY[0]][A_wormX[0]] == -1 || board[A_wormY[0]][A_wormX[0]] == 3)
	{
		A_wormX[0] = A_tmpx;
		A_wormY[0] = A_tmpy;
	}
	else
	{
		if (board[A_wormY[0]][A_wormX[0]] == 2)				// 보트판에 아이템이 있으면
		{
			A_len = A_len + 1;								// 꼬리 늘리고
			board[A_wormY[0]][A_wormX[0]] = 0;				// 아이템 삭제
		}
		else
			board[A_wormY[A_len - 1]][A_wormX[A_len - 1]] = 0;

		for (i = A_len - 1; i > 1; i--)
		{
			A_wormX[i] = A_wormX[i - 1];
			A_wormY[i] = A_wormY[i - 1];
		}
		A_wormX[1] = A_tmpx;
		A_wormY[1] = A_tmpy;
		board[A_wormY[0]][A_wormX[0]] = 3;
	}


	B_tmpx = B_wormX[0];
	B_tmpy = B_wormY[0];

	B_wormX[0] += B_xDirect;
	B_wormY[0] += B_yDirect;

	if (board[B_wormY[0]][B_wormX[0]] == -1 || board[B_wormY[0]][B_wormX[0]] == 3)
	{
		B_wormX[0] = B_tmpx;
		B_wormY[0] = B_tmpy;
	}
	else
	{
		if (board[B_wormY[0]][B_wormX[0]] == 2)				// 보트판에 아이템이 있으면
		{
			B_len = B_len + 1;								// 꼬리 늘리고
			board[B_wormY[0]][B_wormX[0]] = 0;				// 아이템 삭제
		}
		else
			board[B_wormY[B_len - 1]][B_wormX[B_len - 1]] = 0;

		for (i = B_len - 1; i > 1; i--)
		{
			B_wormX[i] = B_wormX[i - 1];
			B_wormY[i] = B_wormY[i - 1];
		}
		B_wormX[1] = B_tmpx;
		B_wormY[1] = B_tmpy;
		board[B_wormY[0]][B_wormX[0]] = 3;
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

void Score(HDC hdc) {
	TCHAR A_str[128];
	TCHAR B_str[128];
	int A_score = A_len;
	int B_score = B_len;

	wsprintf(A_str, TEXT("A 점수는 %d점 입니다."), A_score - 2);
	TextOut(hdc, 50, 450, A_str, lstrlen(A_str));
	wsprintf(B_str, TEXT("B 점수는 %d점 입니다."), B_score - 2);
	TextOut(hdc, 250, 450, B_str, lstrlen(B_str));
}

TCHAR tBuffer[128];
TCHAR result[128];
TCHAR winner[128];
static int count = 100;
int seconds = 0;
void Stopwatch(HDC hdc, HWND hwnd) {
	if (count > 0)
	{
		seconds = count;
		wsprintf(tBuffer, TEXT("남은 시간: %d"), seconds);
		TextOut(hdc, 150, 500, tBuffer, lstrlen(tBuffer));

	}
	else
	{
		wsprintf(result, TEXT("A기록 : %d점,  B기록 : %d점"), A_len-2, B_len-2);
		TextOut(hdc, 150, 150, result, lstrlen(result));
		if (A_len > B_len) {
			wsprintf(winner, TEXT("승자는 A입니다"));
			TextOut(hdc, 150, 250, winner, lstrlen(winner));
		}
		else if (A_len < B_len) {
			wsprintf(winner, TEXT("승자는 B입니다"));
			TextOut(hdc, 150, 250, winner, lstrlen(winner));
		}
		else {
			wsprintf(winner, TEXT("무승부 입니다."));
			TextOut(hdc, 150, 250, winner, lstrlen(winner));
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