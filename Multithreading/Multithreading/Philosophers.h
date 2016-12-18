#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
using namespace std::chrono_literals;
using std::thread;


/*struct P
{
	unsigned int index;//index in the cv array
	unsigned int f1;//index of forks philosopher has access to
	unsigned int f2;
	
};/**/

namespace Philosophers
{
	std::condition_variable cv[5];
	bool forks[5];//if in use true. if not false.
	std::mutex m;

	//P p0;//philosophers
	//P p1;
	//P p2;
	//P p3;
	//P p4;

	bool q = false;

	void Quit()
	{
		//when the user hits enter the function quits
		std::cin.get();
		std::cout << "quitting\n";
		q = true;
		for (int i = 0; i < 5; i++)
		{
			cv[i].notify_all();
		}
		std::this_thread::sleep_for(2s);
		for (int i = 0; i < 5; i++)
		{
			cv[i].notify_all();
		}
	}

	void eat(int index)
	{
		std::unique_lock<std::mutex> lck(m);
		int f1 = index - 1;
		if (f1 <= -1)
		{
			f1 = 4;
		}
		int f2 = index + 1;
		if (f2 > 4)
		{
			f2 = 0;
		}
		while (!q)
		{
			std::cout << "Philosopher " << index << " is thinking\n";
			//m.lock();
			if (!forks[f1] && !forks[f2])
			{
				std::cout << "Philosopher " << index << " is eating\n";
				forks[f1] = true;
				forks[f2] = true;
				std::this_thread::sleep_for(2s);
				std::cout << "Philosopher " << index << " is done eating\n";
				
				forks[f1] = false;
				forks[f2] = false;
				if (index < 4)
				{
					cv[index + 1].notify_all();
				}
				else
				{
					cv[0].notify_all();
				}
				if (index > 0)
				{
					cv[index - 1].notify_all();
				}
				else
				{
					cv[4].notify_one();
				}
				cv[index].wait(lck);
			}
			
			std::cout << "Philosopher " << index << " is thinking\n";
			//m.unlock();
			
		}
	}

	void philosopherRun()
	{
		//p0.index = 0;//p0 is cv[0]
		//p0.f1 = 4;
		//p0.f2 = 0;

		//p1.index = 1;//p1 is cv[1]
		//p1.f1 = 0;
		//p1.f2 = 1;

		//p2.index = 2;//p2 is cv[2]
		//p2.f1 = 1;
		//p2.f2 = 2;

		//p3.index = 3;//p3 is cv[3]
		//p3.f1 = 2;
		//p3.f2 = 3;

		//p4.index = 4;//p4 is cv[4]
		//p4.f1 = 3;
		//p4.f2 = 4;

		for (int i = 0; i < 5; i++)
		{
			forks[i] = false;
		}
		
		thread quitter(Quit); //thread that checks for the user to hit enter. When they do it changes quit to true
		thread t0(eat, 0);
		thread t1(eat, 1);
		thread t2(eat, 2);
		thread t3(eat, 3);
		thread t4(eat, 4);

		t0.join();
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		quitter.join();
		/**/
	}
};

