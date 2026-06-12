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
		return;
	}

	// 삭제하고 싶은 값을 찾았으면 자식 노드가 몇 개인지 체크
	if (currentNode->Data == data)
	{

		// 자식 노드 없는 경우
		if (currentNode->Left == NilNode && currentNode->Right == NilNode)
		{
			// 삭제할 노드가 레드인지 블랙인지 확인
			bool isblack = false;
			// 부모 포인터 미리 저장
			Node* parentNode = currentNode->Parent;

			if (currentNode->Color == BLACK)
			{
				isblack = true;
			}

			// 레드든 블랙이든 삭제
			delete currentNode;
			currentNode = NilNode;

			// 삭제한 노드가 블랙이라면 밸런스 필요함
			if (isblack == true)
			{
				// NilNode의 부모 연결
				NilNode->Parent = parentNode;
				// NilNode여도 호출 ( 밸런스의 기준으로 던져도 됨 )
				DeleteNodeBalance(currentNode);
			}

			return;
		}
		// 자식 노드가 한 개인 경우 ( 자식 노드가 한개라면 무조건 블랙이기 떄문에 색 검사 X )
		else if (currentNode->Left != NilNode && currentNode->Right == NilNode)
		{
			// 부모 노드랑 자식 노드랑 연결 후 삭제
			Node* temp = currentNode->Left; // 부모의 왼쪽 자식 노드
			temp->Parent = currentNode->Parent; // 삭제 노드의 부모 포인터를 가르킴
			delete currentNode;
			currentNode = temp; // 부모의 왼쪽 노드 포인터로 삭제 노드의 자식 노드를 가르키게함

			// 삭제 이후에 최종 삭제 노드를 기준으로 밸런스 작업 
			DeleteNodeBalance(currentNode);

			return;

		}
		else if (currentNode->Left == NilNode && currentNode->Right != NilNode)
		{
			Node* temp = currentNode->Right;
			temp->Parent = currentNode->Parent; // 할아버지 노드와 연결
			delete currentNode; // 삭제한 메모리 위에 자식 노드 주소를 저장하기 때문에 nullptr로 초기화 안해줘도 됨
			currentNode = temp; // 자식 노드의 주소를 저장 

			// 삭제 이후에 최종 삭제 노드를 기준으로 밸런스 작업 
			DeleteNodeBalance(currentNode);

			return;
		}
		// 자식 노드가 두 개인 경우 오른 자식 중에 가장 작은노드 또는 왼자식 중에 가장 큰 노드를 선택해서 값을 바꿔주고 해당 노드 삭제해줌
		// 자식 노드가 둘이라면 색 검사를 해야함 레드, 블랙 둘다 가능하기 때문
		else if (currentNode->Left != NilNode && currentNode->Right != NilNode)
		{
			// 레드, 블랙이든 삭제는 동일
			// 왼쪽 노드에서 가장 큰 값을 찾아서 
			Node* minNode = currentNode->Left;
			while (minNode->Right != NilNode)
			{
				minNode = minNode->Right;
			}
			currentNode->Data = minNode->Data;

			// 삭제할 노드가 왼쪽에 있으므로 왼쪽으로 재귀함수를  통해서 삭제
			deleteNodeRecursive(currentNode->Left, minNode->Data);

			// 최종 삭제 노드는 자식이 2개인 경우가 없기 때문에 여기서 밸런스 호출해줄 필요 없음
			return;
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

// 최종 삭제하려는 노드는 자식이 1개 이하라는 조건이 존재함
void RedBlacktree::DeleteNodeBalance(Node* currentNode) noexcept
{
	// 반복 검사 시작
	// 밸런싱이 필요한 상황이면 루프 ( 루트가 아니며 삭제 노드가 블랙이라면 블랙 -1이 되기 때문에 밸런싱이 필요함 )
	// 밸런스 처리 함수 들어오기 전에 삭제 노드가 블랙이면 함수 호출이 되기 때문에 루드 노트라면 탈출하도록 로직 개선
	while (1)
	{
		// 1. 삭제 노드의 자식이 레드
		if (currentNode == RootNode || currentNode->Color == RED)
		{
			currentNode->Color = BLACK;
			break;
		}

		// 먼저 해당 노드가 부모의 왼자식인지 오른자식인지 확인
		if (currentNode == currentNode->Parent->Left)
		{
			// 2. 삭제 노드의 형제가 레드 ( 3, 4, 5번의 케이스 중 하나로 바뀜 )
			if (currentNode->Parent->Right->Color == RED)
			{
				// 형제 노드 블랙으로 변경
				currentNode->Parent->Right->Color = BLACK;

				// 좌회전 ( 좌회전을 통해서 형제가 블랙이 되는 상황을 만듬 )
				// 형제가 부모가 되면서 형제의 기존 왼자식이 왼쪽으로 넘어염
				LeftRotate(currentNode->Parent);

				// 기존 부모를 레드로 바꿈 ( 형제가 레드였으므로 부모가 절대 레드일 수 없다. 레드 레드 상황은 나오지 않는다 )
				currentNode->Parent->Color = RED;

				// 재작업 
				continue;
			}
			// 3. 삭제 노드의 형제가 블랙이고 형제의 양쪽 자식이 블랙
			else if (currentNode->Parent->Right->Color == BLACK && currentNode->Parent->Right->Left->Color == BLACK && currentNode->Parent->Right->Right->Color == BLACK)
			{
				currentNode->Parent->Right->Color = RED; // 내 쪽에서 블랙 -1이 되었기 때문에 형제 쪽도 -1해준 것
				currentNode = currentNode->Parent;	// 근데 내 쪽은 해결되었으나 할아버지 입장에서 내 부모라인에 -1이 된 것
													// 다시 내 부모 노드 기준으로 루프를 돌게 함

			}
			// 4. 삭제 노드의 형제가 블랙이고 형제의 오른자식이 블랙
			else if (currentNode->Parent->Right->Color == BLACK && currentNode->Parent->Right->Right->Color == BLACK && currentNode->Parent->Right->Left->Color == RED)
			{
				// 형제의 왼자식을 블랙으로
				currentNode->Parent->Right->Left->Color = BLACK;

				// 형제를 레드로
				currentNode->Parent->Right->Color = RED;

				// 형제 기준으로 우회전 ( 이걸로 5번째 상황으로 맞춰짐 )
				RightRotate(currentNode->Parent->Right);
				
				// 5번 상황으로 넘어가도록 다음 루프
				continue;
			}
			// 5. 삭제 노드의 형제가 블랙이고 형제의 오른자식이 레드
			else if (currentNode->Parent->Right->Color == BLACK && currentNode->Parent->Right->Right->Color == RED)
			{
				// 형제 컬러를 부모 컬러로
				currentNode->Parent->Right->Color = currentNode->Parent->Color;

				// 부모 컬러는 블랙
				currentNode->Parent->Color = BLACK;

				// 형제의 오른 자식은 블랙
				currentNode->Parent->Right->Right->Color = BLACK;

				// 부모 기준 좌회전함으로써 해결
				LeftRotate(currentNode->Parent);
				break;
			}

		}
		else if (currentNode == currentNode->Parent->Right)
		{
			// 2. 삭제 노드의 형제가 레드
			if (currentNode->Parent->Left->Color == RED)
			{
				// 형제 노드 블랙으로 변경
				currentNode->Parent->Left->Color = BLACK;

				// 우회전 ( 우회전을 통해서 형제가 블랙이 되는 상황을 만듬 )
				// 형제가 부모가 되면서 형제의 기존 오른자식이 오른쪽으로 넘어염
				RightRotate(currentNode->Parent);

				// 기존 부모를 레드로 바꿈 ( 형제가 레드였으므로 부모가 절대 레드일 수 없다. 레드 레드 상황은 나오지 않는다 )
				currentNode->Parent->Color = RED;

				// 재작업 
				continue;
			}
			// 3. 삭제 노드의 형제가 블랙이고 형제의 양쪽 자식이 블랙
			else if (currentNode->Parent->Left->Color == BLACK && currentNode->Parent->Left->Left->Color == BLACK && currentNode->Parent->Left->Right->Color == BLACK)
			{
				currentNode->Parent->Left->Color = RED; // 내 쪽에서 블랙 -1이 되었기 때문에 형제 쪽도 -1해준 것
				currentNode = currentNode->Parent;	// 근데 내 쪽은 해결되었으나 할아버지 입장에서 내 부모라인에 -1이 된 것
													// 다시 내 부모 노드 기준으로 루프를 돌게 함

			}
			// 4. 삭제 노드의 형제가 블랙이고 형제의 완자식이 블랙
			else if (currentNode->Parent->Left->Color == BLACK && currentNode->Parent->Left->Left->Color == BLACK && currentNode->Parent->Left->Right->Color == RED)
			{
				// 형제의 오른자식을 블랙으로
				currentNode->Parent->Left->Right->Color = BLACK;

				// 형제를 레드로
				currentNode->Parent->Left->Color = RED;

				// 형제 기준으로 좌회전 ( 이걸로 5번째 상황으로 맞춰짐 )
				LeftRotate(currentNode->Parent->Left);

				// 5번 상황으로 넘어가도록 다음 루프
				continue;
			}
			// 5. 삭제 노드의 형제가 블랙이고 형제의 왼자식이 레드
			else if (currentNode->Parent->Left->Color == BLACK && currentNode->Parent->Left->Left->Color == RED)
			{
				// 형제 컬러를 부모 컬러로
				currentNode->Parent->Left->Color = currentNode->Parent->Color;

				// 부모 컬러는 블랙
				currentNode->Parent->Color = BLACK;

				// 형제의 왼 자식은 블랙
				currentNode->Parent->Left->Left->Color = BLACK;

				// 부모 기준 우회전함으로써 해결
				RightRotate(currentNode->Parent);
				break;
			}
		}
	
	}// while 루프 끝

}
