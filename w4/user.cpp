#include "user.h"

#include <iostream>
#include <sstream>
#include <ctime>

namespace user {

    User::User ()
        : unique_id(++uid_generator)
        , level(0)
        , nickname ("N/A")
        , password ("N/A")
        , email ("N/A")
    { }

    User::~User() {
    }

    unsigned short User::getLevel () const {
        return level;
    }

    string User::getNickname () const {
        return nickname;
    }

    string User::getPassword () const {
        return password;
    }

    string User::getEmail () const {
        return email;
    }

    void User::setLevel (const unsigned short &level) {
        this->level = level;
    }

    void User::setNickname (const string &nickname) {
        if (nickname.length() >= NAME_MIN && nickname.length() <= NAME_MAX) {
            for (unsigned i = 0; i < nickname.length(); ++i) {
                if (isSpecialCharacter(nickname[i])) {
                    if (nickname[i] != '.' && nickname[i] != '_') { // characters . and _ are allowed in nicknames
                        throw invalid_argument("setNickname received a string that has invalid characters in it (allowed characters: 0-9, A-Z, . and _ )");
                    }
                }
            }
            this->nickname = nickname;
        }
        else {
            stringstream ss;
            ss << "setNickname received a string that's either too long or too short (length must be " << NAME_MIN << "-" << NAME_MAX << ")";
            throw invalid_argument(ss.str());
        }
    }

    void User::setPassword (const string &password) {
        bool digit = 0, uppercase = 0, special = 0;
        if (password.length() >= PW_MIN && password.length() <= PW_MAX) {
            for (unsigned i = 0; i < password.length(); ++i) {
                if (isdigit(password[i])) {
                    digit = 1;
                }
                else if (isupper(password[i])) {
                    uppercase = 1;
                }
                else if (isSpecialCharacter(password[i])) {
                    special = 1;
                }
            }
            if (digit && uppercase && special) {
                this->password = password;
            }
            else {
                throw invalid_argument("setPassword received a string that doesn't have an upper letter, a number and a special symbol in it");
            }
        }
        else {
            stringstream ss;
            ss << "setPassword received a string that's either too long or too short (length must be " << PW_MIN << "-" << PW_MAX << " characters)";
            throw invalid_argument(ss.str());
        }
    }

    void User::setRandomPassword () {
        password = createRandomPassword(15, 20);
    }

    void User::setEmail (const string &email) {
        if (isEmail(email)) {
            this->email = email;
        }
        else {
            throw invalid_argument("setEmail received a string that didn't pass isEmail validation");
        }
    }

    bool User::operator> (const User &user2) const {
        return (this->level > user2.level);
    }

    bool User::operator< (const User &user2) const {
        return (this->level < user2.level);
    }

    bool User::operator<= (const User &user2) const {
        return (this->level <= user2.level);
    }

    bool User::operator>= (const User &user2) const {
        return (this->level >= user2.level);
    }

    bool User::operator== (const User &user2) const {
        return (this->level == user2.level);
    }

    bool User::operator!= (const User &user2) const {
        return (this->level != user2.level);
    }

    void User::operator++ () {
        ++level;
    }

    unsigned short User::operator++ (int) {
        return level++;
    }

    ostream& operator<< (ostream &o, const User &user) {
        o << user.toString();
        return o;
    }

    istream& operator>> (istream &i, User &user) {
        User temp;
        string input;
        cout << "Enter your new e-mail address: ";
        i >> input;
        temp.setEmail(input);
        cout << "Enter your new username: ";
        i >> input;
        temp.setNickname(input);
        cout << "Enter your new password: ";
        i >> input;
        temp.setPassword(input);
        user.email = temp.email;
        user.nickname = temp.nickname;
        user.password = temp.password;
        return i;
    }

    string User::toString () const {
        stringstream ss;
        ss << "unique_id " << unique_id << endl;
        ss << "level " << level << endl;
        ss << "nickname '" << nickname << "'" << endl;
        ss << "password '" << password << "' passwordRating(password) = " << passwordRating(password) << endl;
        ss << "email '" << email << "'" << endl;
        return ss.str();
    }

