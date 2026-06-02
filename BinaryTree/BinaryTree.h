#pragma once

#include <iostream>

struct Node
{
	Node* Left;
	Node* Right;
	int Data;

	Node(int data) : Left(nullptr), Right(nullptr), Data(data) {}
};

class BinaryTree
{

public:
	BinaryTree() noexcept;
	virtual ~BinaryTree() noexcept;

	inline void InsertNode(int data) noexcept; // 새로운 노드 삽입
	inline bool DeleteNode(int data) noexcept; // 검색한 데이터 삭제
	inline Node* SearchNode(int data) noexcept; // 검색한 데이터의 노드를 찾아서 반환
	inline bool DeleteAll(void) noexcept; // 트리 전체 삭제
	inline void PrintTree(Node* node = nullptr, int Depth = 0, int iLR = 0) noexcept; // 트리 전체 출력

private:
	// InsertNode 내부에서 호출되는 재귀 함수 ( 위치 찾아서 노드 추가하는 함수 )
	inline void InsertNodeRecursive(Node* currentnode, int data) noexcept; // 새로운 노드 삽입
	// SearchNode 내부에서 호출되는 재귀 함수 ( 노드 찾기 )
	inline Node* FindNodeRecursive(Node* node, int data) noexcept;
	// DeleteNode 내부에서 호출되는 재귀 함수 ( 노드 찾기 & 삭제 & 후처리 )
	inline Node* deleteNodeRecursive(Node* node, Node* parentnode, int data) noexcept;

	Node* RootNode;
	int NodeCount;
};

inline BinaryTree::BinaryTree() noexcept : NodeCount(0), RootNode(nullptr) {}

inline BinaryTree::~BinaryTree() noexcept
{
	
}

inline void BinaryTree::InsertNode(int data) noexcept
{
	// 노드를 무한정 넣지 못하게 최대치를 설정해놔야 할듯

	InsertNodeRecursive(RootNode, data);
}

inline void BinaryTree::InsertNodeRecursive(Node* currentNode, int data) noexcept
{
	// 노드가 비어있다면 빈 자리에 노드 생성
	if (currentNode == nullptr)
	{
		currentNode = new Node(data);
		return;
	}
	
	// 노드가 존재한다면 왼쪽, 오른쪽 방향을 결정해서 재귀 호출
	if (currentNode->Data > data)
	{
		InsertNodeRecursive(currentNode->Left, data);
	}
	else if (currentNode->Data < data)
	{
		InsertNodeRecursive(currentNode->Right, data);
	}

	// 여기에 실행흐름이 온 것이면 같은 값을 넣은 것이므로 무시
	std::cout << "Same Value Error" << "\n";
	return;
}

inline bool BinaryTree::DeleteNode(int data) noexcept
{
	deleteNodeRecursive(RootNode, nullptr, data);
}

inline Node* deleteNodeRecursive(Node* node, Node* parentnode, int data) noexcept
{
	if (node == nullptr)
	{


		return;
	}

}
