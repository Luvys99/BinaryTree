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

class RedBlacktree
{

public:
	RedBlacktree();
	~RedBlacktree() noexcept;

	bool InsertNode(int data) noexcept; // 새로운 노드 삽입
	void DeleteNode(int data) noexcept; // 검색한 데이터 삭제
	inline Node* SearchNode(int data) noexcept; // 검색한 데이터의 노드를 찾아서 반환
	inline bool DeleteAll(void) noexcept; // 트리 전체 삭제 ( 후위 순회 )
	// HDC, 현재 노드, 현재 X좌표, 현재 Y좌표, X좌표가 벌어질 간격, Y좌표가 내려갈 간격

	// DrawTreeGDI 함수 호출할 때 Node로 넘기기 위함
	inline Node* GetRoot() const noexcept;

	// 정렬 상태 검증 함수
	bool VerifyBinarySortTree(Node* currentNode, int& prevvalue) noexcept;

private:
	// InsertNode 내부에서 호출되는 재귀 함수 ( 위치 찾아서 노드 추가하는 함수 )
	bool InsertNodeRecursive(Node* &currentNode, Node* parentNode, int data); // new는 예외를 던지기 때문에 noexcept 붙이면 안됨
	// SearchNode 내부에서 호출되는 재귀 함수 ( 노드 찾기 )
	inline Node* FindNodeRecursive(Node* currentNode, int data) noexcept;
	// DeleteNode 내부에서 호출되는 재귀 함수 ( 노드 찾기 & 삭제 & 후처리 )
	void deleteNodeRecursive(Node* &currentNode, int data) noexcept;
	// DeleteAll 내부에서 호출되는 재귀 함수 ( 노드 후위 순회 )
	inline void DeleteAllRecursive(Node* &currentNode) noexcept;
	// 밸런스 함수
	void InsertNodeBalance(Node* currentNode, Node* parentNode) noexcept;
	// 밸런스 회전 함수
	void LeftRotate(Node* currentNode) noexcept;
	void RightRotate(Node* currentNode) noexcept;
	inline void PrintTree(Node* currentNode) const noexcept; // 중위 순회를 통해서 정렬이 유지되고 있는지 이전 값과 비교해서 검증하는 함수

	Node* RootNode;
	Node* NilNode;
	int NodeCount;
};

inline RedBlacktree::RedBlacktree() : NodeCount(0)
{
	// 트리가 생성되면 한 개만 생성해서 모든 Nil노드를 가르키는 것이 하나의 Nil노드를 가르키게 되는 것
	NilNode = new Node;
	NilNode->Color = BLACK;
	RootNode = nullptr;
}

inline RedBlacktree::~RedBlacktree() noexcept
{
	//Tree 전체 삭제
	DeleteAll();
}

inline Node* RedBlacktree::GetRoot() const noexcept
{
	return RootNode;
}

inline Node* RedBlacktree::SearchNode(int data) noexcept
{
	return FindNodeRecursive(RootNode, data);
}

inline Node* RedBlacktree::FindNodeRecursive(Node* currentNode, int data) noexcept
{
	// 검색한 노드가 존재하지 않으면 그 다음 노드 검색하도록 리턴
	if (currentNode == NilNode || currentNode == nullptr)
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

inline bool RedBlacktree::DeleteAll(void) noexcept // 트리 전체 삭제 ( 후위 순회 )
{
	DeleteAllRecursive(RootNode);
	return false;
}

inline void RedBlacktree::DeleteAllRecursive(Node* &currentNode) noexcept
{
	if (currentNode == NilNode || currentNode == nullptr)
	{
		return;
	}

	DeleteAllRecursive(currentNode->Left);
	DeleteAllRecursive(currentNode->Right);
	delete currentNode;
	currentNode = nullptr; // delete 이후 0x8123을 nullptr 수정해줌으로써 읽기 메모리 접근 에러 방지

}

inline void RedBlacktree::PrintTree(Node* currentNode) const noexcept // 트리 전체 출력 ( 중위 순회 )
{

	if (currentNode == NilNode)
	{
		return;
	}

	PrintTree(currentNode->Left);
	std::cout << currentNode->Data << "\n";
	PrintTree(currentNode->Right);

}

