#include "RedBlackTree.h"
#include "RedBlackTreeGDI.h"
#include "Globals.h"
#include "RedBlackTreeTest.h"
#include <windows.h>
#include <iostream>
#include <random>
#include <psapi.h> // GetProcessMemoryInfo()

#define WM_STRESS_TEST (WM_USER + 1)
#define IDC_EDIT_INPUT 101
#define IDC_BTN_INSERT 102
#define IDC_BTN_DELETE 103

const wchar_t szTitle[] = L"ViewerBinaryTree";
const wchar_t szWindowClass[] = L"MyBasicWindowClass";

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);


int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // 창 표시 상태 기본값
    int nCmdShow = SW_SHOWDEFAULT;

    // 윈도우 창의 기본 정보 등록
    MyRegisterClass(hInstance);

    // 윈도우 창 생성 및 화면 보여주기, WM_PAINT 발생시켜서 화면을 업데이트하는 용도
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;

    // 기본 메시지 루프
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        
        TranslateMessage(&msg);
        DispatchMessage(&msg);

    }

    return (int)msg.wParam;

}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;

    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // C++11 난수 생성기 초기화 (Mersenne Twister)
    // main문이 아닌 static 지역 변수선언으로 프로그램 실행되는 동안 소멸하지 않고 최초 초기화 1회만 함으로써 성능저하도 해결
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dis(1, 100000); // 데이터가 들어간 범위 설정

    switch (message)
    {
    case WM_CREATE:
    {
        // 입력용 Edit 컨트롤 (숫자만 입력되도록 ES_NUMBER 스타일 적용)
        CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            10, 10, 100, 25, hWnd, (HMENU)IDC_EDIT_INPUT, NULL, NULL);

        // 삽입 버튼
        CreateWindowW(L"BUTTON", L"Insert", WS_CHILD | WS_VISIBLE,
            120, 10, 80, 25, hWnd, (HMENU)IDC_BTN_INSERT, NULL, NULL);

        // 삭제 버튼
        CreateWindowW(L"BUTTON", L"Delete", WS_CHILD | WS_VISIBLE,
            210, 10, 80, 25, hWnd, (HMENU)IDC_BTN_DELETE, NULL, NULL);

        // 생성하자마자 테스트를 위함
        for (int i = 0; i < 10000; i++)
        {
            g_tree.InsertNode(dis(gen));
        }
        // 테스트시 활성화
        PostMessage(hWnd, WM_STRESS_TEST, 0, 0);
        
    }
    break;
    case WM_STRESS_TEST:
    {
        // 5번의 삭제와 5번의 삽입
        // 삭제와, 삽입 이후에 정확하게 정렬되어 있는지 검증
        StressTestFuc(gen);

        // 여러 번 반복했을 때 에러가 발생하는지, 정렬이 잘 되는지 검증
        PostMessage(hWnd, WM_STRESS_TEST, 0, 0);

        break;
    }
    // 추가된 버튼 클릭 처리 부분
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (wmId == IDC_BTN_INSERT || wmId == IDC_BTN_DELETE)
        {
            wchar_t buffer[256] = { 0, };
            // Edit 컨트롤에서 텍스트 가져오기
            GetWindowTextW(GetDlgItem(hWnd, IDC_EDIT_INPUT), buffer, 256);

            int value = _wtoi(buffer); // 문자열을 정수로 변환

            if (value > 0)
            {
                if (wmId == IDC_BTN_INSERT)
                    g_tree.InsertNode(value);
                else
                    g_tree.DeleteNode(value);

                // 동작 완료 후 입력창 비우기
                SetWindowTextW(GetDlgItem(hWnd, IDC_EDIT_INPUT), L"");

                // 화면 갱신 (WM_PAINT 유도)
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
    }
    break;
    case WM_LBUTTONDOWN:
    {
        g_isDragging = true;
        // 마우스를 클릭한 순간의 좌표를 기억
        g_lastMousePos.x = LOWORD(lParam);
        g_lastMousePos.y = HIWORD(lParam);

        // 마우스가 창 밖으로 빠르게 벗어나도 드래그 상태를 잃지 않도록 마우스 캡처
        SetCapture(hWnd);
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (g_isDragging)
        {
            // 현재 마우스 좌표
            int currentX = LOWORD(lParam);
            int currentY = HIWORD(lParam);

            // 마우스가 이동한 거리만큼 카메라 좌표 이동
            g_cameraX += (currentX - g_lastMousePos.x);
            g_cameraY += (currentY - g_lastMousePos.y);

            // 다음 이동 계산을 위해 현재 위치를 갱신
            g_lastMousePos.x = currentX;
            g_lastMousePos.y = currentY;

            // 화면 갱신 (WM_PAINT 호출)
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        g_isDragging = false;
        ReleaseCapture(); // 마우스 캡처 해제
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        int startX = 500 + g_cameraX;
        int startY = 50 + g_cameraY;
        int initialXOffset = 200;

        // 화면 가로 500, 세로 50 위치를 Root로 잡고 그리기 시작
        // 자식으로 갈때마다 X는 150씩 좁아지고, Y는 70씩 내려감
        DrawTreeGDI(hdc, g_tree.GetRoot(), startX, startY, initialXOffset, 70);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
    {
        // 창이 닫힐 때 트리는 소멸자(~BinaryTree)에 의해 자동으로 메모리 해제됨
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}