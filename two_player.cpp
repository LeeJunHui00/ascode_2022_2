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

#define ITEMNUM 10							// �������� ���� 10��

int board[22][22];							// ������ 22x22 ������ 2���� ���� �Ҵ�
int A_wormX[30], A_wormY[30];				// A�������� X��ǥ Y��ǥ ���� �Ҵ�
int A_xDirect, A_yDirect, A_len;			// A�������� ���� �� ���� ����

int B_wormX[30], B_wormY[30];				// B�������� X��ǥ Y��ǥ ���� �Ҵ�
int B_xDirect, B_yDirect, B_len;			// B�������� ���� �� ���� ����

TCHAR tBuffer[128];							// ���� �ð� ����� ���� �迭 �Ҵ�
TCHAR result[128];							// A������ B���� ����� ���� �迭
TCHAR winner[128];							// A������ B������ ũ�� �� �� ���� ����� ���� �迭
static int count = 100;						// 100(10��)���� ���� �پ��� �����ġ

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


void GameInit() {							// �ʱ� ������ ����
	int i;
	for (i = 0; i < 22; i++)				// ������ �׵θ� �κ��� -1�� �Ҵ��ϴ� �ݺ���
	{
		board[i][0] = -1;
		board[i][21] = -1;
		board[0][i] = -1;
		board[21][i] = -1;
	}

	A_wormX[0] = 2;		A_wormY[0] = 1;		// A�������� �Ӹ� �ʱ� ��ġ (2,1)
	A_wormX[1] = 1;		A_wormY[1] = 1;		// A�������� ���� �ʱ� ��ġ (1,1)
	board[A_wormY[0]][A_wormX[0]] = 3;		// �������� �Ӹ� ��ġ�� ������ ���� 3���� �Ҵ�
	board[A_wormY[1]][A_wormX[1]] = 3;		// �������� ���� ��ġ�� ������ ���� 3���� �Ҵ�

	B_wormX[0] = 19;	B_wormY[0] = 20;	// B�������� �Ӹ� �ʱ� ��ġ (19,20)
	B_wormX[1] = 20;	B_wormY[1] = 20;	// B�������� ���� �ʱ� ��ġ (20,20)
	board[B_wormY[0]][B_wormX[0]] = 3;		// �������� �Ӹ� ��ġ�� ������ ���� 3���� �Ҵ�
	board[B_wormY[1]][B_wormX[1]] = 3;		// �������� ���� ��ġ�� ������ ���� 3���� �Ҵ�

	ItemGenerator();						// ������ ������ ���� �Լ�

	A_len = 2;								// A �������� �� ���̴� 2
	B_len = 2;								// B �������� �� ���̴� 2

	A_xDirect = 1;	A_yDirect = 0;			// xDirect�� 1 yDirect�� 0 �� ���� �������ڸ��� ���������� ������
	B_xDirect = -1;	B_yDirect = 0;			// xDirect�� -1 yDirect�� 0 �� ���� �������ڸ��� �������� ������
}

void DirectControl(int DirectKey)						// param ���� Ű �Է� �޾� ����Ű ���ϴ� �Լ�
{
	switch (DirectKey) {								// param Ű�� �Է� ������
	case VK_LEFT:										// ���� ����Ű �Է� ��
		if (A_xDirect == 1)								// �̹� xDirect �� 1�̶�� ���� ���������� �̵�����
			break;
		if (board[A_wormY[0]][A_wormX[0] - 1] != -1)	// ��(-1) �� �ƴ϶��
		{
			A_xDirect = -1;								// xDirect = -1 �� �������� �̵���
			A_yDirect = 0;
		}
		break;
	case VK_RIGHT:										// ������ ����Ű �Է� ��
		if (A_xDirect == -1)
			break;
		if (board[A_wormY[0]][A_wormX[0] + 1] != -1)	// ��(-1) �� �ƴ϶��
		{
			A_xDirect = 1;								// xDirect = 1 �� ���������� �̵���
			A_yDirect = 0;
		}
		break;
	case VK_UP:											// ���� ����Ű �Է� ��
		if (A_yDirect == 1)
			break;
		if (board[A_wormY[0] - 1][A_wormX[0]] != -1)	// ��(-1) �� �ƴ϶��
		{
			A_xDirect = 0;
			A_yDirect = -1;								// yDirect = -1 �� ���� �̵���
		}
		break;
	case VK_DOWN:
		if (A_yDirect == -1)							// �Ʒ��� ����Ű �Է� ��
			break;
		if (board[A_wormY[0] + 1][A_wormX[0]] != -1)	// ��(-1) �� �ƴ϶��
		{
			A_xDirect = 0;
			A_yDirect = 1;								// yDirect = 1 �� �Ʒ��� �̵���
		}
		break;
	case VK_NUMPAD4:
		if (B_xDirect == 1)								// �̹� xDirect �� 1�̶�� ���� ���������� �̵�����
			break;
		if (board[B_wormY[0]][B_wormX[0] - 1] != -1)	// ��(-1) �� �ƴ϶��
		{
			B_xDirect = -1;								// xDirect = -1 �� �������� �̵���
			B_yDirect = 0;
		}
		break;
	case VK_NUMPAD6:
		if (B_xDirect == -1)
			break;
		if (board[B_wormY[0]][B_wormX[0] + 1] != -1)	// ��(-1) �� �ƴ϶��
		{
			B_xDirect = 1;								// xDirect = 1 �� ���������� �̵���
			B_yDirect = 0;
		}
		break;
	case VK_NUMPAD8:
		if (B_yDirect == 1)
			break;
		if (board[B_wormY[0] - 1][B_wormX[0]] != -1)	// ��(-1) �� �ƴ϶��
		{
			B_xDirect = 0;
			B_yDirect = -1;								// yDirect = 1 �� ���� �̵���
		}
		break;
	case VK_NUMPAD5:
		if (B_yDirect == -1)
			break;
		if (board[B_wormY[0] + 1][B_wormX[0]] != -1)	// ��(-1) �� �ƴ϶��
		{
			B_xDirect = 0;
			B_yDirect = 1;								// yDirect = 1 �� �Ʒ��� �̵���
		}
		break;
	}
}			

