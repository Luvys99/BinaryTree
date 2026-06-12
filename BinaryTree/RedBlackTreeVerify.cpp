#include "RedBlackTree.h"

bool RedBlacktree::VerifyRedBlackTree()  noexcept
{
	// 루트노드가 없으면 그냥 종료
	if (RootNode == NilNode) return true;

	// 루트 노드가 블랙인지 확인
	if (RootNode->Color == RED)
	{
		std::cout << "RootNode is Red" << "\n";
		return false;
	}

	int prevValue = -99999;		// 트리 데이터 최솟값보다 무조건 작은 값 세팅
	int referenceBlackDepth = -1; // 기준 블랙 높이

	// 정렬, RedBlackTree 검증 재귀 시작
	return CheckRedBlackeTreeRulesRecursive(RootNode, prevValue, 0, referenceBlackDepth);

}

bool RedBlacktree::CheckRedBlackeTreeRulesRecursive(Node* currentNode, int& prevvalue, int currentBlackDepth, int& refBlackDepth) noexcept
{
	// Base 노드에 도달했을 때 블랙 깊이 규칙 검사
	if (currentNode == NilNode)
	{
		if (refBlackDepth == -1)
		{
			refBlackDepth = currentBlackDepth; // 첫 경로의 블랙-깊이를 기준값으로 등록
		}
		else if (currentBlackDepth != refBlackDepth)
		{
			std::cout << "[verification failed] Black Depth Not Euqal!! \n";
			return false;
		}

		return true;
	}

	// 자식으로 내려가기 전 RBT 규칙 검사
	if (currentNode->Color == BLACK)
	{
		currentBlackDepth++; // 블랙 노드면 경로에 블랙 갯수 추가
	}
	else if (currentNode->Color == RED) // 레드라면 레드 레드 인지 체크
	{
		if (currentNode->Left->Color == RED || currentNode->Right->Color == RED)
		{
			std::cout << "[verification failed] Red Red case " << currentNode->Data << "\n";
			return false;
		}
	}


	if (CheckRedBlackeTreeRulesRecursive(currentNode->Left, prevvalue, currentBlackDepth, refBlackDepth) == false)
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

	if (CheckRedBlackeTreeRulesRecursive(currentNode->Right, prevvalue, currentBlackDepth, refBlackDepth) == false)
	{
		return false;
	}

	// 오른쪽, 왼쪽 모두 정렬 검증 통과
	return true;

}

