#include "user.h"

#include <iostream>
#include <stdexcept>

using namespace std;

void f() {
    user::User a, b;
    a.init("admin@pinigenai.lt", "Admin", "P4$$word");
    b.init("awesome@gmail.com", "awesome", "aDjl!J#!#L KjFdjla34343");
    cout << a << endl << b << endl;
    cout << endl << endl << endl;
}

int main () {
    try {
        f();
        user::User a, b, c;
        a.init("admin@pinigenai.lt", "Admin", "P4$$word");
        b.init("awesome@gmail.com", "awesome", "aDjl!J#!#L KjFdjla34343");
        cout << a << endl << b;
        cin >> b;
        cout << b;
        // cout << c; // InitException
    }
    catch (invalid_argument &err) {
        cout << err.what() << endl;
    }
    catch (overflow_error &err) {
        cout << err.what() << endl;
    }
    catch (runtime_error &err) {
        cout << err.what() << endl;
    }
    catch (...) {
        cout << "Unknown exception occured" << endl;
    }
}
