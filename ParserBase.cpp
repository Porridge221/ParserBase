//
// Created by yuri on 10.11.16.
//

#include <iostream>
#include <cstring>
#include <cmath>
#include "ParserBase.h"


ParserBase::ParserBase() : pos(0), state(Expr), priority(0), parent(nullptr) {
    varTable.clear();
    while (!operands.empty())
        operands.pop();
    while (!operations.empty())
        operations.pop();
}

ParserBase::ParserBase(std::string input) : source(input) {
    ParserBase();
    this->Parse();
}

void ParserBase::setExpression(std::string input) {
    ParserBase();
    this->source = input;
    this->Parse();
}

void ParserBase::Parse() {
    try {
        while (!checkEnd()) {
            Signals signal = getSignal(source);
            Cell cell = FSM_table[state][signal];
            (this->*cell.worker)(cell.state);
        }

        displace();

        this->parent = operands.top();
    }
    catch (ParserBaseException &e) {
        std::cout << e.getException() << std::endl;
    }
}

void ParserBase::displace() {

    while (!operations.empty() && operations.top()->getType() != EMBED) {
        AstNode *right_expr = operands.top();
        operands.pop();
        AstNode *left_expr = operands.top();
        operands.pop();
        AstNode *op = operations.top();
        op->addChild(left_expr);
        op->addChild(right_expr);
        operations.pop();
        operands.push(op);
    }
    while (!operations.empty() && operations.top()->getType() == EMBED) {
        operations.pop();

        AstNode *tmp = operands.top();
        operands.pop();
        if (operands.top()->getType() == UNARY) {
            operands.push(tmp);
            operands.top()->addChild(tmp);
        }
        else
            operands.top()->addChild(tmp);
    }

}

std::string ParserBase::getSource() {
    return source;
}

int ParserBase::getPos() {
    return pos;
}

char ParserBase::getSymbol() {
    return pos < source.length() ? source[pos] : (char)0;
}

bool ParserBase::checkEnd() {
    return getSymbol() == 0;
}

void ParserBase::Next() {
    if (!checkEnd())
        pos++;
}

void ParserBase::Assign(std::string id, double value) {
    for (auto& t : varTable) {
        if (id == t.id) {
            t.value = value;
            t.exist = true;
            return;
        }
    }
    std::cout << "Переменной с данным именем не существует" << std::endl;
}

Signals ParserBase::getSignal(std::string input) {
    if (DEFAULT_WHITESPACES.find(input[pos]) != std::string::npos)
        return Space;
    if (isdigit(input[pos]))
        return Digit;
    if (DEFAULT_BINARYOPERATIONS.find(input[pos]) != std::string::npos)
        return BinOp;
    if (input[pos] == '-')
        return UnOp;
    if (input[pos] == '(')
        return BktL;
    if (input[pos] == ')')
        return BktR;
    if (DEFAULT_ENDL.find(input[pos]) != std::string::npos)
        return Endl;
    if (isalpha(input[pos])) {
        for (auto& t : DEFAULT_FUNCTIONS) {
            if (std::strncmp(&input[pos] , t.c_str(), t.size()) == 0
                && !isalpha(*(&input[pos] + t.size())) && !isdigit(*(&input[pos] + t.size()))) {
                return Func;
            }
        }
        return ID;
    }

    return Unknown;
}

int ParserBase::get_priority(const std::string& token) {
    if (token == "+") return 1;
    if (token == "-") return 1;
    if (token == "*") return 2;
    if (token == "/") return 2;
    //if (token == "mod") return 2; // остаток от деления
    //if (token == "**") return 3; // степень
    return 0; // Возвращаем 0 если токен - это не бинарная операция (например ")")
}


void ParserBase::ReadSpaces(States state) {
    this->state = state;
    Next();
}

void ParserBase::ReadNumber(States state) {
    this->state = state;
    std::string number;
    while (isdigit(source[pos])) {
        number.push_back(source[pos]);
        Next();
    }
    AstNode *tmp = new AstNode(NUMBER, number, nullptr, nullptr);  // TYPE_E
    operands.push(tmp);
}

void ParserBase::ReadBinaryOp(States state) {
    this->state = state;
    std::string binary;
    binary.push_back(source[pos]);
    Next();

    AstNode *tmp = new AstNode(binary[0], binary, nullptr, nullptr);    // TYPE_E
    int priority = get_priority(binary);
    if (priority < this->priority) {
        displace();
    }
    this->priority = priority;
    operations.push(tmp);
}

