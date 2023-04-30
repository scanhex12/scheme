#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <string>
#include "error.h"

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const {
        return other.value == value;
    }
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in) : in_(in) {
    }

    bool IsEnd() {
        if (!next_called_) {
            return false;
        }
        while (in_->peek() != EOF && (in_->peek() == ' ' || in_->peek() == '\n')) {
            in_->get();
        }
        return in_->peek() == EOF;
    }

    void Next() {
        if (must_move_) {
            in_->get();
        }
        must_move_ = true;
        next_called_ = true;
        while (in_->peek() != EOF && (in_->peek() == ' ' || in_->peek() == '\n')) {
            in_->get();
        }
        if (in_->peek() != EOF) {
            GetToken();
        }
    }

    Token GetToken() {
        if (!next_called_) {
            return prev_token_;
        }
        next_called_ = false;
        if (in_->peek() == '(') {
            prev_token_ = BracketToken::OPEN;
            return prev_token_;
        } else if (in_->peek() == ')') {
            prev_token_ = BracketToken::CLOSE;
            return prev_token_;
        } else if (in_->peek() == '.') {
            prev_token_ = DotToken();
            return prev_token_;
        } else if (in_->peek() == '\'') {
            prev_token_ = QuoteToken();
            return prev_token_;
        } else {
            std::string cur;
            int prev_type = GetType(in_->peek());
            while (in_->peek() != EOF && in_->peek() != ' ' && in_->peek() != '\n') {
                int cur_type = GetType(in_->peek());
                if (prev_type != cur_type && !(prev_type == 7 && cur_type == 5) && cur_type != 7 &&
                    !(prev_type == 8 && cur_type == 5)) {
                    break;
                }
                cur.push_back(in_->peek());
                in_->get();
            }
            must_move_ = false;
            try {
                prev_token_ = ConstantToken{std::stoi(cur)};
                return prev_token_;
            } catch (...) {
                prev_token_ = SymbolToken{cur};
                return prev_token_;
            }
        }
    }

private:
    bool IsDigit(char c) {
        return ('0' <= c && c <= '9');
    }

    bool IsAlpha(char c) {
        return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '<' || c == '=' ||
                c == '>' || c == '*' || c == '/' || c == '#' || c == '?' || c == '!');
    }

    int GetType(char c) {
        if (in_->peek() == '(') {
            return 1;
        } else if (in_->peek() == ')') {
            return 2;
        } else if (in_->peek() == '.') {
            return 3;
        } else if (in_->peek() == '\'') {
            return 4;
        } else if (IsDigit(c)) {
            return 5;
        } else if (c == '-') {
            return 7;
        } else if (IsAlpha(c)) {
            return 6;
        } else if (c == '+') {
            return 8;
        } else if (c == ' ') {
            return 9;
        }
        std::string error_message = "Syntax error:";
        error_message.push_back(c);
        throw SyntaxError(error_message);
    }

    bool must_move_ = true;
    char next_called_ = true;
    Token prev_token_ = QuoteToken();
    std::istream* in_;
};
