#include "RedBlackTree.h"

bool RedBlacktree::InsertNode(int data) noexcept
{
	if (RootNode == nullptr)
	{
		RootNode = new Node(data);
		RootNode->Color = BLACK;
		RootNode->Parent = NilNode;
		RootNode->Left = NilNode;
		RootNode->Right = NilNode;
		return true;
	}

	bool ret = InsertNodeRecursive(RootNode, nullptr, data);

	// 루트는 무조건 블랙으로 마지막에 칠해줌
	if (RootNode != NilNode)
	{
		RootNode->Color = BLACK;
	}

	return ret;
}

// 삽입할 때 값의 자리에 맞는 빈 노드를 찾고 부모노드와 연결하기 위해서 인자로 부모노드를 전달해야 함
bool RedBlacktree::InsertNodeRecursive(Node*& currentNode, Node* parentNode, int data)
{
	// 노드가 비어있다면 빈 자리에 노드 생성
	if (currentNode == NilNode)
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
			InsertNodeBalance(currentNode);
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

void RedBlacktree::InsertNodeBalance(Node* currentNode) noexcept
{
	// 색상 변경 이후에 재귀할 때 루트노드라면 블랙으로 변경해주고 끝
	if (currentNode == RootNode)
	{
		currentNode->Color = BLACK;
		return;
	}

	// 레드 레드 문제가 해결되었기 때문에 리턴
	if (currentNode->Parent->Color == BLACK)
	{
		return;
	}

	// 무조건 부모가 레드인 상태 그리고 할아버지, 삼촌 노드가 존재하는 상황이다 ( 레드, 레드 상황 )
	Node* parentNode = currentNode->Parent;
	Node* grandParentNode = parentNode->Parent;
	Node* uncleNode = NilNode; // Nil일수도 있고 할아버지의 왼쪽, 오른쪽 자식인지 확인해야 함

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
		// 삼촌이 왼쪽 자식이라는 것은 부모가 오른쪽 자식이라는 것
		if (uncleNode == grandParentNode->Left)
		{
			// 삽입된 노드가 부모의 왼쪽 노드라면? ( LR )
			if (currentNode == parentNode->Left)
			{
				// LR 케이스를 우회전을 통해서 LL로 만들었음
				RightRotate(parentNode);

				// LL 케이스 처리 ( 할아버지 기준 좌회전 )
				currentNode->Color = BLACK;
				grandParentNode->Color = RED;
				LeftRotate(grandParentNode);
			}
			else // ( 원래 LL 이라면? )
			{
				// 색상 변경 후 회전 ( LL )
				parentNode->Color = BLACK;
				grandParentNode->Color = RED;

				// 할아버지 기준 좌회전
				LeftRotate(grandParentNode);
			}

		}
		// 삼촌이 오른쪽 자식이면 부모가 왼쪽 자식임
		else if (uncleNode == grandParentNode->Right)
		{
			// 삽입 노드가 부모의 오른쪽 자식이라면? ( RL )
			if (currentNode == parentNode->Right)
			{
				// RL 케이스를 좌회전을 통해서 RR 케이스 만듬
				LeftRotate(parentNode);

				// RR 케이스 처리 ( 할아버지 기준 우회전 )
				currentNode->Color = BLACK;
				grandParentNode->Color = RED;
				RightRotate(grandParentNode);

			}
			else
			{
				// 색상 변경 후 회전 ( RR )
				parentNode->Color = BLACK;
				grandParentNode->Color = RED;

				// 할아버지 기준 우회전
				RightRotate(grandParentNode);
			}

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

		InsertNodeBalance(grandParentNode);

	}

}

void RedBlacktree::LeftRotate(Node* currentNode) noexcept
{
	Node* grandparentNode = currentNode;
	Node* parentNode = currentNode->Right;

	// 1. 고아 노드를 할아버지 왼쪽으로 보냄
	grandparentNode->Right = parentNode->Left;

	if (grandparentNode->Right != NilNode)
	{
		grandparentNode->Right->Parent = grandparentNode;
	}

	// 2. 부모 노드가 올라가면서 부모 노드의 부모를 증조부와 연결함
	// 1) 할아버지의 부모와 부모 노드와 연결
	parentNode->Parent = grandparentNode->Parent;

	// 2) 증조부가 존재하지 않으면 부모 노드가 루트 노드라는 것
	if (grandparentNode->Parent == NilNode)
	{
		RootNode = parentNode;
	}
	// 3) 증조부 존재하고 할아버지 노드가 증조부의 왼쪽 노드라면?
	else if (grandparentNode == grandparentNode->Parent->Left)
	{
		grandparentNode->Parent->Left = parentNode;
	}
	// 4) 증조부 존재하고 할아버지 노드가 증조부의 오른쪽 노드였다면
	else if (grandparentNode == grandparentNode->Parent->Right)
	{
		grandparentNode->Parent->Right = parentNode;
	}

	// 3. 부모의 왼쪽 자식에 할아버지 노드를 연결, 할아버지의 부모를 부모 노드로 연결
	parentNode->Left = grandparentNode;
	grandparentNode->Parent = parentNode;

}

void RedBlacktree::RightRotate(Node* currentNode) noexcept
{

	Node* grandparentNode = currentNode;
	Node* parentNode = currentNode->Left;

	// 1. 고아 노드를 할아버지 오른쪽으로 보냄
	grandparentNode->Left = parentNode->Right;

	if (grandparentNode->Left != NilNode)
	{
		grandparentNode->Left->Parent = grandparentNode;
	}

	// 2. 부모 노드가 올라가고 할아버지 노드가 부모 노드의 오른쪽 자식 노드로 내려옴
	// 1) 할아버지의 부모와 부모 노드와 연결
	parentNode->Parent = grandparentNode->Parent;

	// 2) 증조부가 존재하지 않으면 부모 노드가 루트 노드라는 것
	if (grandparentNode->Parent == NilNode)
	{
		RootNode = parentNode;
	}
	// 3) 증조북 존재하고 할아버지 노드가 증조부의 왼쪽 노드라면?
	else if (grandparentNode == grandparentNode->Parent->Left)
	{
		grandparentNode->Parent->Left = parentNode;
	}
	// 4) 증조부 존재하고 할아버지 노드가 증조부의 오른쪽 노드였다면
	else if (grandparentNode == grandparentNode->Parent->Right)
	{
		grandparentNode->Parent->Right = parentNode;
	}

	// 3. 부모의 오른쪽 자식에 할아버지 노드를 연결, 할아버지의 부모를 부모 노드로 연결
	parentNode->Right = grandparentNode;
	grandparentNode->Parent = parentNode;

}