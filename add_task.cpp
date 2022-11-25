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


void ItemGenerator()						// ������ ������ ���� �Լ�
{
	int i, x, y;
	for (i = 0; i < ITEMNUM; i++)			// �������� ���� �� 10��
	{
		x = rand() % 20 + 1;				// x�� ��ǥ ������ 22, rand()%20 ������� 19���� �̹Ƿ� ������ ���� �׵θ� +1
		y = rand() % 20 + 1;				// y�� ��ǥ ������ 22, rand()%20 ������� 19���� �̹Ƿ� ������ ���� �׵θ� +1
		if (board[y][x] == 0)
			board[y][x] = 2;				// board[y][x] �� 0 �� �������� ���� ����, 2�� �������� �ִ� ����
		else
		{							
			i = i - 1;						// �������� �� �� ���������� -1 ��ŭ for���� ���ƾ���
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
				Rectangle(hdc, x * 20, y * 20, (x + 1) * 20, (y + 1) * 20);					// �׵θ� ��ĭ�� 20x20 �簢��
				break;
			case 2:
				SelectObject(hdc, GetStockObject(BLACK_BRUSH));								// ������ board[y][x]==2 ������ �ȿ� ä��� 
				Ellipse(hdc, x * 20, y * 20, (x + 1) * 20, (y + 1) * 20);					// ������ Ÿ�� �׸���
				SelectObject(hdc, GetStockObject(WHITE_BRUSH));								// �����̸� �׸��� ���� �귯��
				break;
			}
		}
	}

	SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(255, 0, 0)));								// ������ �Ӹ� �� ������ �׸��� ���� ��
	Ellipse(hdc, wormX[0] * 20, wormY[0] * 20, (wormX[0] + 1) * 20, (wormY[0] + 1) * 20);	// ������ �Ӹ� �׸���
	SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(0, 0, 255)));								// ������ ���� �� �Ķ��� �׸��� ���� ��

	for (i = 1; i < len; i++)
		Ellipse(hdc, wormX[i] * 20, wormY[i] * 20, (wormX[i] + 1) * 20, (wormY[i] + 1) * 20);	// ������ ���� len ��ŭ �׸���
}


void GameInit()
{
	int i;
	for (i = 0; i < 22; i++)			// ������ �׵θ� �κ� -1�� ����
	{
		board[i][0] = -1;
		board[i][21] = -1;
		board[0][i] = -1;
		board[21][i] = -1;
	}
	wormX[0] = 2; wormY[0] = 1;			// ������ �ʱ� �Ӹ� ��ġ (2,1)
	wormX[1] = 1; wormY[1] = 1;			// ������ �ʱ� ���� ��ġ (1,1)
	board[wormY[0]][wormX[0]] = 3;		// 3�� ������ �Ӹ� �� ������
	board[wormY[1]][wormX[1]] = 3;		// 3�� ������ �� �� �����δٴ� ��

	ItemGenerator();

	len = 2;							// �������� ���� = len
	xDirect = 1; yDirect = 0;			// xDirect �� 1�̸� ���������� -1�̸� ����
}


void Score(HDC hdc) {
	TCHAR str[128];
	int score = len;
	wsprintf(str, TEXT("���� ������ %d�� �Դϴ�."), score - 2);
	TextOut(hdc, 350, 450, str, lstrlen(str));
}


void DirectControl(int DirectKey)					// param ���� Ű �Է� �޾� ����Ű ���ϴ� �Լ�
{
	switch (DirectKey) {							// param Ű�� �Է� ������
	case VK_LEFT:									// ����Ű �Է� ��
		if (xDirect == 1)							// �̹� xDirect �� 1�̶�� ���� ���������� �̵�����
			break;
		if (board[wormY[0]][wormX[0] - 1] != -1)	// ���� ��°� �ƴϸ� ��������
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
		if (board[wormY[0]][wormX[0]] == 2)				// ��Ʈ�ǿ� �������� ������
		{
			len = len + 1;								// ���� �ø���
			board[wormY[0]][wormX[0]] = 0;				// ������ ����
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