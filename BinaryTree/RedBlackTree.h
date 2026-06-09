#pragma once

#include <windows.h>
#include <iostream>
#include <string>

enum NODE_COLOR
{
	BLACK = 0,
	RED
};

struct Node
{
	Node* Parent;
	Node* Left;
	Node* Right;
	int Data;

	NODE_COLOR Color;

	Node() : Parent(nullptr), Left(nullptr), Right(nullptr), Data(NULL), Color(RED) {}
	Node(int data) : Parent(nullptr), Left(nullptr), Right(nullptr), Data(data), Color(RED) {}
};

class RedBlackree
{

public:
	RedBlackree() noexcept;
	virtual ~RedBlackree() noexcept;

	inline bool InsertNode(int data) noexcept; // 새로운 노드 삽입
	inline void DeleteNode(int data) noexcept; // 검색한 데이터 삭제
	inline Node* SearchNode(int data) noexcept; // 검색한 데이터의 노드를 찾아서 반환
	inline bool DeleteAll(void) noexcept; // 트리 전체 삭제 ( 후위 순회 )
	// HDC, 현재 노드, 현재 X좌표, 현재 Y좌표, X좌표가 벌어질 간격, Y좌표가 내려갈 간격
	inline void DrawTreeGDI(HDC hdc, Node* node, int x, int y, int xOffset, int yOffset) const noexcept;

	// DrawTreeGDI 함수 호출할 때 Node로 넘기기 위함
	inline Node* GetRoot() const noexcept;

	// 정렬 상태 검증 함수
	inline bool VerifyBinarySortTree(Node* currentNode, int& prevvalue) noexcept;

private:
	// InsertNode 내부에서 호출되는 재귀 함수 ( 위치 찾아서 노드 추가하는 함수 )
	inline bool InsertNodeRecursive(Node* &currentNode, Node* parentNode, int data); // new는 예외를 던지기 때문에 noexcept 붙이면 안됨
	// SearchNode 내부에서 호출되는 재귀 함수 ( 노드 찾기 )
	inline Node* FindNodeRecursive(Node* currentNode, int data) noexcept;
	// DeleteNode 내부에서 호출되는 재귀 함수 ( 노드 찾기 & 삭제 & 후처리 )
	inline void deleteNodeRecursive(Node* &currentNode, int data) noexcept;
	// DeleteAll 내부에서 호출되는 재귀 함수 ( 노드 후위 순회 )
	inline void DeleteAllRecursive(Node* &currentNode) noexcept;
	// 밸런스 함수
	inline void InsertNodeBalance(Node* &currentNode, Node* parentNode) noexcept;
	inline void PrintTree(Node* currentNode) const noexcept; // 중위 순회를 통해서 정렬이 유지되고 있는지 이전 값과 비교해서 검증하는 함수

	Node* RootNode;
	Node* NilNode;
	int NodeCount;
};

inline RedBlackree::RedBlackree() noexcept : NodeCount(0)
{
	// 트리가 생성되면 한 개만 생성해서 모든 Nil노드를 가르키는 것이 하나의 Nil노드를 가르키게 되는 것
	NilNode = new Node;
	NilNode->Color = BLACK;
	RootNode = NilNode;
}

inline RedBlackree::~RedBlackree() noexcept
{
	//Tree 전체 삭제
	DeleteAll();
}

inline bool RedBlackree::InsertNode(int data) noexcept
{
	if (RootNode == nullptr)
	{
		RootNode = new Node(data);
		RootNode->Color = BLACK;
		RootNode->Left = NilNode;
		RootNode->Right = NilNode;
		return true;
	}

	return InsertNodeRecursive(RootNode, nullptr, data);
}

inline bool RedBlackree::InsertNodeRecursive(Node* &currentNode, Node* parentNode, int data)
{
	// 노드가 비어있다면 빈 자리에 노드 생성
	if (currentNode == nullptr)
	{
		// 노드는 Red 노드로 항상 리프 노드에 삽입됨, 항상 Left, Right에 NilNode 가르키도록 함
		currentNode = new Node(data);
		currentNode->Parent = parentNode;
		currentNode->Left = NilNode;
		currentNode->Right = NilNode;

		// 삽입으로 인해서 레드 블랙 트리의 균형이 깨졌는지 체크하고 밸런싱
		// 부모가 RED만 아니면 밸런스 로직을 거칠 필요가 없음
		if (currentNode->Parent->Color == RED)
		{
			InsertNodeBalance(currentNode, currentNode->Parent);
		}
		return true;
	}
	
	// 노드가 존재한다면 왼쪽, 오른쪽 방향을 결정해서 재귀 호출
	if (currentNode->Data > data)
	{
		InsertNodeRecursive(currentNode->Left, currentNode, data);
		return true;
	}
	else if (currentNode->Data < data)
	{
		InsertNodeRecursive(currentNode->Right, currentNode, data);
		return true;
	}
	
	return false;
}

