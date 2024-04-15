# ThreadPool
Simple C++ Thread Pool for executing arbitrary number of callbacks on limited number of workers.

Example:
```C++
#include <iostream>
#include <vector>
#include "ThreadPool.h"

int main()
{
	ThreadPool pool;
	std::vector<std::future<int>> futures;

	for (int i = 0; i < 1000; i++)
	{
		auto lbd = [i]() { return i; };
		auto fut = pool.push(lbd);
		futures.emplace_back(std::move(fut));
	}

	for (int i = 990; i < 1000; i++)
		std::cout << futures[i].get() << std::endl; // prints 990, ..., 999

}
```
