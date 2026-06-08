#include "BinaryTree.h"
#include "Profiler.h"
#include <windows.h>
#include <iostream>
#include <random>
#include <psapi.h> // GetProcessMemoryInfo()

#define WM_STRESS_TEST (WM_USER + 1)

const wchar_t szTitle[] = L"ViewerBinaryTree";
const wchar_t szWindowClass[] = L"MyBasicWindowClass";

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void                StressTestFuc(std::mt19937& gen);

int g_cameraX = 0;          // 카메라 X축 이동 누적값
int g_cameraY = 0;          // 카메라 Y축 이동 누적값
bool g_isDragging = false;  // 마우스 왼쪽 버튼을 누르고 있는지 여부
POINT g_lastMousePos;       // 직전 마우스 커서의 위치 (이동 거리 계산용)

// 전역 변수로 트리 객체 선언
BinaryTree g_tree;

int test_count = 10000;
int test_Loop_count = 0;

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
        // 생성하자마자 
        for (int i = 0; i < 10000; i++)
        {
            g_tree.InsertNode(dis(gen));
        }

        // 테스트시 활성화
        PostMessage(hWnd, WM_STRESS_TEST, 0, 0);
    }
    break;
    case WM_STRESS_TEST:

        // 5번의 삭제와 5번의 삽입
        // 삭제와, 삽입 이후에 정확하게 정렬되어 있는지 검증
        StressTestFuc(gen);

        // 여러 번 반복했을 때 에러가 발생하는지, 정렬이 잘 되는지 검증
        PostMessage(hWnd, WM_STRESS_TEST, 0, 0);

        break;
    case WM_LBUTTONDOWN:
        g_isDragging = true;
        // 마우스를 클릭한 순간의 좌표를 기억
        g_lastMousePos.x = LOWORD(lParam);
        g_lastMousePos.y = HIWORD(lParam);

        // 마우스가 창 밖으로 빠르게 벗어나도 드래그 상태를 잃지 않도록 마우스 캡처
        SetCapture(hWnd);
        break;
    case WM_MOUSEMOVE:
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
    case WM_LBUTTONUP:
        g_isDragging = false;
        ReleaseCapture(); // 마우스 캡처 해제
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        int startX = 500 + g_cameraX;
        int startY = 50 + g_cameraY;
        int initialXOffset = 5000;

        // 화면 가로 500, 세로 50 위치를 Root로 잡고 그리기 시작
        // 자식으로 갈때마다 X는 150씩 좁아지고, Y는 70씩 내려감
        g_tree.DrawTreeGDI(hdc, g_tree.GetRoot(), startX, startY, initialXOffset, 70);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        // 창이 닫힐 때 트리는 소멸자(~BinaryTree)에 의해 자동으로 메모리 해제됨
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void StressTestFuc(std::mt19937& gen)
{
    PROFILE
    std::uniform_int_distribution<int> dis(1, 100000);
    int testLoopCount = 10000;

    for (int i = 0; i < testLoopCount; i++)
    {
        std::vector<int> targetToDelete;

        //5개의 삭제할 랜덤한 노드의 정수를 저장할 때까지
        while (targetToDelete.size() < 5)
        {

            int randomvalue1 = dis(gen);

            if (g_tree.SearchNode(randomvalue1) != nullptr)
            {
                targetToDelete.push_back(randomvalue1);
            }

        }

        // 찾은 5개의 정수 삭제
        for (int value : targetToDelete)
        {
            g_tree.DeleteNode(value);
        }

        // 5개의 랜덤 함수 삽입
        int count = 0;
        while (count < 5)
        {
            int randomvalue2 = dis(gen);

            // 랜덤한 값이 정상적으로 삽입되었으면 count++, 없으면 다음 루프
            if (g_tree.InsertNode(randomvalue2) != false)
            {
                count++;
            }
        }

        // 검증 중위순회를 통해서 값들이 삭제, 삽입에도 정렬이 보장되는지
        int prevvalue = -1; // 최소값보다 작은 값중에서도 가장 큰 값
        if (g_tree.VerifyBinarySortTree(g_tree.GetRoot(), prevvalue) == false)
        {
            std::cout << "test failed!! " << i << " Loop error" << "\n";
            return;
        }

        
    }
    test_Loop_count++;
    std::cout << "test success!! " << "Loop Count : " << test_Loop_count << "\n";
}

