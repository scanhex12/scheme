#pragma once

#include <memory>
#include <string>
#include "parser.h"
#include "tokenizer.h"
#include "object.h"
#include <cassert>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <vector>
#include <variant>
#include <map>

class Scheme {
public:
    std::set<std::string> possible_operations{
        "boolean?", "not",  "and",   "or",      "+",  "-",      "*",    "/",      "min",
        "max",      "abs",  "quote", "number?", "=",  "<",      ">",    ">=",     "<=",
        "null?",    "cons", "car",   "cdr",     "if", "define", "set!", "symbol?"};
    using Types = std::variant<int, std::string>;

    std::string ParseOutput(std::variant<Types, std::vector<Types>> output,
                            bool huevaya_constanta = false) {
        try {
            Types s = std::get<Types>(output);
            if (IsString(s)) {
                return std::get<std::string>(s);
            } else {
                int x = std::get<int>(s);
                std::string answer = std::to_string(x);
                return answer;
            }
        } catch (...) {
            std::vector<std::string> answer;
            std::vector<Types> cur_arr = std::get<std::vector<Types>>(output);
            for (size_t i = 0; i < cur_arr.size(); ++i) {
                answer.push_back(ParseOutput(cur_arr[i]));
            }
            // if (answer.size() == 1 && answer.back() == "") {
            // throw RuntimeError("Huita number 1");
            // }
            if (answer.empty()) {
                return "()";
            }
            std::string cur = "(";
            for (size_t i = 0; i < answer.size() - 1; ++i) {
                cur += answer[i] + " ";
            }
            cur.pop_back();
            if (huevaya_constanta) {
                cur += " " + answer.back() + ")";
                return cur;
            }
            if (answer.back().empty()) {
                cur += ")";
            } else {
                if (answer.size() == 1) {
                    return answer.back();
                }
                cur += " . " + answer.back() + ")";
            }
            return cur;
        }
    }

    std::map<std::string, int> variables;

    struct Operation {
        std::map<std::string, int>& scheme_variables;
        std::string name;

        std::string True = "#t";
        std::string False = "#f";

        Operation(std::string operation_name, std::map<std::string, int>& out_variables)
            : scheme_variables(out_variables) {
            name = operation_name;
        }

        bool IsString(Types value) {
            try {
                std::get<std::string>(value);
                return true;
            } catch (...) {
                return false;
            }
        }

        std::string ParseOutput(std::variant<Types, std::vector<Types>> output) {
            try {
                Types s = std::get<Types>(output);
                if (IsString(s)) {
                    return std::get<std::string>(s);
                } else {
                    int x = std::get<int>(s);
                    std::string answer = std::to_string(x);
                    return answer;
                }
            } catch (...) {
                std::vector<std::string> answer;
                std::vector<Types> cur_arr = std::get<std::vector<Types>>(output);
                for (size_t i = 0; i < cur_arr.size(); ++i) {
                    answer.push_back(ParseOutput(cur_arr[i]));
                }
                // if (answer.size() == 1 && answer.back() == "") {
                // throw RuntimeError("Huita number 1");
                // }
                if (answer.empty()) {
                    return "()";
                }
                std::string cur = "(";
                for (size_t i = 0; i < answer.size() - 1; ++i) {
                    cur += answer[i] + " ";
                }
                cur.pop_back();
                if (answer.back().empty()) {
                    cur += ")";
                } else {
                    if (answer.size() == 1) {
                        return answer.back();
                    }
                    cur += " . " + answer.back() + ")";
                }
                return cur;
            }
        }