inline void RedBlackree::InsertNodeBalance(Node* &currentNode, Node* parentNode) noexcept
{
	// 레드 레드 문제가 해결되었기 때문에 리턴
	if (currentNode->Parent->Color == BLACK)
	{
		return;
	}

	// 무조건 부모가 레드인 상태 그리고 할아버지, 삼촌 노드가 존재하는 상황이다 ( 레드, 레드 상황 )
	Node* grandParentNode = parentNode->Parent;
	Node* uncleNode = nullptr; // Nil일수도 있고 할아버지의 왼쪽, 오른쪽 자식인지 확인해야 함

	// 부모가 왼쪽 자식이면 삼촌은 오른쪽 자식
	if (grandParentNode->Left == parentNode)
	{
		uncleNode = grandParentNode->Right;
	}
	// 부모가 오른쪽 자식이면 삼촌은 왼쪽 자식
	else if (grandParentNode->Right == parentNode)
	{
		uncleNode = grandParentNode->Left;
	}

	// 부모는 레드 확정 삼촌이 Nil노드인지, BlACK 노드인지 확인( 같은 블랙노드라서 경우가 같음 )
	if (uncleNode == NilNode || uncleNode->Color == BLACK)
	{
		//회전
		if (uncleNode == grandParentNode->Left)
		{
			// 할아버지 기준 우회전

		}
		else if (uncleNode == grandParentNode->Right)
		{
			// 할아버지 기준 좌회전

		}

		// 회전 이후에는 종료
		return;
	}
	// 삼촌이 레드인 경우
	else if (uncleNode->Color == RED)
	{
		// 노드 색상 변경
		currentNode->Parent->Color = BLACK;
		uncleNode->Color = BLACK;
		grandParentNode->Color = RED;

		InsertNodeBalance(grandParentNode, grandParentNode->Parent);

	}

}




inline void RedBlackree::DeleteNode(int data) noexcept
{
	deleteNodeRecursive(RootNode, data);
}

inline void RedBlackree::deleteNodeRecursive(Node* &currentNode, int data) noexcept
{
	// 노드가 비어있으면 리턴
	if (currentNode == nullptr)
	{
		// std::cout << "Empty Tree" << "\n";
		return;
	}

	// 삭제하고 싶은 값을 찾았으면 자식 노드가 몇긴지 체크
	if (currentNode->Data == data)
	{

		// 자식 노드가 없다면 바로 삭제
		if (currentNode->Left == nullptr && currentNode->Right == nullptr)
		{
			//std::cout << "Remove Nodedata : " << currentNode->data << "\n";
			delete currentNode;
			currentNode = nullptr;
			return;
		}
		// 자식 노드가 한 개라면 부모 노드랑 자식 노드랑 연결 후 삭제
		else if (currentNode->Left != nullptr && currentNode->Right == nullptr)
		{
			Node* temp = currentNode->Left; // 부모의 왼쪽 포인터 주소
			delete currentNode;
			currentNode = temp; // 부모 포인터가 왼쪽 자식 노드를 가르킴
			return;

		}
		else if (currentNode->Left == nullptr && currentNode->Right != nullptr)
		{
			Node* temp = currentNode->Right;
			delete currentNode; // 삭제한 메모리 위에 자식 노드 주소를 저장하기 때문에 nullptr로 초기화 안해줘도 됨
			currentNode = temp;
			return;
		}
		// 자식 노드가 두 개 라면 해당 노드를 지우는 것이 아니라 대체할 만한 값을 찾아서 변경하고 변경된 값의 노드를 삭제
		// 삭제하려는 값보다 작은 값중에서 가장 큰 값 또는 큰 값 중에서 가장 작은 값 둘 중에 하나와 변경하면 올바르게 이진트리 구조를 
		// 유지하면서 변경이 가능하다. 
		// 이번엔 오른쪽 트리에서 가장 작은 값을 구해서 바꿔주고 해당 노드 삭제하는 방법을 선택
		else if (currentNode->Left != nullptr && currentNode->Right != nullptr)
		{
			Node* minNode = currentNode->Right;
			while (minNode->Left != nullptr)
			{
				minNode = minNode->Left;
			}
			currentNode->Data = minNode->Data;
			
			// 왼쪽 자식노드는 없는 것은 확정이지만 오른쪽 자식 노드가 있을 수 있기 때문에 재귀호출을 통해서 안전하게 삭제
			//delete minNode;
			deleteNodeRecursive(currentNode->Right, minNode->Data);
			
		}

	}

	// 삭제할 데이터가 아니라면 왼쪽노드 부터 방문해서 삭제할 데이터가 있는지 체크, 왼쪽에 없다면 나와서 오른쪽 방문
	if (currentNode->Data > data)
	{
		deleteNodeRecursive(currentNode->Left, data);
	}
	else if (currentNode->Data < data)
	{
		deleteNodeRecursive(currentNode->Right, data);
	}

}

