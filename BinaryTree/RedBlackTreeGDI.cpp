#include "RedBlackTreeGDI.h"
#include "RedBlackTree.h"
#include "Globals.h"

// GDI 트리 그리기 재귀 함수
void DrawTreeGDI(HDC hdc, Node* node, int x, int y, int xOffset, int yOffset) noexcept
{
    Node* NilNode = g_tree.GetNil();

    if (node == NilNode)
    {
        return;
    }

    // 1. 자식 노드로 향하는 '선(Edge)'을 먼저 그립니다.
    // 선을 먼저 그려야 나중에 그리는 '원(노드)' 뒤로 선이 깔끔하게 숨겨집니다.
    if (node->Left != NilNode)
    {
        MoveToEx(hdc, x, y, nullptr); // 현재 위치에서
        LineTo(hdc, x - xOffset, y + yOffset); // 왼쪽 아래로 선 긋기

        // 왼쪽 자식 재귀 호출 (X 간격을 반으로 줄임)
        DrawTreeGDI(hdc, node->Left, x - xOffset, y + yOffset, xOffset / 2, yOffset);
    }

    if (node->Right != NilNode)
    {
        MoveToEx(hdc, x, y, nullptr); // 현재 위치에서
        LineTo(hdc, x + xOffset, y + yOffset); // 오른쪽 아래로 선 긋기

        // 오른쪽 자식 재귀 호출 (X 간격을 반으로 줄임)
        DrawTreeGDI(hdc, node->Right, x + xOffset, y + yOffset, xOffset / 2, yOffset);
    }

    // 2. 현재 노드의 '원(Circle)'을 그립니다.
    int radius = 15; // 원의 반지름
    HBRUSH hNodeBrush;

    if (node->Color == RED)
    {
        hNodeBrush = CreateSolidBrush(RGB(255, 100, 100)); // 가독성 좋은 연한 빨간색
    }
    else
    {
        hNodeBrush = CreateSolidBrush(RGB(80, 80, 80)); // 짙은 회색 (완전 검정이면 선이 안 보일 수 있음)
    }

    // 생성한 브러시를 DC에 적용하고 기존 브러시 백업
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hNodeBrush);

    // 기본 펜(검정)과 브러시(흰색)로 내부가 채워진 원이 그려집니다.
    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

    // 3. 원 안에 '데이터(숫자)'를 출력합니다.
    std::wstring strData = std::to_wstring(node->Data); // int를 wstring으로 변환

    SetBkMode(hdc, TRANSPARENT); // 텍스트 배경을 투명하게 (원 색상을 가리지 않게)
    SetTextAlign(hdc, TA_CENTER | TA_BASELINE); // 텍스트를 좌표의 정중앙에 정렬

    // 어두운 배경(빨강, 회색) 위에서도 글씨가 잘 보이도록 텍스트 색상을 흰색으로 변경
    COLORREF oldTextColor = SetTextColor(hdc, RGB(255, 255, 255));

    // y 좌표를 살짝 보정하여 원의 중앙에 오도록 맞춤
    TextOutW(hdc, x, y + 5, strData.c_str(), strData.length());

    // 4. (매우 중요) 사용한 GDI 리소스 원상복구 및 메모리 해제
    SetTextColor(hdc, oldTextColor); // 글자색 원래대로 복구
    SelectObject(hdc, hOldBrush);    // 브러시 원래대로 복구
    DeleteObject(hNodeBrush);        // ★ 생성했던 색상 브러시 삭제 (메모리 누수 방지)
}