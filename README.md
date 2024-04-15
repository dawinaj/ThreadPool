# ThreadPool
Simple C++ Thread Pool for executing arbitrary number of callbacks on limited number of workers.


Based on [CodeReview](https://codereview.stackexchange.com/questions/174684/c-threadpool-executor-implementation). 

Modified to compile on MSVC and take arbitrary arguments. 

Requires C++23 (for `std::move_only_function`, to compile on MSVC). 


## Example:
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