inline Node* RedBlackree::SearchNode(int data) noexcept
{
	return FindNodeRecursive(RootNode, data);
}

inline Node* RedBlackree::FindNodeRecursive(Node* currentNode, int data) noexcept
{
	// 검색한 노드가 존재하지 않으면 그 다음 노드 검색하도록 리턴
	if (currentNode == nullptr)
	{
		//std::cout << "can't find data" << "\n";
		return nullptr;
	}

	// 검색한 노드가 데이터가 일치하면 노드 자체를 리턴 ( 이미 &형이기 때문에 리턴해주면 될듯
	if (currentNode->Data == data)
	{
		return currentNode;
	}

	// 검색한 노드가 데이터가 일치하지 않으면 데이터 값을 비교하고 다음 노드 검색
	if (currentNode->Data > data)
	{
		return FindNodeRecursive(currentNode->Left, data);
	}
	else if (currentNode->Data < data)
	{
		return FindNodeRecursive(currentNode->Right, data);
	}

	return nullptr;
}

inline bool RedBlackree::DeleteAll(void) noexcept // 트리 전체 삭제 ( 후위 순회 )
{
	DeleteAllRecursive(RootNode);
	return false;
}

inline void RedBlackree::DeleteAllRecursive(Node* &currentNode) noexcept
{
	if (currentNode == nullptr)
	{
		return;
	}

	DeleteAllRecursive(currentNode->Left);
	DeleteAllRecursive(currentNode->Right);
	delete currentNode;
	currentNode = nullptr; // delete 이후 0x8123을 nullptr 수정해줌으로써 읽기 메모리 접근 에러 방지

}

inline void RedBlackree::PrintTree(Node* currentNode) const noexcept // 트리 전체 출력 ( 중위 순회 )
{

	if (currentNode == nullptr)
	{
		return;
	}

	PrintTree(currentNode->Left);
	std::cout << currentNode->Data << "\n";
	PrintTree(currentNode->Right);

}

// GDI 트리 그리기 재귀 함수
inline void RedBlackree::DrawTreeGDI(HDC hdc, Node* node, int x, int y, int xOffset, int yOffset) const noexcept
{
	if (node == nullptr)
	{
		return;
	}

	// 1. 자식 노드로 향하는 '선(Edge)'을 먼저 그립니다.
	// 선을 먼저 그려야 나중에 그리는 '원(노드)' 뒤로 선이 깔끔하게 숨겨집니다.
	if (node->Left != nullptr)
	{
		MoveToEx(hdc, x, y, nullptr); // 현재 위치에서
		LineTo(hdc, x - xOffset, y + yOffset); // 왼쪽 아래로 선 긋기

		// 왼쪽 자식 재귀 호출 (X 간격을 반으로 줄임)
		DrawTreeGDI(hdc, node->Left, x - xOffset, y + yOffset, xOffset / 2, yOffset);
	}

	if (node->Right != nullptr)
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

inline Node* RedBlackree::GetRoot() const noexcept
{ 
	return RootNode; 
}

inline bool RedBlackree::VerifyBinarySortTree(Node* currentNode, int& prevvalue) noexcept
{

	if (currentNode == nullptr)
	{
		return true;
	}

	if (VerifyBinarySortTree(currentNode->Left, prevvalue) == false)
	{
		return false;
	}

	// 이전 값이 더 크다면 정렬 실패된 것이므로 종료
	if (prevvalue >= currentNode->Data)
	{
		std::cout << "failed Verify BinarySortTree!!!!!" << "\n";
		return false;
	}
	prevvalue = currentNode->Data;

	if (VerifyBinarySortTree(currentNode->Right, prevvalue) == false)
	{
		return false;
	}

	// 오른쪽, 왼쪽 모두 정렬 검증 통과
	return true;

}

