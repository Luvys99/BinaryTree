#pragma once
#include "RedBlackTree.h"
#include <windows.h>

// C++ 17부터 허용되는 문법 extern 대신 inline ( 선언과 초기화를 동시에 함 )

inline int g_cameraX = 0;          // 카메라 X축 이동 누적값
inline int g_cameraY = 0;          // 카메라 Y축 이동 누적값
inline bool g_isDragging = false;  // 마우스 왼쪽 버튼을 누르고 있는지 여부
inline POINT g_lastMousePos;       // 직전 마우스 커서의 위치 (이동 거리 계산용)

// 전역 변수로 트리 객체 선언
inline RedBlacktree g_tree;