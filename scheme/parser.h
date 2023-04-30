#pragma once

#include <memory>

#include "object.h"
#include <tokenizer.h>
#include <cassert>
#include "error.h"

inline std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    assert(tokenizer->GetToken() == Token{BracketToken::OPEN});

    std::shared_ptr<Object> root = std::shared_ptr<Object>(new Cell(nullptr, nullptr, nullptr));
    tokenizer->Next();
    if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
        tokenizer->Next();
        return nullptr;
    }
    std::shared_ptr<Object> current_root = root;
    while (!(tokenizer->GetToken() == Token{BracketToken::CLOSE})) {
        if (tokenizer->GetToken() == Token{QuoteToken()}) {
            tokenizer->Next();
        }
        if (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
            std::shared_ptr<Cell> cur = As<Cell>(current_root);
            cur->GetFirst() = ReadList(tokenizer);
            if (Is<Cell>(cur->GetFirst())) {
                As<Cell>(cur->GetFirst())->GetParent() = cur;
            }
        } else {
            std::shared_ptr<Cell> cur = As<Cell>(current_root);
            try {
                int value = std::get<ConstantToken>(tokenizer->GetToken()).value;
                cur->GetFirst() = std::shared_ptr<Number>(new Number(value));
            } catch (...) {
                try {
                    std::string value = std::get<SymbolToken>(tokenizer->GetToken()).name;
                    cur->GetFirst() = std::shared_ptr<Symbol>(new Symbol(value));
                } catch (...) {
                    throw SyntaxError("Parser error");
                }
            }
            tokenizer->Next();
        }
        if (tokenizer->GetToken() == Token{DotToken()}) {
            tokenizer->Next();
            if (tokenizer->GetToken() == Token{QuoteToken()}) {
                tokenizer->Next();
            }
            if (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
                std::shared_ptr<Cell> cur = As<Cell>(current_root);
                cur->GetSecond() = ReadList(tokenizer);
                if (Is<Cell>(cur->GetSecond())) {
                    As<Cell>(cur->GetSecond())->GetParent() = cur;
                }
            } else {
                std::shared_ptr<Cell> cur = As<Cell>(current_root);
                try {
                    int value = std::get<ConstantToken>(tokenizer->GetToken()).value;
                    cur->GetSecond() = std::shared_ptr<Number>(new Number(value));
                } catch (...) {
                    try {
                        std::string value = std::get<SymbolToken>(tokenizer->GetToken()).name;
                        cur->GetSecond() = std::shared_ptr<Symbol>(new Symbol(value));
                    } catch (...) {
                        throw SyntaxError("Parser error");
                    }
                }
                tokenizer->Next();
            }
            break;
        } else if (!(tokenizer->GetToken() == Token{BracketToken::CLOSE})) {
            As<Cell>(current_root)->GetSecond() =
                std::shared_ptr<Cell>(new Cell(nullptr, nullptr, nullptr));
            As<Cell>(As<Cell>(current_root)->GetSecond())->GetParent() = current_root;
            current_root = As<Cell>(current_root)->GetSecond();
        }
    }
    if (!(tokenizer->GetToken() == Token{BracketToken::CLOSE})) {
        throw SyntaxError("Parser error");
    }
    tokenizer->Next();
    return root;
}

inline std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    std::shared_ptr<Object> root = nullptr;
    bool is_quote = false;
    if (tokenizer->GetToken() == Token{QuoteToken()}) {
        tokenizer->Next();
        is_quote = true;
    }
    if (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
        root = ReadList(tokenizer);
    } else {
        std::shared_ptr<Cell> cur = As<Cell>(root);
        try {
            int value = std::get<ConstantToken>(tokenizer->GetToken()).value;
            root = std::shared_ptr<Object>(new Number(value));
            tokenizer->Next();
        } catch (...) {
            try {
                std::string value = std::get<SymbolToken>(tokenizer->GetToken()).name;
                root = std::shared_ptr<Object>(new Symbol(value));
                tokenizer->Next();
            } catch (...) {
                try {
                    std::get<QuoteToken>(tokenizer->GetToken());
                    root = std::shared_ptr<Object>(new Symbol("'"));
                    tokenizer->Next();
                } catch (...) {
                    throw SyntaxError("Parser error");
                }
            }
        }
    }
    if (is_quote) {
        if (!root) {
            root = std::shared_ptr<Object>(new Symbol("()"));
        }
        root->is_quote = true;
    }
    return root;
}