void ParserBase::ReadUnaryOp(States state) {
    this->state = state;
    std::string unary;
    unary.push_back(source[pos]);
    Next();

    /*AstNode *embed = new AstNode(EMBED, "", nullptr, nullptr);
    operations.push(embed);
    AstNode *tmp = new AstNode(UNARY, unary, nullptr, nullptr);
    operands.push(tmp);*/

    Signals signal = getSignal(source);             // <<--  BAD
    Cell cell = FSM_table[state][signal];
    (this->*cell.worker)(cell.state);

    AstNode *num = operands.top();
    operands.pop();
    AstNode *tmp = new AstNode(UNARY, unary, nullptr, nullptr);
    tmp->addChild(num);
    operands.push(tmp);
}

void ParserBase::ReadFunction(States state) {
    this->state = state;
    std::string function;
    while (source[pos] != ' ' && source[pos] != '(') {
        function.push_back(source[pos]);
        Next();
    }
    AstNode *embed = new AstNode(EMBED, "", nullptr, nullptr);
    operations.push(embed);
    AstNode *tmp = new AstNode(function[0], "", nullptr, nullptr);
    operands.push(tmp);
}

void ParserBase::ReadLeftBkt(States state) {
    this->state = state;
    Next();
    if (operations.empty() || operations.top()->getType() != EMBED) {
        AstNode *embed = new AstNode(EMBED, "", nullptr, nullptr);
        operations.push(embed);
        AstNode *tmp = new AstNode(BLOCK, "", nullptr, nullptr);
        operands.push(tmp);
    }
    this->priority = 0;
}

void ParserBase::ReadRightBkt(States state) {
    this->state = state;
    Next();
    displace();
    this->priority = operations.empty() ? 0 : get_priority(operations.top()->getToken());
}

void ParserBase::ReadID(States state) {
    this->state = state;
    std::string id;
    id.push_back(source[pos]);
    Next();
    while (isalpha(source[pos]) || isdigit(source[pos])) {
        id.push_back(source[pos]);
        Next();
    }
    AstNode *tmp = new AstNode(IDENT, id, nullptr, nullptr);  // TYPE_E
    varTable.push_back(varRecord(id));
    operands.push(tmp);

}

void ParserBase::RP(States state) {

}

void ParserBase::HandleError(States state) {
    this->state = state;
    throw ParserBaseException("Неверный синтаксис выражения");
}

void ParserBase::Execute() {
    try {
        if (this->parent == nullptr)
            throw ParserBaseException("Выражение не было разобрано");
        std::cout << "result = " << ExecuteNode(parent) << std::endl;
    }
    catch (ParserBaseException &e) {
        std::cout << e.getException() << std::endl;
    }

}

double ParserBase::ExecuteNode(AstNode *node) {
    switch (node->getType()) {
        case UNKNOWN:
            throw ParserBaseException("Неизвестный токен/операция");
        case NUMBER:
            return std::stod(node->getToken());
        case ADD:
            return ExecuteNode(node->getLeftNode()) + ExecuteNode(node->getRightNode());
        case SUB:
            return ExecuteNode(node->getLeftNode()) - ExecuteNode(node->getRightNode());
        case MUL:
            return ExecuteNode(node->getLeftNode()) * ExecuteNode(node->getRightNode());
        case DIV:
            if (fabs(ExecuteNode(node->getRightNode())) < eps)
                throw ParserBaseException("Деление на ноль");
            return ExecuteNode(node->getLeftNode()) / ExecuteNode(node->getRightNode());
        case BLOCK:
            return ExecuteNode(node->getLeftNode());
        case UNARY:
            return -ExecuteNode(node->getLeftNode());
        case SIN:
            return sin(ExecuteNode(node->getLeftNode()));
        case COS:
            return cos(ExecuteNode(node->getLeftNode()));
        case TG:
            return tan(ExecuteNode(node->getLeftNode()));
        case IDENT:
            for (auto& t : varTable) {
                if (node->getToken() == t.id) {
                    if (t.exist)
                        return t.value;
                    else
                        throw ParserBaseException("Значение {0} не определено");
                }
            }
    }
}
