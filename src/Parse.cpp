#include "Parse.hpp"

#include <iostream>

namespace autojson {

void ParseError(const std::string &message, const char *contentPos) {
    std::cerr << "[ERROR]\t" << message << '\n';
    std::cerr << "[Json]";
    for (int i = 0; i < 20; i += 1, contentPos++) {
        if (*contentPos == '\0') {
            break;
        }
        std::cerr << *contentPos;
    }
    std::cerr << '\n';
}

bool CanSkipWhitespace(const char *content, const std::string &customPass) {
    if (*content == '\0') {
        return false;
    }

    for (char itr : customPass) {
        if (itr == *content) {
            return true;
        }
    }

    if (*content == ' ' or *content == '\t' or *content == '\n' or *content == '\r') {
        return true;
    }

    return false;
}

void SkipWhitespace(const char *&content, const std::string &customPass) {
    while (CanSkipWhitespace(content, customPass)) {
        content++;
    }
}

std::string EscapeKeys(const std::string &content) {
    std::string result = "";
    for (char itr : content) {
        bool needsEscape = false;
        for (char c : "\"\'") {
            if (itr == c) {
                needsEscape = true;
            }
        }

        if (needsEscape) {
            result += '\\';
            result += itr;
            continue;
        }

        if (itr == '\n') {
            result += "\\n";
            continue;
        }

        if (itr == '\r') {
            result += "\\r";
            continue;
        }

        if (itr == '\t') {
            result += "\\t";
            continue;
        }

        result += itr;
    }
    return result;
}

std::string ParseWord(const char *&content) {
    std::string word = "";
    auto IsDelimiter = [&]() {
        for (char itr : "\n \t\r,:{}[]") {
            if (itr == *content) {
                return true;
            }
        }

        return false;
    };

    while (!IsDelimiter()) {
        word += *content;
        content++;
    }

    return word;
}

std::string ParseString(const char *&content) {
    if (*content == '\"' or *content == '\'') {
        content++;
    }

    std::string txt = "";
    bool escaped = false;
    while (not ((*content == '\"' or *content == '\'') and (escaped == false))) {
        if (escaped) {
            if (*content != '\'' and *content != '\"') {
                txt += '\\';
            }
            txt += *content;
            content++;
            escaped = false;
            continue;
        }

        if (*content == '\\') {
            escaped = true;
            content++;
            continue;
        }

        txt += *content;
        content++;
    }
    content++;

    return txt;
}

} // namespace autojson
