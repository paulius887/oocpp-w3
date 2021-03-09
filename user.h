#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

#define PW_MIN 8
#define PW_MAX 50
#define NAME_MIN 5
#define NAME_MAX 50

#include <string>
#include <vector>

namespace user {

using namespace std;

class User {
public:
    User ();
    ~User();
    unsigned short getLevel () const;
    string getNickname () const;
    string getPassword () const;
    string getEmail () const;
    void setLevel (const unsigned short &level);
    void setNickname (const string &nickname);
    void setPassword (const string &password);
    void setRandomPassword ();
    void setEmail (const string &email);
    bool operator== (const User &user2) const;
    bool operator!= (const User &user2) const;
    bool operator< (const User &user2) const;
    bool operator<= (const User &user2) const;
    bool operator> (const User &user2) const;
    bool operator>= (const User &user2) const;
    void operator++ ();
    unsigned short operator++ (int);
    friend ostream& operator<< (ostream &o, const User &user);
    friend istream& operator>> (istream &o, User &user);
    string toString () const;
private:
    static int uid_generator;
    const int unique_id;
    unsigned short level;
    string nickname;
    string password;
    string email;
    int customRandom(const int &min, const int &max) const;
    string createRandomPassword (const int &length_min, const int &length_max) const;
    bool isEmail(const string &email) const;
    bool isSpecialCharacter(const char &c) const;
    int passwordRating (const string &input_password) const;
};

}

#endif // USER_H_INCLUDED