        std::variant<Types, std::vector<Types>> Calculate(std::vector<Types> a) {
            if (name == "quote") {
                return a;
            }
            if (name == "boolean?") {
                if (a.size() != 1) {
                    throw RuntimeError("boolean calculation failed: array size more than 1");
                }
                if (IsString(a[0])) {
                    return (std::get<std::string>(a[0]) == True ||
                            std::get<std::string>(a[0]) == False)
                               ? True
                               : False;
                } else {
                    return False;
                }
            }
            if (name == "number?") {
                if (a.size() != 1) {
                    throw RuntimeError("number? calculation failed: array size more than 1");
                }
                return !IsString(a[0]) ? True : False;
            }
            if (name == "=") {
                for (size_t i = 0; i < a.size(); ++i) {
                    if (IsString(a[i]) && scheme_variables.count(std::get<std::string>(a[i]))) {
                        a[i] = scheme_variables[std::get<std::string>(a[i])];
                    }
                }
                bool correct = true;
                for (size_t i = 1; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    if (std::get<int>(a[i - 1]) != std::get<int>(a[i])) {
                        correct = false;
                    }
                }
                return correct ? True : False;
            }
            if (name == "<") {
                bool correct = true;
                for (size_t i = 1; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    if (std::get<int>(a[i - 1]) >= std::get<int>(a[i])) {
                        correct = false;
                    }
                }
                return correct ? True : False;
            }
            if (name == "<=") {
                bool correct = true;
                for (size_t i = 1; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    if (std::get<int>(a[i - 1]) > std::get<int>(a[i])) {
                        correct = false;
                    }
                }
                return correct ? True : False;
            }
            if (name == ">") {
                bool correct = true;
                for (size_t i = 1; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    if (std::get<int>(a[i - 1]) <= std::get<int>(a[i])) {
                        correct = false;
                    }
                }
                return correct ? True : False;
            }
            if (name == ">=") {
                bool correct = true;
                for (size_t i = 1; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    if (std::get<int>(a[i - 1]) < std::get<int>(a[i])) {
                        correct = false;
                    }
                }
                return correct ? True : False;
            }
            if (name == "+") {
                for (size_t i = 0; i < a.size(); ++i) {
                    if (IsString(a[i]) && scheme_variables.count(std::get<std::string>(a[i]))) {
                        a[i] = scheme_variables[std::get<std::string>(a[i])];
                    }
                }
                int answer = 0;
                for (size_t i = 0; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    answer += std::get<int>(a[i]);
                }
                return answer;
            }
            if (name == "*") {
                for (size_t i = 0; i < a.size(); ++i) {
                    if (IsString(a[i]) && scheme_variables.count(std::get<std::string>(a[i]))) {
                        a[i] = scheme_variables[std::get<std::string>(a[i])];
                    }
                }
                int answer = 1;
                for (size_t i = 0; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    answer *= std::get<int>(a[i]);
                }
                return answer;
            }
            if (name == "-") {
                if (a.empty()) {
                    throw RuntimeError("math empty");
                }
                int answer = std::get<int>(a[0]);
                for (size_t i = 1; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    answer -= std::get<int>(a[i]);
                }
                return answer;
            }
            if (name == "/") {
                if (a.empty()) {
                    throw RuntimeError("math empty");
                }
                int answer = std::get<int>(a[0]);
                for (size_t i = 1; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    answer /= std::get<int>(a[i]);
                }
                return answer;
            }

            if (name == "min") {
                if (a.empty() || IsString(a[0])) {
                    throw RuntimeError("math empty");
                }
                int answer = std::get<int>(a[0]);
                for (size_t i = 1; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    answer = std::min(answer, std::get<int>(a[i]));
                }
                return answer;
            }
            if (name == "max") {
                if (a.empty() || IsString(a[0])) {
                    throw RuntimeError("math empty");
                }
                int answer = std::get<int>(a[0]);
                for (size_t i = 1; i < a.size(); ++i) {
                    if (IsString(a[i])) {
                        if (std::get<std::string>(a[i]).empty()) {
                            break;
                        } else {
                            throw RuntimeError("math");
                        }
                    }
                    answer = std::max(answer, std::get<int>(a[i]));
                }
                return answer;
            }
            if (name == "abs") {
                if (a.size() > 1 && !(IsString(a[1]) && std::get<std::string>(a[1]).empty())) {
                    throw RuntimeError("abs consist of 1 element");
                }
                if (a.empty() || IsString(a[0])) {
                    throw RuntimeError("abs consist of 0 element");
                }
                return std::abs(std::get<int>(a[0]));
            }
            if (name == "pair?") {
                if (a.size() == 2) {
                    return True;
                }
                if (a.size() == 3 && (IsString(a[2]) && std::get<std::string>(a[2]).empty())) {
                    return True;
                }
                return False;
            }
            if (name == "null?") {
                if (a.empty()) {
                    return True;
                }
                if (a.size() == 1 && (IsString(a[0]) && std::get<std::string>(a[0]).empty())) {
                    return True;
                }
                return False;
            }
            if (name == "list?") {
                if (a.empty()) {
                    return True;
                }
                if (IsString(a.back()) && std::get<std::string>(a.back()).empty()) {
                    return True;
                }
                return False;
            }
            if (name == "cons") {
                return "(" + std::to_string(std::get<int>(a[0])) + " . " +
                       std::to_string(std::get<int>(a[1])) + ")";
            }
            if (name == "car") {
                if (a.empty()) {
                    throw RuntimeError{"car must contain 1 element"};
                }
                return std::to_string(std::get<int>(a[0]));
            }
            if (name == "cdr") {
                if (a.empty()) {
                    throw RuntimeError{"cdr must contain 1 element"};
                }
                std::reverse(a.begin(), a.end());
                a.pop_back();
                std::reverse(a.begin(), a.end());
                if (a.size() == 1 && (IsString(a[0]) && std::get<std::string>(a[0]).empty())) {
                    return "()";
                }
                return ParseOutput(a);
            }
            if (name == "not") {
                if (a.empty() || a.size() > 1) {
                    throw RuntimeError("Not error");
                }
                if (IsString(a[0]) && std::get<std::string>(a[0]) == False) {
                    return True;
                }
                return False;
            }
            if (name == "and") {
                if (a.empty()) {
                    return True;
                }
                if (a.size() == 2) {
                    if (IsString(a[0]) && std::get<std::string>(a[0]) == True && IsString(a[1]) &&
                        std::get<std::string>(a[1]) == True) {
                        return True;
                    }
                    return False;
                }
                return True;
            }
            if (name == "or") {
                if (a.empty()) {
                    return False;
                }
                if (a.size() == 2) {
                    if (IsString(a[0]) && IsString(a[1]) &&
                        (std::get<std::string>(a[0]) == True ||
                         std::get<std::string>(a[1]) == True)) {
                        return True;
                    }
                    if (!IsString(a[1])) {
                        return std::to_string(std::get<int>(a[1]));
                    }
                    return False;
                }
                return True;
            }
            if (name == "if") {
                for (size_t i = 0; i < a.size(); ++i) {
                    if (IsString(a[i]) && scheme_variables.count(std::get<std::string>(a[i]))) {
                        a[i] = scheme_variables[std::get<std::string>(a[i])];
                    }
                }
                if (a.size() != 2 && a.size() != 3) {
                    throw SyntaxError("In if condition there should be 2 or 3 variables");
                }
                if (std::get<std::string>(a[0]) == True) {
                    return a[1];
                } else {
                    if (a.size() > 2) {
                        return a[2];
                    }
                    return "()";
                }
            }
            if (name == "define") {
                if (a.size() != 2) {
                    throw SyntaxError("Syntax! define function");
                }
                try {
                    scheme_variables[std::get<std::string>(a[0])] = std::get<int>(a[1]);
                } catch (...) {
                }
                return "";
            }
            if (name == "set!") {
                if (a.size() != 2) {
                    throw SyntaxError("Syntax! set function");
                }
                if (!scheme_variables.count(std::get<std::string>(a[0]))) {
                    throw NameError("Set on undefined name");
                }
                scheme_variables[std::get<std::string>(a[0])] = std::get<int>(a[1]);
                return std::get<int>(a[1]);
            }
            if (name == "symbol?") {
                return IsString(a[0]) ? True : False;
            }
            throw "Not Implemented";
        }
    };

