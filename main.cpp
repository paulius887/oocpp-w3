#include "user.h"

#include <iostream>
#include <stdexcept>

using namespace std;

int main () {
    try {
        user::User a, b;
        a.setNickname("Admin");
        a.setPassword("P4$$word");
        a.setEmail("admin@pinigenai.lt");
        ++b;
        b.setNickname("pineapple911");
        b.setRandomPassword();
        b.setEmail("pewdiepie@gmail.com");
        cout << a << endl << b << endl;
        cin >> a;
        cout << endl << a;
        a.setPassword("Password123"); // invalid_argument exception (no special character)
    }
    catch (invalid_argument &err) {
        cout << err.what() << endl;
    }
    catch (...) {
        cout << "Unknown exception" << endl;
    }
}
