#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

#include <string>
#include <vector>

namespace user {

using namespace std;

class User {
private:
    class InnerUser;
    InnerUser *inner;
public:
    User ();
    User (const string &email, const string &username, const string &password);
    ~User();
    static int generateRandomNumber(const int &min, const int &max);
    static string generateRandomPassword (const int &length_min, const int &length_max);
    void init (const string &email, const string &username, const string &password);
    void checkIfInit (InnerUser *inner, const char *file, int line) const;
    int getUid () const;
    string getEmail () const;
    string getUsername () const;
    string getPassword () const;
    unsigned short getLevel () const;
    void setEmail (const string &email);
    void setUsername (const string &username);
    void setPassword (const string &password);
    void setRandomPassword ();
    void setLevel (const unsigned short &level);
    bool operator== (const User &user2) const;
    bool operator!= (const User &user2) const;
    bool operator< (const User &user2) const;
    bool operator<= (const User &user2) const;
    bool operator> (const User &user2) const;
    bool operator>= (const User &user2) const;
    void operator++ ();
    unsigned short operator++ (int);
    string toString () const;

    friend istream& operator>> (istream &o, User &user);
    friend ostream& operator<< (ostream &o, const User &user);
};

}

#endif // USER_H_INCLUDED
