#include <windows.h>
#include <TCHAR.H>

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
int wormX[30], wormY[30];
int xDirect, yDirect, len;


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
	Ellipse(hdc, wormX[0] * 20, wormY[0] * 20, (wormX[0] + 1) * 20, (wormY[0] + 1) * 20);	// 지렁이 머리 그리기
	SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(0, 0, 255)));								// 지렁이 몸통 색 파란색 그리기 위한 팬

	for (i = 1; i < len; i++)
		Ellipse(hdc, wormX[i] * 20, wormY[i] * 20, (wormX[i] + 1) * 20, (wormY[i] + 1) * 20);	// 지렁이 몸통 len 만큼 그리기
}


void GameInit()
{
	int i;
	for (i = 0; i < 22; i++)			// 보드판 테두리 부분 -1로 설정
	{
		board[i][0] = -1;
		board[i][21] = -1;
		board[0][i] = -1;
		board[21][i] = -1;
	}
	wormX[0] = 2; wormY[0] = 1;			// 지렁이 초기 머리 위치 (2,1)
	wormX[1] = 1; wormY[1] = 1;			// 지렁이 초기 꼬리 위치 (1,1)
	board[wormY[0]][wormX[0]] = 3;		// 3은 지렁이 머리 가 움직임
	board[wormY[1]][wormX[1]] = 3;		// 3은 지렁이 몸 가 움직인다는 뜻

	ItemGenerator();

	len = 2;							// 지렁이의 길이 = len
	xDirect = 1; yDirect = 0;			// xDirect 가 1이면 오른쪽으로 -1이면 왼쪽
}


void Score(HDC hdc) {
	TCHAR str[128];
	int score = len;
	wsprintf(str, TEXT("현재 점수는 %d점 입니다."), score - 2);
	TextOut(hdc, 350, 450, str, lstrlen(str));
}


void DirectControl(int DirectKey)					// param 으로 키 입력 받아 방향키 정하는 함수
{
	switch (DirectKey) {							// param 키가 입력 받으면
	case VK_LEFT:									// 왼쪽키 입력 시
		if (xDirect == 1)							// 이미 xDirect 가 1이라는 뜻은 오른쪽으로 이동상태
			break;
		if (board[wormY[0]][wormX[0] - 1] != -1)	// 벽에 닿는거 아니면 왼쪽으로
		{
			xDirect = -1;
			yDirect = 0;
		}
		break;
	case VK_RIGHT:
		if (xDirect == -1)
			break;
		if (board[wormY[0]][wormX[0] + 1] != -1)
		{
			xDirect = 1;
			yDirect = 0;
		}
		break;
	case VK_UP:
		if (yDirect == 1)
			break;
		if (board[wormY[0] - 1][wormX[0]] != -1)
		{
			xDirect = 0;
			yDirect = -1;
		}
		break;
	case VK_DOWN:
		if (yDirect == -1)
			break;
		if (board[wormY[0] + 1][wormX[0]] != -1)
		{
			xDirect = 0;
			yDirect = 1;
		}
		break;
	}
}


void MovingWorm()
{
	int tmpx, tmpy, i;

	tmpx = wormX[0];
	tmpy = wormY[0];
	wormX[0] += xDirect; 
	wormY[0] += yDirect;

	if (board[wormY[0]][wormX[0]] == -1 || board[wormY[0]][wormX[0]] == 3)
	{
		wormX[0] = tmpx;
		wormY[0] = tmpy;
	}
	else
	{
		if (board[wormY[0]][wormX[0]] == 2)				// 보트판에 아이템이 있으면
		{
			len = len + 1;								// 꼬리 늘리고
			board[wormY[0]][wormX[0]] = 0;				// 아이템 삭제
		}
		else
			board[wormY[len - 1]][wormX[len - 1]] = 0;	

		for (i = len - 1; i > 1; i--)
		{
			wormX[i] = wormX[i - 1];
			wormY[i] = wormY[i - 1];
		}
		wormX[1] = tmpx;
		wormY[1] = tmpy;
		board[wormY[0]][wormX[0]] = 3;
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (iMsg) {
	case WM_CREATE:
		GameInit();
		SetTimer(hwnd, 1, 100, NULL);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		Score(hdc);
		DrawGameBoard(hdc);
		EndPaint(hwnd, &ps);
		break;
	case WM_KEYDOWN:
		DirectControl((int)wParam);
		break;
	case WM_TIMER:
		MovingWorm();
		InvalidateRgn(hwnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		KillTimer(hwnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}