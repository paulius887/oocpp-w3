#include "user.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <climits>

namespace user {

    const char PW_MIN = 8;
    const char PW_MAX = 50;
    const char NAME_MIN = 5;
    const char NAME_MAX = 50;

    const string GENERATERANDOMPASSWORD_BAD_LENGTHS = "generateRandomPassword received a length_min of < 3, or length_min > length_max";
    const string ALREADY_INIT = "init called to initialize already initialized inner";
    const string NO_INIT = "Attempted to use uninitialized inner";
    const string UID_GEN_OVERFLOW = "init called, but uid_generator is INT_MAX";
    const string SETUSERNAME_TAKEN_USERNAME = "setUsername received an username that's already taken";
    const string SETUSERNAME_NOT_USERNAME = "setUsername received a string that didn't pass isUsername validation";
    const string SETUSERNAME_BAD_LENGTH = "setUsername received a string that's not " + to_string((short)NAME_MIN) + "-" + to_string((short)NAME_MAX) + " characters long";
    const string SETPASSWORD_MISSING_CHARACTERS = "setPassword received a string that doesn't have an upper letter, a number and a special symbol in it";
    const string SETPASSWORD_BAD_LENGTH = "setPassword received a string that's not " + to_string((short)PW_MIN) + "-" + to_string((short)PW_MAX) + " characters long";
    const string SETEMAIL_TAKEN_EMAIL = "setEmail received an email that's already taken";
    const string SETEMAIL_NOT_EMAIL = "setEmail received a string that didn't pass isEmail validation";
    const string INCREMENT_LEVEL_OVERFLOW = "operator++ called, but level is USHRT_MAX";

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    class InitException : public runtime_error { // https://stackoverflow.com/questions/348833
    private:
        string msg;
    public:
        InitException(const string &arg, const char *file, int line);
        ~InitException();
        const char *what() const noexcept;
    };

    InitException::InitException(const string &arg, const char *file, int line) : runtime_error(arg) {
        ostringstream oss;
        oss << file << ":" << line << ": " << arg;
        msg = oss.str();
    }

    InitException::~InitException() {
    }

    const char* InitException::what() const noexcept {
        return msg.c_str();
    }

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    class User::InnerUser {
    private:
        InnerUser();
        ~InnerUser();
        static int uid_generator;
        static vector<string*> takenEmails;
        static vector<string*> takenUsernames;
        const int uid;
        string email;
        string username;
        string password;
        unsigned short level;
        bool isEmail (const string &email) const noexcept;
        bool isEmailNotTaken (const string &email) const;
        bool isUsername (const string &username) const;
        bool isUsernameNotTaken (const string &username) const;
        bool isSpecialCharacter (const char &c) const;
        int getPasswordRating (const string &input_password) const;

        friend class User;
        friend istream& operator>> (istream &o, User &user);
    };

    int User::InnerUser::uid_generator = 0;
    vector<string*> User::InnerUser::takenEmails;
    vector<string*> User::InnerUser::takenUsernames;

