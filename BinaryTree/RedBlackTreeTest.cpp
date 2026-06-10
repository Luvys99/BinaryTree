#include "RedBlackTreeTest.h"
#include "RedBlackTree.h"
#include "Globals.h"

void StressTestFuc(std::mt19937& gen)
{

    std::uniform_int_distribution<int> dis(1, 100000);
    int testLoopCount = 10000;

    for (int i = 0; i < testLoopCount; i++)
    {
        std::vector<int> targetToDelete;

        //5개의 삭제할 랜덤한 노드의 정수를 저장할 때까지
        while (targetToDelete.size() < 5)
        {

            int randomvalue1 = dis(gen);

            if (g_tree.SearchNode(randomvalue1) != nullptr)
            {
                targetToDelete.push_back(randomvalue1);
            }

        }

        // 찾은 5개의 정수 삭제
        for (int value : targetToDelete)
        {
            g_tree.DeleteNode(value);
        }

        // 5개의 랜덤 함수 삽입
        int count = 0;
        while (count < 5)
        {
            int randomvalue2 = dis(gen);

            // 랜덤한 값이 정상적으로 삽입되었으면 count++, 없으면 다음 루프
            if (g_tree.InsertNode(randomvalue2) != false)
            {
                count++;
            }
        }

        // 검증 중위순회를 통해서 값들이 삭제, 삽입에도 정렬이 보장되는지
        int prevvalue = -1; // 최소값보다 작은 값중에서도 가장 큰 값
        if (g_tree.VerifyBinarySortTree(g_tree.GetRoot(), prevvalue) == false)
        {
            std::cout << "test failed!! " << i << " Loop error" << "\n";
            return;
        }


    }
    test_Loop_count++;
    std::cout << "test success!! " << "Loop Count : " << test_Loop_count << "\n";
}