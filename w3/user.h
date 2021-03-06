#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

#define PW_MIN 8
#define PW_MAX 50
#define NAME_MIN 4
#define NAME_MAX 50

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <ctime>
#include <exception>

namespace user {

using namespace std;

class User {
private:
    static int uid_generator;
    int unique_id;
    string nickname;
    string password;
    string email;
    int customRandom(int min, int max);
    string createRandomPassword (int length_min, int length_max);
    bool isEmail(string new_email);
    bool isSpecialCharacter(char);
    int passwordRating (string input_password);
public:
    User ();
    ~User();
    string getNickname ();
    string getPassword ();
    string getEmail ();
    void setNickname (string new_nickname);
    void setPassword (string new_password);
    void setRandomPassword ();
    void setEmail (string new_email);
    string toString ();
};

}

#endif // USER_H_INCLUDED
