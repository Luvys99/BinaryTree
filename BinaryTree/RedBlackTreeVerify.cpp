#include "RedBlackTree.h"

bool RedBlacktree::VerifyBinarySortTree(Node* currentNode, int& prevvalue) noexcept
{

	if (currentNode == NilNode)
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
