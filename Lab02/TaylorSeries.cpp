#include <iostream>
#include <math.h>
#include <conio.h>
using namespace std;

unsigned int factorial(unsigned int n);

int main(int argc, char* argv[])
{
	if (argc == 4)
	{
		double interval_start = atof(argv[1]);
		double interval_end = atof(argv[2]);
		double increment_step = atof(argv[3]);
		double sum = 1.0;
		float k = 0.5;
		double numerator = 1.0;
		double curr_sum = 0.0;

		cout << "Interval from x = " << interval_start << " to x = " << interval_end << ":" << endl;
		cout << "Increment step: " << increment_step << endl;

		for (double i = interval_start; i <= interval_end; i += increment_step)
		{
			for (int n = 1; n <= 5; n++)
			{
				numerator *= (k - n + 1);
				curr_sum = numerator * pow(i, n) / factorial(n);
				sum += curr_sum;
				n++;
			}
			cout << "Series sum for x = " << i << ": " << sum << endl;
			
			sum = 1.0;
			numerator = 1.0;
			curr_sum = 0.0;
		}
	}
	else
	{
		cout << "Something went wrong" << endl;
	}

	_getch();
	return 0;
}

unsigned int factorial(unsigned int n)
{
	if (n == 0)
		return 1;
	return n * factorial(n - 1);
}