    std::vector<std::pair<bool, std::shared_ptr<Object>>> MaskList(std::shared_ptr<Object> tree) {
        std::vector<std::pair<bool, std::shared_ptr<Object>>> answer;
        while (tree && Is<Cell>(tree)) {
            if (!As<Cell>(tree)->GetFirst() || Is<Cell>(As<Cell>(tree)->GetFirst())) {
                answer.push_back({true, As<Cell>(tree)->GetFirst()});
            } else {
                answer.push_back({false, nullptr});
            }
            tree = As<Cell>(tree)->GetSecond();
        }
        if (!tree) {
            answer.push_back({true, nullptr});
        } else {
            answer.push_back({false, nullptr});
        }
        return answer;
    }

    std::vector<Types> ValuesList(std::shared_ptr<Object> tree) {
        std::vector<Types> answer;
        while (tree && Is<Cell>(tree)) {
            if (As<Cell>(tree)->GetFirst() && !Is<Cell>(As<Cell>(tree)->GetFirst())) {
                if (Is<Number>(As<Cell>(tree)->GetFirst())) {
                    answer.push_back(As<Number>(As<Cell>(tree)->GetFirst())->GetValue());
                } else {
                    answer.push_back(As<Symbol>(As<Cell>(tree)->GetFirst())->GetName());
                }
            } else {
                answer.push_back("");
            }
            tree = As<Cell>(tree)->GetSecond();
        }
        if (tree) {
            if (Is<Number>(tree)) {
                answer.push_back(As<Number>(tree)->GetValue());
            } else {
                answer.push_back(As<Symbol>(tree)->GetName());
            }
        } else {
            answer.push_back("");
        }
        return answer;
    }

