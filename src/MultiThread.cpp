//#include <thread>
//#include <vector>
//#include <mutex>
//#include <cstdio>
//#include "gl_core_4_4.h"
//#include "glm_header.h"
//
//int main()
//{
//
//
//	std::vector<std::thread> threads;
//	vec4 myVectors[50000];
//	int chunkLength = 50000 / 10;
//
//	for (int i = 0; i < 10; i++) {
//		threads.push_back(std::thread(
//			// defining a lambda that can access myMutex
//			[&](int low, int high){
//			for (int j = low; j < high; j++)
//			{
//				myVectors[i] = glm::normalize(myVectors[i]);
//				//printf(myVectors[i]);
//			}
//		}, i * chunkLength, (i+1) * chunkLength
//		));
//
//	}
//	for (auto& thread : threads)
//		thread.join();
//
//	system("pause");
//
//	return 0;
//}