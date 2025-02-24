#include "redis.h"
#include <iostream>
using namespace std;

int main() {
    RedisUtil redis("localhost", 6379);
    
    cout << redis.getKey("arg") << endl;
    cout << redis.getKey("name") << endl;
    // redis.deleteKey("name");
    redis.disconnect();
    return 0;
}