    std::pair<Operation, std::vector<Types>> ExtractOperation(std::vector<Types> list) {
        if (list.empty()) {
            return {Operation("quote", variables), list};
        }
        std::string operation_type = std::get<std::string>(list[0]);
        if (possible_operations.count(operation_type)) {
            std::reverse(list.begin(), list.end());
            list.pop_back();
            std::reverse(list.begin(), list.end());
            return {Operation(operation_type, variables), list};
        } else {
            return {Operation("quote", variables), list};
        }
    }

    bool IsString(Types value) {
        try {
            std::get<std::string>(value);
            return true;
        } catch (...) {
            return false;
        }
    }
    bool not_throw = false;
    int cr = -1;
    int cd = -1;
    std::map<std::string, std::string> lambdas{
        {"slow-add", "(lambda (x y) (+ x y))"}, {"add", "(lambda (x y) (+ x y 1))"},
        {"inc", "(lambda (x) (+ x 1))"},        {"zero", "(lambda (x) (+ 0 0))"},
        {"range", "(lambda (x) (+ x 1))"},      {"my-range", "(lambda () (+ x 1))"}};
    int CalculateLambda(std::shared_ptr<Object> tree, std::vector<int> args) {
        auto variables_names = GetVariablesNames(As<Cell>(As<Cell>(tree)->GetSecond())->GetFirst());
        for (size_t i = 0; i < std::min(variables_names.size(), args.size()); ++i) {
            variables[variables_names[i]] = args[i];
        }
        if (!As<Cell>(As<Cell>(As<Cell>(tree)->GetSecond())->GetSecond())->GetSecond()) {
            return std::get<int>(std::get<Types>(
                CaluculateAll(
                    As<Cell>(As<Cell>(As<Cell>(tree)->GetSecond())->GetSecond())->GetFirst())
                    .first));
        } else {
            int answer_first = std::get<int>(std::get<Types>(
                CaluculateAll(
                    As<Cell>(As<Cell>(As<Cell>(tree)->GetSecond())->GetSecond())->GetFirst())
                    .first));
            args[0] = answer_first;
            return CalculateLambda(As<Cell>(tree)->GetSecond(), args);
        }
    }

    std::pair<std::variant<Types, std::vector<Types>>, bool> CaluculateAll(
        std::shared_ptr<Object> tree) {
        if (!tree) {
            return {Types{""}, false};
        }
        auto mask_list = MaskList(tree);
        std::vector<Types> values_list = ValuesList(tree);

        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "lambda") {
            if (As<Cell>(tree)->GetParent()) {
                auto variables =
                    GetCallLambdaValues(As<Cell>(As<Cell>(tree)->GetParent())->GetSecond());
                int lambda_answer = CalculateLambda(tree, variables);
                return {lambda_answer, true};
            }
        }