    int User::uid_generator = 0;

    int User::customRandom (const int &min, const int &max) const { // stackoverflow.com/questions/12657962
        int n = max - min + 1;
        int remainder = RAND_MAX % n;
        int x;
        do {
            x = rand();
        }
        while (x >= RAND_MAX - remainder);
        return min + x % n;
    }

    string User::createRandomPassword (const int &length_min, const int &length_max) const {
        if (length_min < PW_MIN || length_max > PW_MAX) {
            stringstream ss;
            ss << "createrRandomPassword received an invalid length_min or length_max value (values must be " << PW_MIN << "-" << PW_MAX << ")";
            throw invalid_argument(ss.str());
        }
        srand(time(NULL));
        bool digit, uppercase, special;
        string random_password;
        do {
            random_password.clear();
            digit = uppercase = special = 0;
            int length = customRandom(length_min, length_max);
            for (int i = 0; i < length; ++i) {
                int char_type = customRandom (0, 3);
                if (char_type == 0) {
                    random_password += customRandom ('a', 'z');
                }
                else if (char_type == 1) {
                    uppercase = 1;
                    random_password += customRandom ('A', 'Z');
                }
                else if (char_type == 2) {
                    digit = 1;
                    random_password += customRandom ('0', '9');
                }
                else {
                    special = 1;
                    int temp = customRandom (1, 32);
                    if (temp <= 16) {
                        random_password += customRandom (' ', '/');
                    }
                    else if (temp <= 23) {
                        random_password += customRandom (':', '@');
                    }
                    else if (temp <= 28) {
                        random_password += customRandom ('[', '`');
                    }
                    else {
                        random_password += customRandom ('{', '~');
                    }
                }
            }
        }
        while (!digit || !uppercase || !special);
        return random_password;
    }

    // - at least one character before @, before . and after .
    // - first character must be alphabetic
    // - all characters before @ can be alphabetic, numeric, or '.', '_', '-'
    // - all characters after @ must be alphabetic or numeric
    // - all characters after . must be alphabetic
    bool User::isEmail (const string &email) const {
        if (!isalpha(email[0])) {
            return 0;
        }
        for (unsigned i = 0; i < email.length(); ++i) {
            if (email[i] == '@') {
                if (i == 0) {
                    return 0;
                }
                unsigned ver = ++i;
                for (; i < email.length(); ++i) {
                    if (email[i] == '.') {
                        if (i - ver == 0) {
                            return 0;
                        }
                        ver = ++i;
                        for (; i < email.length() + 1; ++i) {
                            if (email[i] == '\0') {
                                if (i - ver == 0) {
                                    return 0;
                                }
                                else {
                                    return 1;
                                }
                            }
                            if (!isalpha(email[i])) {
                                return 0;
                            }
                        }
                    }
                    if (!isalnum(email[i])) {
                        return 0;
                    }
                }
            }
            if (!isalnum(email[i]) && email[i] != '.' && email[i] != '_' && email[i] != '-') {
                return 0;
            }
        }
        return 0;
    }

    bool User::isSpecialCharacter (const char &c) const { // ' ' (space) is considered a special symbol
        if (!isalnum(c) && c > 31 && c < 127) {
            return 1;
        }
        else {
            return 0;
        }
    }

    int User::passwordRating (const string &input_password) const { // <25 - weak (return 0), <40 - decent (return 1), <55 - strong (return 2), <70 - very strong (return 3)
        int password_rating = 0;
        for (unsigned i = 0; i < input_password.length(); ++i) {
            if (islower(input_password[i])) {
                password_rating += 2;
            }
            else if (isdigit(input_password[i]) || isupper(input_password[i])) {
                password_rating += 3;
            }
            else if (isSpecialCharacter(input_password[i])) {
                password_rating += 5;
            }
        }
        if (password_rating < 25) {
            return 0;
        }
        else if (password_rating >= 70) {
            return 3;
        }
        else {
            return (password_rating - 24) / 15;
        }
    }

}
