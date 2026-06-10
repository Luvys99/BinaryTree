#include "RedBlackTree.h"

void RedBlacktree::DeleteNode(int data) noexcept
{
	deleteNodeRecursive(RootNode, data);
}

void RedBlacktree::deleteNodeRecursive(Node*& currentNode, int data) noexcept
{
	// 노드가 비어있으면 리턴
	if (currentNode == NilNode)
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