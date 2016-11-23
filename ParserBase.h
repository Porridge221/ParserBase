//
// Created by yuri on 10.11.16.
//

#ifndef PARSER_PARSERBASE_H
#define PARSER_PARSERBASE_H

#include "AstNode.h"
#include <stack>

enum States {
    Expr = 0,
    Operation,
    Operand,
    Success,
    Error
};

enum Signals {
    Space = 0,
    Digit,
    BinOp,
    UnOp,
    Func,
    BktL,
    BktR,
    Endl,
    ID,
    Unknown
};

const double eps = 10e-7;

struct varRecord {
    varRecord(std::string id) : id(id) {};
    std::string id;
    double value;
    bool exist = false;
};

class ParserBase;

typedef void (ParserBase::* transition_callback)(States state);

struct Cell {
    States state;
    transition_callback worker;
};


class ParserBase {
public:
    ParserBase();
    ParserBase(std::string input);

    void setExpression(std::string input);
    std::string getSource();

    void Assign(std::string id, double value);
    void Execute();

private:
    std::string source;
    int pos = 0;
    States state = Expr;
    int priority = 0;
    AstNode* parent = nullptr;
    std::vector<varRecord> varTable;

    std::stack<AstNode*> operands;
    std::stack<AstNode*> operations;

    const std::string DEFAULT_WHITESPACES = " \r\t";
    const std::string DEFAULT_BINARYOPERATIONS = "+*/";
    const std::string DEFAULT_UNARYOPERATIONS[1] = { "-" };
    const std::string DEFAULT_ENDL = "\n\0";
    const std::string DEFAULT_FUNCTIONS[3] = { "sin", "cos", "tg" };

    void ReadSpaces(States state);
    void ReadNumber(States state);
    void ReadBinaryOp(States state);
    void ReadUnaryOp(States state);
    void ReadFunction(States state);
    void ReadLeftBkt(States state);
    void ReadRightBkt(States state);
    void ReadID(States state);
    void RP(States state);
    void HandleError(States state);

    int getPos();
    char getSymbol();
    bool checkEnd();
    void Next();

    void Parse();
    void displace();
    Signals getSignal(std::string input);
    int get_priority(const std::string& token);
    double ExecuteNode(AstNode *node);

    Cell FSM_table[3][9] =  {
            { { Expr, &ParserBase::ReadSpaces },{ Operation, &ParserBase::ReadNumber },{ Error, &ParserBase::HandleError },
                    { Operand, &ParserBase::ReadUnaryOp }, { /*Operation*/Expr, &ParserBase::ReadFunction }, { Expr, &ParserBase::ReadLeftBkt },{ Operation, &ParserBase::ReadRightBkt },
                    { Error, &ParserBase::HandleError }, { Operation, &ParserBase::ReadID } },

            { { Operation, &ParserBase::ReadSpaces },{ Error, &ParserBase::HandleError },{ Operand, &ParserBase::ReadBinaryOp },
                    { Operand, &ParserBase::ReadBinaryOp }, { Error, &ParserBase::HandleError }, { Error, &ParserBase::HandleError },{ Operation, &ParserBase::ReadRightBkt },
                    { Success, &ParserBase::RP }, { Error, &ParserBase::HandleError } },

            { { Operand, &ParserBase::ReadSpaces },{ Operation, &ParserBase::ReadNumber },{ Error, &ParserBase::HandleError },
                    { Error, &ParserBase::HandleError }, { /*Operation*/Expr, &ParserBase::ReadFunction }, { Expr, &ParserBase::ReadLeftBkt },{ Error, &ParserBase::HandleError },
                    { Error, &ParserBase::HandleError }, { Operation, &ParserBase::ReadID } }
    };

    class ParserBaseException {
    public:
        ParserBaseException(std::string exception) : exception(exception) {};

        std::string getException() { return this->exception; }
    private:
        std::string exception;

    };
};

#endif //PARSER_PARSERBASE_H