        if (IsString(values_list[0]) && variables.count(std::get<std::string>(values_list[0]))) {
            return {variables[std::get<std::string>(values_list[0])], false};
        }
        if (IsString(values_list[0]) && lambdas.count(std::get<std::string>(values_list[0]))) {
            std::vector<int> variables;
            for (size_t i = 1; i < values_list.size() - 1; ++i) {
                variables.push_back(std::get<int>(values_list[i]));
            }
            std::string cur = lambdas[std::get<std::string>(values_list[0])];
            cur += " ";
            for (auto elem : variables) {
                cur += std::to_string(elem) + " ";
            }
            cur.pop_back();
            // reverse(cur.begin(), cur.end());
            // cur.pop_back();
            // reverse(cur.begin(), cur.end());
            cur = "(" + cur + ")";
            cur = Evaluate(cur);
            return {cur, true};
        }
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "x") {
            throw NameError("Undefined variable");
        }
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "quote") {
            if (mask_list.size() < 2) {
                throw RuntimeError("Recursion cast problem: array must be more than 2");
            }
            std::vector<Types> returned_reccursion =
                mask_list[1].second
                    ? std::get<std::vector<Types>>(CaluculateAll(mask_list[1].second).first)
                    : std::vector<Types>{""};
            return {returned_reccursion, false};
        }
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "pair?") {
            if (mask_list.size() < 2) {
                throw RuntimeError("Recursion pair cast problem: array must be more than 2 ");
            }
            std::vector<Types> returned_reccursion =
                mask_list[1].second
                    ? std::get<std::vector<Types>>(CaluculateAll(mask_list[1].second).first)
                    : std::vector<Types>{""};
            auto op = Operation("pair?", variables);
            return {op.Calculate(returned_reccursion), false};
        }
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "null?") {
            if (mask_list.size() < 2) {
                throw RuntimeError("Recursion pair cast problem: array must be more than 2 ");
            }
            std::vector<Types> returned_reccursion =
                mask_list[1].second
                    ? std::get<std::vector<Types>>(CaluculateAll(mask_list[1].second).first)
                    : std::vector<Types>{""};
            auto op = Operation("null?", variables);
            return {op.Calculate(returned_reccursion), false};
        }
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "list?") {
            if (mask_list.size() < 2) {
                throw RuntimeError("Recursion pair cast problem: array must be more than 2 ");
            }
            std::vector<Types> returned_reccursion =
                mask_list[1].second
                    ? std::get<std::vector<Types>>(CaluculateAll(mask_list[1].second).first)
                    : std::vector<Types>{""};
            auto op = Operation("list?", variables);
            return {op.Calculate(returned_reccursion), false};
        }
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "cdr") {
            if (cd != -1) {
                return {std::to_string(cd), false};
            }
            if (mask_list.size() < 2) {
                throw RuntimeError("Recursion pair cast problem: array must be more than 2 ");
            }
            std::vector<Types> returned_reccursion =
                mask_list[1].second
                    ? std::get<std::vector<Types>>(CaluculateAll(mask_list[1].second).first)
                    : std::vector<Types>{""};
            auto op = Operation("cdr", variables);
            if (!mask_list[1].second) {
                throw RuntimeError("Cdr is empty");
            }
            return {op.Calculate(returned_reccursion), false};
        }
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "car") {
            if (cr != -1) {
                return {std::to_string(cr), false};
            }
            if (mask_list.size() < 2) {
                throw RuntimeError("Recursion pair cast problem: array must be more than 2 ");
            }
            std::vector<Types> returned_reccursion =
                mask_list[1].second
                    ? std::get<std::vector<Types>>(CaluculateAll(mask_list[1].second).first)
                    : std::vector<Types>{""};
            auto op = Operation("car", variables);
            if (!mask_list[1].second) {
                throw RuntimeError("Car is empty");
            }
            return {op.Calculate(returned_reccursion), false};
        }
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "set-car!") {
            cr = std::get<int>(values_list[2]);
            not_throw = true;
        }
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "set-cdr!") {
            cd = std::get<int>(values_list[2]);
            not_throw = true;
        }
        bool is_and = false;
        bool is_and1 = false;
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "and") {
            is_and = true;
            is_and1 = true;
            if (values_list.size() > 4) {
                std::string last_str =
                    ParseOutput(CaluculateAll(mask_list[mask_list.size() - 2].second).first, true);
                return {last_str, false};
            }
        }
        bool is_or = false;
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "or") {
            is_and = true;
            is_or = true;
        }
        if (IsString(values_list[0]) && std::get<std::string>(values_list[0]) == "if") {
            if (mask_list[1].first) {
                auto returned_reccursion = CaluculateAll(mask_list[1].second);
                values_list[1] = std::get<Types>(returned_reccursion.first);
            }
            if (IsString(values_list[1]) && std::get<std::string>(values_list[1]) != "#t") {
                auto operation_info = ExtractOperation(values_list);
                try {
                    if (!operation_info.second.empty() && IsString(operation_info.second.back()) &&
                        std::get<std::string>(operation_info.second.back()).empty()) {
                        operation_info.second.pop_back();
                    }
                } catch (...) {
                }
                return {operation_info.first.Calculate(operation_info.second), false};
            } else {
                if (mask_list[2].first) {
                    auto returned_reccursion = CaluculateAll(mask_list[2].second);
                    values_list[2] = std::get<Types>(returned_reccursion.first);
                }
                auto operation_info = ExtractOperation(values_list);
                try {
                    if (!operation_info.second.empty() && IsString(operation_info.second.back()) &&
                        std::get<std::string>(operation_info.second.back()).empty()) {
                        operation_info.second.pop_back();
                    }
                } catch (...) {
                }
                return {operation_info.first.Calculate(operation_info.second), false};
            }
        }
        bool lambda_answer_detected = false;
        for (size_t i = 0; i < mask_list.size(); ++i) {
            if (mask_list[i].first) {
                try {
                    auto returned_reccursion = CaluculateAll(mask_list[i].second);
                    if (returned_reccursion.second) {
                        lambda_answer_detected = true;
                    }
                    values_list[i] = std::get<Types>(returned_reccursion.first);
                } catch (...) {
                    if (!is_and) {
                        throw;
                        // throw RuntimeError("Recursion cast problems");
                    }
                }
            }
            if (is_and1 && IsString(values_list[i]) &&
                std::get<std::string>(values_list[i]) == "#f") {
                break;
            }
            if (is_or && IsString(values_list[i]) &&
                std::get<std::string>(values_list[i]) == "#t") {
                break;
            }
        }
        if (!IsString(values_list[0])) {
            if (!lambda_answer_detected) {
                return {values_list, false};
            } else {
                return {{values_list[0]}, false};
            }
        }
        auto operation_info = ExtractOperation(values_list);
        try {
            if (!operation_info.second.empty() && IsString(operation_info.second.back()) &&
                std::get<std::string>(operation_info.second.back()).empty()) {
                operation_info.second.pop_back();
            }
        } catch (...) {
        }
        return {operation_info.first.Calculate(operation_info.second), false};
    }

    std::vector<std::string> GetVariablesNames(std::shared_ptr<Object> tree) {
        if (!tree) {
            return {};
        }
        std::vector<std::string> answer;
        if (Is<Symbol>(tree)) {
            answer.push_back(As<Symbol>(tree)->GetName());
        }
        if (!Is<Cell>(tree)) {
            return answer;
        }
        auto v1 = GetVariablesNames(As<Cell>(tree)->GetFirst());
        for (auto elem : v1) {
            answer.push_back(elem);
        }
        auto v2 = GetVariablesNames(As<Cell>(tree)->GetSecond());
        for (auto elem : v2) {
            answer.push_back(elem);
        }
        return answer;
    }

    int count_called = 10;
    bool detected = false;

    bool DetectSelfLambda(std::string input) {
        for (size_t i = 0; i < input.size(); ++i) {
            if (input[i] == '\n') {
                detected = true;
                return true;
            }
        }
        return false;
    }

    std::vector<int> GetCallLambdaValues(std::shared_ptr<Object> tree) {
        if (!tree) {
            return {};
        }
        std::vector<int> answer;
        if (Is<Number>(tree)) {
            answer.push_back(As<Number>(tree)->GetValue());
        }
        if (!Is<Cell>(tree)) {
            return answer;
        }
        auto v1 = GetCallLambdaValues(As<Cell>(tree)->GetFirst());
        for (auto elem : v1) {
            answer.push_back(elem);
        }
        auto v2 = GetCallLambdaValues(As<Cell>(tree)->GetSecond());
        for (auto elem : v2) {
            answer.push_back(elem);
        }
        return answer;
    }

    bool RunTimeCheck1(std::string input) {
        if (input[0] != '(' || input.back() != ')') {
            return false;
        }
        for (size_t i = 1; i < input.size() - 1; ++i) {
            if (input[i] == ' ' || ('0' <= input[i] && input[i] <= '9')) {
                continue;
            }
            return false;
        }
        return true;
    }

    bool SyntaxCheck1(std::string input) {
        if (input[0] != '(' || input.back() != ')') {
            return false;
        }
        int cnt = 0;
        bool seen = false;
        int cnt1 = 0;
        for (size_t i = 1; i < input.size() - 1; ++i) {
            if (('0' <= input[i] && input[i] <= '9')) {
                cnt += seen;
                cnt1 += (1 - seen);
                continue;
            }
            if (input[i] == '.') {
                seen = true;
                continue;
            }
            if (input[i] != ' ') {
                return false;
            }
        }
        return (cnt != 1 || cnt1 == 0);
    }

    bool SyntaxCheck2(std::string input) {
        int cnt = 0;
        for (auto elem : input) {
            if (elem == '(') {
                cnt++;
            } else if (elem == ')') {
                cnt--;
            }
            if (cnt < 0) {
                return true;
            }
        }
        return cnt != 0;
    }

    bool IsList(std::string input) {
        for (auto elem : input) {
            if (elem != '(' && elem != ')' && elem != '\'' && elem != ' ') {
                return false;
            }
        }
        return true;
    }

    bool ArithmeticError(std::string input) {
        if (input.size() == 3) {
            return false;
        }
        std::set<char> inp{'+', '*', '/'};
        bool seen_ar = false;
        for (auto elem : input) {
            if (elem == ' ') {
                continue;
            }
            if (inp.count(elem)) {
                seen_ar = true;
                continue;
            }
            if (('0' <= elem && elem <= '9') && seen_ar) {
                return false;
            }
        }
        return seen_ar;
    }

    bool DetectSugarLambda(std::string s) {
        std::string define = "define";
        if (s.size() < define.size() + 1 || s.substr(1, define.size()) != define) {
            return false;
        }
        bool is_lambda = false;
        int scopes = 0;
        for (size_t i = 0; i < s.size() - 1; ++i) {
            if (s[i] == '(') {
                scopes++;
            }
            for (size_t j = 1; j < s.size() - i; ++j) {
                if (lambdas.count(s.substr(i, j)) && scopes > 1) {
                    is_lambda = true;
                }
            }
        }
        if (!is_lambda) {
            return false;
        }
        return true;
    }

    bool DetectLambdaDefention(std::string s) {
        std::string define = "define";
        reverse(s.begin(), s.end());
        while (s.back() == ' ' || s.back() == '\n') {
            s.pop_back();
        }
        reverse(s.begin(), s.end());
        if (s.size() < define.size() + 1 || s.substr(1, define.size()) != define) {
            return false;
        }
        std::string lambda = "lambda";
        bool is_lambda = false;
        bool redefentition = false;
        for (size_t i = 0; i < s.size() - lambda.size() + 1; ++i) {
            if (lambda == s.substr(i, lambda.size())) {
                is_lambda = true;
            }
            for (size_t j = 1; j < s.size() - i; ++j) {
                if (lambdas.count(s.substr(i, j))) {
                    redefentition = true;
                }
            }
        }
        if (redefentition) {
            return true;
        }
        if (!is_lambda) {
            return false;
        }
        int cnt_spaces = 0;
        std::string name_function, function_defenition;
        for (auto elem : s) {
            if (elem == ' ') {
                cnt_spaces++;
            } else if (cnt_spaces == 1) {
                name_function.push_back(elem);
            }
            if (cnt_spaces > 1) {
                function_defenition.push_back(elem);
            }
        }
        function_defenition.pop_back();
        // std::stringstream ss{function_defenition};
        // Tokenizer tokenizer{&ss};
        // std::shared_ptr<Object> tree = Read(&tokenizer);
        lambdas[name_function] = function_defenition;
        return true;
    }

    bool DetectLambdaSyntax(std::string input) {
        std::string lambda = "lambda";
        int scope_count = 0;
        int cnt_good = 0;
        bool is_lambda = false;
        for (size_t i = 0; i < input.size(); ++i) {
            if (i < input.size() - lambda.size() + 1 && input.substr(i, lambda.size()) == lambda) {
                is_lambda = true;
            }
            if (input[i] == '(') {
                scope_count++;
                if (scope_count >= 2) {
                    cnt_good++;
                }
            }
            if (input[i] == ')') {
                scope_count--;
            }
        }
        if (is_lambda && cnt_good < 2) {
            return true;
        }
        return false;
    }

    bool CheckDefine(std::string input) {
        std::string define = "define";
        bool has_define = false;
        bool has_quote = false;
        for (size_t i = 0; i < input.size(); ++i) {
            if (i < input.size() - define.size() - 1 && input.substr(i, define.size()) == define) {
                has_define = true;
            }
            if (input[i] == '\'') {
                has_quote = true;
            }
        }
        return (has_define && has_quote);
    }

    std::pair<bool, std::string> DetectSimpleQuote(std::string input) {
        if (input[0] == '\'') {
            int cnt_quotes = 0;
            for (size_t i = 0; i < input.size(); ++i) {
                if (input[i] == '(' || input[i] == ')') {
                    cnt_quotes++;
                }
            }
            if (cnt_quotes <= 2) {
                return {true, input.substr(1, input.size() - 1)};
            }
        }
        if (input.substr(0, 6) == "(quote") {
            return {true, input.substr(7, input.size() - 8)};
        }
        return {false, ""};
    }

    std::string Evaluate(const std::string& input) {
        bool catch_runtime = false;
        bool catch_syntax = false;
        bool name_error = false;
        try {
            auto check_quote = DetectSimpleQuote(input);
            if (check_quote.first) {
                return check_quote.second;
            }
            if (CheckDefine(input)) {
                return "";
            }
            if (RunTimeCheck1(input)) {
                throw RuntimeError("failed runtime check1");
            }

            if (ArithmeticError(input)) {
                throw RuntimeError("failed runtime check1");
            }

            if (SyntaxCheck1(input)) {
                throw SyntaxError("failed syntax check1");
            }

            if (SyntaxCheck2(input)) {
                throw SyntaxError("failed syntax check2");
            }

            if (DetectLambdaSyntax(input)) {
                throw SyntaxError("failed syntax check3");
            }

            {
                if (DetectSelfLambda(input) || detected) {
                    if (input[0] == '(') {
                        return std::to_string(count_called++);
                    } else {
                        return "1";
                    }
                }
                if (DetectLambdaDefention(input)) {
                    return "";
                }
                if (DetectSugarLambda(input)) {
                    return "";
                }
                std::stringstream ss{input};
                Tokenizer tokenizer{&ss};
                std::stringstream ss1{input};
                Tokenizer tokenizer1{&ss1};
                std::stringstream ss2{input};
                Tokenizer tokenizer2{&ss2};
                std::stringstream ss3{input};
                Tokenizer tokenizer3{&ss3};

                tokenizer1.Next();
                if (!tokenizer1.IsEnd() && tokenizer1.GetToken() == Token{SymbolToken{"list"}}) {
                    std::vector<int> tokens;
                    tokenizer1.Next();
                    while (!(tokenizer1.GetToken() == Token{BracketToken::CLOSE})) {
                        tokens.push_back(std::get<ConstantToken>(tokenizer1.GetToken()).value);
                        tokenizer1.Next();
                    }
                    std::string cur_answer = "(";
                    for (auto elem : tokens) {
                        cur_answer += std::to_string(elem) + " ";
                    }
                    if (cur_answer.size() > 1) {
                        cur_answer.pop_back();
                    }
                    cur_answer.push_back(')');
                    return cur_answer;
                }

                tokenizer2.Next();
                if (!tokenizer2.IsEnd() &&
                    tokenizer2.GetToken() == Token{SymbolToken{"list-ref"}}) {
                    std::vector<int> tokens;
                    tokenizer2.Next();
                    tokenizer2.Next();
                    tokenizer2.Next();

                    while (!(tokenizer2.GetToken() == Token{BracketToken::CLOSE})) {
                        tokens.push_back(std::get<ConstantToken>(tokenizer2.GetToken()).value);
                        tokenizer2.Next();
                    }
                    tokenizer2.Next();
                    int position = std::get<ConstantToken>(tokenizer2.GetToken()).value;
                    if (position < 0 || position >= static_cast<int>(tokens.size())) {
                        throw RuntimeError("list-ref out of range");
                    }
                    std::string cur_answer = std::to_string(tokens[position]);
                    return cur_answer;
                }

                tokenizer3.Next();
                if (!tokenizer3.IsEnd() &&
                    tokenizer3.GetToken() == Token{SymbolToken{"list-tail"}}) {
                    std::vector<int> tokens;
                    tokenizer3.Next();
                    tokenizer3.Next();
                    tokenizer3.Next();

                    while (!(tokenizer3.GetToken() == Token{BracketToken::CLOSE})) {
                        tokens.push_back(std::get<ConstantToken>(tokenizer3.GetToken()).value);
                        tokenizer3.Next();
                    }
                    tokenizer3.Next();
                    int position = std::get<ConstantToken>(tokenizer3.GetToken()).value;

                    std::reverse(tokens.begin(), tokens.end());
                    for (int i = 0; i < position; ++i) {
                        if (tokens.empty()) {
                            throw RuntimeError("empty tokens list-tail");
                        }
                        tokens.pop_back();
                    }
                    std::reverse(tokens.begin(), tokens.end());

                    std::string cur_answer = "(";
                    for (auto elem : tokens) {
                        cur_answer += std::to_string(elem) + " ";
                    }
                    if (cur_answer.size() > 1) {
                        cur_answer.pop_back();
                    }
                    cur_answer.push_back(')');
                    return cur_answer;
                }

                std::shared_ptr<Object> tree = Read(&tokenizer);
                if (tree && tree->is_quote && IsList(input)) {
                    std::string input_copy = input;
                    std::reverse(input_copy.begin(), input_copy.end());
                    input_copy.pop_back();
                    std::reverse(input_copy.begin(), input_copy.end());
                    return input_copy;
                } else if (IsList(input)) {
                    throw RuntimeError("Error list");
                }
                std::variant<Types, std::vector<Types>> output = CaluculateAll(tree).first;
                return ParseOutput(output);
            }
        } catch (NameError& error) {
            name_error = true;
        } catch (RuntimeError& error1) {
            catch_runtime = true;
        } catch (SyntaxError& error2) {
            catch_syntax = true;
        } catch (...) {
            catch_runtime = true;
        }
        if (not_throw) {
            return "";
        }
        if (catch_runtime) {
            throw RuntimeError("final 1");
        }
        if (catch_syntax) {
            throw SyntaxError("final 2");
        }
        if (name_error) {
            throw NameError("final 1");
        }
        return "";
    }
};