void MovingWorm() 
{
	int A_tmpx, A_tmpy, i;								// ������ A�������� X��ǥ, Y��ǥ ������ ���� ����
	int B_tmpx, B_tmpy;									// ������ B�������� X��ǥ, Y��ǥ ������ ���� ����

	A_tmpx = A_wormX[0];								// ���� A�������� X��ǥ �Ҵ�
	A_tmpy = A_wormY[0];								// ���� A�������� Y��ǥ �Ҵ�

	A_wormX[0] += A_xDirect;							// ���� A�������� ����(����:-1, ������ : +1)�� ���� �Ӹ� X��ǥ�� ����
	A_wormY[0] += A_yDirect;							// ���� A�������� ����(����:-1, �Ʒ��� : +1)�� ���� �Ӹ� Y��ǥ�� ����

	if (board[A_wormY[0]][A_wormX[0]] == -1 || board[A_wormY[0]][A_wormX[0]] == 3)		// ���� ��ġ�� ���̰ų� �����̰� �ִٸ�
	{
		A_wormX[0] = A_tmpx;							// �ٽ� ���� X��ǥ�� ���ư�
		A_wormY[0] = A_tmpy;							// �ٽ� ���� Y��ǥ�� ���ư�
	}
	else
	{
		if (board[A_wormY[0]][A_wormX[0]] == 2)				// ��Ʈ�ǿ� �������� ������
		{
			A_len = A_len + 1;								// ���� �ø���
			board[A_wormY[0]][A_wormX[0]] = 0;				// ������ ����
		}
		else
			board[A_wormY[A_len - 1]][A_wormX[A_len - 1]] = 0;		// �������� �������� -1 ��ġ ������ ����

		for (i = A_len - 1; i > 1; i--)						// ��ĭ�� �������� �ϱ� ������ ���� ��ġ ��ĭ�� �����̱�
		{
			A_wormX[i] = A_wormX[i - 1];
			A_wormY[i] = A_wormY[i - 1];
		}
		A_wormX[1] = A_tmpx;								// ��ĭ�� �����̱�
		A_wormY[1] = A_tmpy;
		board[A_wormY[0]][A_wormX[0]] = 3;					// ������ �ִ� ��ġ�� board[][] = 3 ���� �ٲٱ�
	}


	B_tmpx = B_wormX[0];									// ���� B�������� X��ǥ �Ҵ�
	B_tmpy = B_wormY[0];									// ���� B�������� Y��ǥ �Ҵ�

	B_wormX[0] += B_xDirect;								// ���� B�������� ����(����:-1, ������ : +1)�� ���� �Ӹ� X��ǥ�� ����
	B_wormY[0] += B_yDirect;								// ���� B�������� ����(����:-1, �Ʒ��� : +1)�� ���� �Ӹ� Y��ǥ�� ����

	if (board[B_wormY[0]][B_wormX[0]] == -1 || board[B_wormY[0]][B_wormX[0]] == 3)		// ���� ��ġ�� ���̰ų� �����̰� �ִٸ�
	{
		B_wormX[0] = B_tmpx;								// �ٽ� ���� X��ǥ�� ���ư�
		B_wormY[0] = B_tmpy;								// �ٽ� ���� Y��ǥ�� ���ư�
	}
	else
	{
		if (board[B_wormY[0]][B_wormX[0]] == 2)				// ��Ʈ�ǿ� �������� ������
		{
			B_len = B_len + 1;								// ���� �ø���
			board[B_wormY[0]][B_wormX[0]] = 0;				// ������ ����
		}
		else
			board[B_wormY[B_len - 1]][B_wormX[B_len - 1]] = 0;		// �������� �������� -1 ��ġ ������ ����

		for (i = B_len - 1; i > 1; i--)						// ��ĭ�� �������� �ϱ� ������ ���� ��ġ ��ĭ�� �����̱�
		{
			B_wormX[i] = B_wormX[i - 1];
			B_wormY[i] = B_wormY[i - 1];
		}
		B_wormX[1] = B_tmpx;								// ��ĭ�� �����̱�
		B_wormY[1] = B_tmpy;
		board[B_wormY[0]][B_wormX[0]] = 3;					// ������ �ִ� ��ġ�� board[][] = 3 ���� �ٲٱ�
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
	Ellipse(hdc, A_wormX[0] * 20, A_wormY[0] * 20, (A_wormX[0] + 1) * 20, (A_wormY[0] + 1) * 20);	// ������ �Ӹ� �׸���
	Ellipse(hdc, B_wormX[0] * 20, B_wormY[0] * 20, (B_wormX[0] + 1) * 20, (B_wormY[0] + 1) * 20);	// ������ �Ӹ� �׸���
	SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(0, 0, 255)));								// ������ ���� �� �Ķ��� �׸��� ���� ��

	for (i = 1; i < A_len; i++)
		Ellipse(hdc, A_wormX[i] * 20, A_wormY[i] * 20, (A_wormX[i] + 1) * 20, (A_wormY[i] + 1) * 20);	// ������ ���� len ��ŭ �׸���

	for (i = 1; i < B_len; i++)
		Ellipse(hdc, B_wormX[i] * 20, B_wormY[i] * 20, (B_wormX[i] + 1) * 20, (B_wormY[i] + 1) * 20);	// ������ ���� len ��ŭ �׸���

}