    /* - at least one character before '@', before '.' and after '.'
       - first character must be alphabetic
       - all characters (except first) before @ can be alphanumeric, '.', '_' or '-'
       - all characters after @ must be alphanumeric
       - all characters after . must be alphabetic */
    bool User::InnerUser::isEmail (const string &email) const noexcept {
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

    bool User::InnerUser::isEmailNotTaken (const string &email) const {
        for (unsigned i = 0; i < takenEmails.size(); ++i) {
            if (email == *(takenEmails[i])) {
                return 0;
            }
        }
        return 1;
    }

    bool User::InnerUser::isUsername (const string &username) const {
        for (unsigned i = 0; i < username.length(); ++i) {
            if (isSpecialCharacter(username[i])) {
                if (username[i] != '.' && username[i] != '_') { // characters . and _ are allowed in usernames
                    return 0;
                }
            }
        }
        return 1;
    }

    bool User::InnerUser::isUsernameNotTaken (const string &username) const {
        for (unsigned i = 0; i < takenUsernames.size(); ++i) {
            if (username == *(takenUsernames[i])) {
                return 0;
            }
        }
        return 1;
    }

    bool User::InnerUser::isSpecialCharacter (const char &c) const { // ' ' (space) is considered a special symbol
        if (!isalnum(c) && c > 31 && c < 127) {
            return 1;
        }
        else {
            return 0;
        }
    }

    int User::InnerUser::getPasswordRating (const string &input_password) const {
        int password_rating = 0; // <25 - return 0, <40 - return 1, <55 - return 2, >=70 - return 3
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

    User::InnerUser::InnerUser () : uid(uid_generator), level(0)  {
    }

    User::InnerUser::~InnerUser () {
        cout << endl;
        if (username.size() && email.size()) {
            for (unsigned i = 0; i < takenEmails.size(); ++i) {
                if (*(takenEmails[i]) == email) {
                    takenEmails.erase(takenEmails.begin() + i);
                    takenUsernames.erase(takenUsernames.begin() + i);
                    break;
                }
            }
        }
        else if (email.size()) { // if setEmail succeeds but setUsername fails during init or cin
            for (unsigned i = 0; i < takenEmails.size(); ++i) {
                if (*(takenEmails[i]) == email) {
                    takenEmails.erase(takenEmails.begin() + i);
                    break;
                }
            }
        }
        else if (username.size()) {
            for (unsigned i = 0; i < takenUsernames.size(); ++i) {
                if (*(takenUsernames[i]) == username) {
                    takenUsernames.erase(takenUsernames.begin() + i);
                    break;
                }
            }
        }
    }

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    User::User () : inner(NULL) {
    }

    User::~User() {
        delete inner;
    }

    int User::generateRandomNumber (const int &min, const int &max) { // stackoverflow.com/questions/12657962
        int n = max - min + 1;
        int remainder = RAND_MAX % n;
        int x;
        do {
            x = rand();
        }
        while (x >= RAND_MAX - remainder);
        return min + x % n;
    }

    string User::generateRandomPassword (const int &length_min, const int &length_max) {
        if (length_min < 3 || length_min > length_max) {
            throw invalid_argument(GENERATERANDOMPASSWORD_BAD_LENGTHS);
        }
        srand(time(NULL));
        bool digit, uppercase, special;
        string random_password;
        do {
            random_password.clear();
            digit = uppercase = special = 0;
            int length = generateRandomNumber(length_min, length_max);
            for (int i = 0; i < length; ++i) {
                int char_type = generateRandomNumber (0, 3);
                if (char_type == 0) {
                    random_password += generateRandomNumber ('a', 'z');
                }
                else if (char_type == 1) {
                    uppercase = 1;
                    random_password += generateRandomNumber ('A', 'Z');
                }
                else if (char_type == 2) {
                    digit = 1;
                    random_password += generateRandomNumber ('0', '9');
                }
                else {
                    special = 1;
                    int temp = generateRandomNumber (1, 32);
                    if (temp <= 16) {
                        random_password += generateRandomNumber (' ', '/');
                    }
                    else if (temp <= 23) {
                        random_password += generateRandomNumber (':', '@');
                    }
                    else if (temp <= 28) {
                        random_password += generateRandomNumber ('[', '`');
                    }
                    else {
                        random_password += generateRandomNumber ('{', '~');
                    }
                }
            }
        }
        while (!digit || !uppercase || !special);
        return random_password;
    }


    void User::init (const string &email, const string &username, const string &password) {
        if (inner != NULL) {
            throw InitException (ALREADY_INIT, __FILE__, __LINE__);
        }
        try {
            inner = new User::InnerUser();
            if (inner->uid_generator == INT_MAX) {
                throw overflow_error (UID_GEN_OVERFLOW);
            }
            setEmail (email);
            setUsername (username);
            setPassword (password);
        }
        catch (...) {
            delete inner;
            inner = NULL;
            throw;
        }
        ++(inner->uid_generator);
    }

    void User::checkIfInit (InnerUser *inner, const char *file, int line) const {
        if (inner == NULL) {
            throw InitException(NO_INIT, file, line);
        }
    }

    int User::getUid () const {
        checkIfInit(inner, __FILE__, __LINE__);
        return inner->uid;
    }

    unsigned short User::getLevel () const {
        checkIfInit(inner, __FILE__, __LINE__);
        return inner->level;
    }

    string User::getUsername () const {
        checkIfInit(inner, __FILE__, __LINE__);
        return inner->username;
    }

    string User::getPassword () const {
        checkIfInit(inner, __FILE__, __LINE__);
        return inner->password;
    }

    string User::getEmail () const {
        checkIfInit(inner, __FILE__, __LINE__);
        return inner->email;
    }

    void User::setLevel (const unsigned short &level) {
        checkIfInit(inner, __FILE__, __LINE__);
        inner->level = level;
    }

    void User::setUsername (const string &username) {
        checkIfInit(inner, __FILE__, __LINE__);
        if (username.length() >= NAME_MIN && username.length() <= NAME_MAX) {
            if (inner->isUsername(username)) {
                if (inner->isUsernameNotTaken(username)) {
                    inner->username = username;
                    if (inner->uid_generator == inner->uid) { // new user (init)
                        inner->takenUsernames.push_back(&inner->username);
                    }
                    else { // existing user
                    }
                }
                else {
                    throw invalid_argument(SETUSERNAME_TAKEN_USERNAME);
                }
            }
            else {
                throw invalid_argument(SETUSERNAME_NOT_USERNAME);
            }
        }
        else {
            throw invalid_argument(SETUSERNAME_BAD_LENGTH);
        }
    }

    void User::setPassword (const string &password) {
        checkIfInit(inner, __FILE__, __LINE__);
        bool digit = 0, uppercase = 0, special = 0;
        if (password.length() >= PW_MIN && password.length() <= PW_MAX) {
            for (unsigned i = 0; i < password.length(); ++i) {
                if (isdigit(password[i])) {
                    digit = 1;
                }
                else if (isupper(password[i])) {
                    uppercase = 1;
                }
                else if (inner->isSpecialCharacter(password[i])) {
                    special = 1;
                }
            }
            if (digit && uppercase && special) {
                inner->password = password;
            }
            else {
                throw invalid_argument(SETPASSWORD_MISSING_CHARACTERS);
            }
        }
        else {
            throw invalid_argument(SETPASSWORD_BAD_LENGTH);
        }
    }

    void User::setRandomPassword () {
        checkIfInit(inner, __FILE__, __LINE__);
        inner->password = generateRandomPassword(15, 20);
    }

    void User::setEmail (const string &email) {
        checkIfInit(inner, __FILE__, __LINE__);
        if (inner->isEmail(email)) {
            if (inner->isEmailNotTaken(email)) {
                inner->email = email;
                if (inner->uid_generator == inner->uid) { // new user (init)
                    inner->takenEmails.push_back(&inner->email);
                }
                else { // existing user
                }
            }
            else {
                throw invalid_argument(SETEMAIL_TAKEN_EMAIL);
            }
        }
        else {
            throw invalid_argument(SETEMAIL_NOT_EMAIL);
        }
    }

    bool User::operator> (const User &user2) const {
        checkIfInit(inner, __FILE__, __LINE__);
        checkIfInit(user2.inner, __FILE__, __LINE__);
        return (inner->level > user2.inner->level);
    }

    bool User::operator< (const User &user2) const {
        checkIfInit(inner, __FILE__, __LINE__);
        checkIfInit(user2.inner, __FILE__, __LINE__);
        return (inner->level < user2.inner->level);
    }

    bool User::operator<= (const User &user2) const {
        checkIfInit(inner, __FILE__, __LINE__);
        checkIfInit(user2.inner, __FILE__, __LINE__);
        return (inner->level <= user2.inner->level);
    }

    bool User::operator>= (const User &user2) const {
        checkIfInit(inner, __FILE__, __LINE__);
        checkIfInit(user2.inner, __FILE__, __LINE__);
        return (inner->level >= user2.inner->level);
    }

    bool User::operator== (const User &user2) const {
        checkIfInit(inner, __FILE__, __LINE__);
        checkIfInit(user2.inner, __FILE__, __LINE__);
        return (inner->level == user2.inner->level);
    }

    bool User::operator!= (const User &user2) const {
        checkIfInit(inner, __FILE__, __LINE__);
        checkIfInit(user2.inner, __FILE__, __LINE__);
        return (inner->level != user2.inner->level);
    }

    void User::operator++ () {
        checkIfInit(inner, __FILE__, __LINE__);
        if (inner->level == USHRT_MAX) {
            throw overflow_error (INCREMENT_LEVEL_OVERFLOW);
        }
        ++(inner->level);
    }

    unsigned short User::operator++ (int) {
        checkIfInit(inner, __FILE__, __LINE__);
        if (inner->level == USHRT_MAX) {
            throw overflow_error (INCREMENT_LEVEL_OVERFLOW);
        }
        return (inner->level)++;
    }

    string User::toString () const {
        checkIfInit(inner, __FILE__, __LINE__);
        ostringstream oss;
        oss << "uid " << getUid() << endl;
        oss << "level " << getLevel() << endl;
        oss << "email '" << getEmail() << "'" << endl;
        oss << "username '" << getUsername() << "'" << endl;
        oss << "password '" << getPassword() << "' passwordRating(password) = " << inner->getPasswordRating(inner->password) << endl;
        cout << endl;
        return oss.str();
    }

    ostream& operator<< (ostream &o, const User &user) {
        o << user.toString();
        return o;
    }

    istream& operator>> (istream &i, User &user) {
        string new_email, new_user, new_pw;
        cout << "Enter your e-mail address: ";
        i >> new_email;
        cout << "Enter your username: ";
        i >> new_user;
        cout << "Enter your password: ";
        i >> new_pw;
        if (user.inner == NULL) { // new user (init)
            user.init(new_email, new_user, new_pw);
        }
        else { // existing user
            do {
                User temp;
                temp.init(new_email, new_user, new_pw); // validation using temp member
                --(temp.inner->uid_generator);
            }
            while (0);
            user.setEmail(new_email);
            user.setUsername(new_user);
            user.setPassword(new_pw);
        }
        return i;
    }
}
