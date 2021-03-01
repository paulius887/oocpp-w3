#include "user.h"

using namespace std;

int main () {
    try {
    user::User a, b;
    a.setNickname("Name");
    a.setPassword("P4$$word");
    a.setEmail("admin@pinigenai.lt");
    b.setNickname("pineapple911");
    b.setRandomPassword();
    b.setEmail("pewdiepie@gmail.com");
    cout << a.toString() << endl << b.toString();
    b.setPassword("Password123"); // exception (no special character)
    }
    catch (string err) {
        cout << err << endl;
    }
    catch (...) {
        cout << "Unknown exception" << endl;
    }
}
