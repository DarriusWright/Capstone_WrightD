#include <queue>
#include <iostream>
#include <ctime>
#include <functional>

using namespace std;
struct MinValue
{
	float min;
	int id;
	bool operator<(const MinValue& rhs) const {
      return rhs.min < this->min;
  }
};


void main()	
{
	srand(time(0));
	priority_queue<MinValue,vector<int>,less<int> > q;
	for( int i = 0; i != 10; ++i ) q.push({1.0f *i,i});

	
	
    cout <<q.top().min << "Min-heap, popped one by one: ";
	
    while( ! q.empty() ) {
        cout << q.top() << ' ';  // 0 3 3 3 4 5 5 6 8 9
		q.pop();
    }
    cout << endl;

	system("pause");
}