void Score(HDC hdc) {													// ���ھ� ����ϴ� �Լ�
	TCHAR A_str[128];													// A�������� ���̸� ���ڿ��� �ٲٴ� �迭
	TCHAR B_str[128];													// B�������� ���̸� ���ڿ��� �ٲٴ� �迭
	int A_score = A_len;												// A���ھ� ���� �Ҵ�
	int B_score = B_len;												// B���ھ� ���� �Ҵ�

	wsprintf(A_str, TEXT("A ������ %d�� �Դϴ�."), A_score - 2);		// A ���� ���
	TextOut(hdc, 50, 450, A_str, lstrlen(A_str));						// (50, 450) ��ġ�� ���
	wsprintf(B_str, TEXT("B ������ %d�� �Դϴ�."), B_score - 2);		// B ���� ���
	TextOut(hdc, 250, 450, B_str, lstrlen(B_str));						// (250, 450) ��ġ�� ���
}

void Stopwatch(HDC hdc, HWND hwnd) {
	int seconds = 0;													// �ð� �Ҵ� ���� ����
	seconds = count;													// �������� count �� �������� seconds�� �Ҵ�
	if (count > 0)														// 1�ʿ� 10�� �پ��� count �� 0���� ũ�ٸ�
	{
		wsprintf(tBuffer, TEXT("���� �ð�: %d"), seconds);				// ���� �ð� ���
		TextOut(hdc, 150, 500, tBuffer, lstrlen(tBuffer));				// (150, 500) ��ġ�� ���

	}
	else
	{																	// count �� 0���� �۴ٸ�
		wsprintf(result, TEXT("A��� : %d��,  B��� : %d��"), A_len-2, B_len-2);	// A�� B�� ���� ���
		TextOut(hdc, 150, 150, result, lstrlen(result));				// (150, 150) ��ġ�� ���
		if (A_len > B_len) {											// A �������� ���̿� B �������� ���� ���ؼ� A�����̰� ũ�ٸ�
			wsprintf(winner, TEXT("���ڴ� A�Դϴ�"));					// ���� A ���
			TextOut(hdc, 150, 250, winner, lstrlen(winner));			// (150, 250) ��ġ�� ���
		}
		else if (A_len < B_len) {										// A �������� ���̿� B �������� ���� ���ؼ� B�����̰� ũ�ٸ�
			wsprintf(winner, TEXT("���ڴ� B�Դϴ�"));					// ���� B ���
			TextOut(hdc, 150, 250, winner, lstrlen(winner));			// (150, 250) ��ġ�� ���
		}
		else {															// A �������� ���̿� B �������� ���� ���ؼ� ���ٸ�
			wsprintf(winner, TEXT("���º� �Դϴ�."));					// ���º� ���
			TextOut(hdc, 150, 250, winner, lstrlen(winner));			// (150, 250) ��ġ�� ���
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