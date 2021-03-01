#include "user.h"

namespace user {

int User::customRandom (int min, int max){ // stackoverflow.com/questions/12657962
    int n = max - min + 1;
    int remainder = RAND_MAX % n;
    int x;
    do {
        x = rand();
    }
    while (x >= RAND_MAX - remainder);
    return min + x % n;
}

string User::createRandomPassword (int length_min, int length_max) {
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

// at least one character before @, before . and after .
// first character must be alphabetic
// all characters before @ can be alphabetic, numeric, or '.', '_', and '-'
// all characters after @ must be alphabetic or numeric
// all characters after . must be alphabetic
bool User::isEmail (string new_email) {
    if (!isalpha(new_email[0])) {
        return 0;
    }
    for (unsigned i = 0; i < new_email.length(); ++i) {
        if (new_email[i] == '@') {
            if (i == 0) {
                return 0;
            }
            unsigned ver = ++i;
            for (; i < new_email.length(); ++i) {
                if (new_email[i] == '.') {
                    if (i - ver == 0) {
                        return 0;
                    }
                    ver = ++i;
                    for (; i < new_email.length() + 1; ++i) {
                        if (new_email[i] == '\0') {
                            if (i - ver == 0) {
                                return 0;
                            }
                            else {
                                return 1;
                            }
                        }
                        if (!isalpha(new_email[i])) {
                            return 0;
                        }
                    }
                }
                if (!isalnum(new_email[i])) {
                    return 0;
                }
            }
        }
        if (!isalnum(new_email[i]) && new_email[i] != '.' && new_email[i] != '_' && new_email[i] != '-') {
            return 0;
        }
    }
    return 0;
}

bool User::isSpecialCharacter (char c) { // ' ' (space) is considered a special symbol
    if (!isalnum(c) && c > 31 && c < 127) {
        return 1;
    }
    else {
        return 0;
    }
}

int User::passwordRating (string input_password) { // <25 - weak (return 0), <40 - decent (return 1), <55 - strong (return 2), <70 - very strong (return 3)
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

User::User () : unique_id(++uid_generator) {
}

User::~User() {
}

string User::getNickname () {
    return nickname;
}

string User::getPassword () {
    return password;
}

string User::getEmail () {
    return email;
}

void User::setNickname (string new_nickname) {
    if (new_nickname.length() >= NAME_MIN && new_nickname.length() <= NAME_MAX) {
        for (unsigned i = 0; i < new_nickname.length(); ++i) {
            if (isSpecialCharacter(new_nickname[i])) {
                if (new_nickname[i] != '.' && new_nickname[i] != '_') { // characters . and _ are allowed in nicknames
                    throw invalid_argument("setNickname received a string that has invalid characters in it (allowed characters: 0-9, A-Z, special symbols . and _");
                }
            }
        }
        nickname = new_nickname;
    }
    else {
        stringstream ss;
        ss << "setNickname received a string that's either too long or too short (length must be " << NAME_MIN << "-" << NAME_MAX << ")";
        throw invalid_argument(ss.str());
    }
}

void User::setPassword (string new_password) {
    bool digit = 0, uppercase = 0, special = 0;
    if (new_password.length() >= PW_MIN && new_password.length() <= PW_MAX) {
        for (unsigned i = 0; i < new_password.length(); ++i) {
            if (isdigit(new_password[i])) {
                digit = 1;
            }
            else if (isupper(new_password[i])) {
                uppercase = 1;
            }
            else if (isSpecialCharacter(new_password[i])) {
                special = 1;
            }
        }
        if (digit && uppercase && special) {
            password = new_password;
        }
        else {
            throw invalid_argument("setPassword received a string that doesn't have an upper letter, a number and a special symbol in it");
        }
    }
    else {
        stringstream ss;
        ss << "setPassword received a string that's either too long or too short (length must be " << PW_MIN << "-" << PW_MAX << ")";
        throw invalid_argument(ss.str());
    }
}

void User::setRandomPassword () {
    password = createRandomPassword(15, 20);
}

void User::setEmail (string new_email) {
    if (isEmail(new_email)) {
        email = new_email;
    }
    else {
        throw invalid_argument("setEmail received a string that didn't pass isEmail validation");
    }
}

string User::toString () {
    stringstream ss;
    ss << "unique_id " << unique_id << endl;
    ss << "nickname '" << nickname << "'" << endl;
    ss << "password '" << password << "' passwordRating(password) = " << passwordRating(password) << endl;
    ss << "email '" << email << "'" << endl;
    return ss.str();
}

int User::uid_generator = 0;

}
