//
// Created by yuri on 10.11.16.
//

#ifndef PARSER_ASTNODE_H
#define PARSER_ASTNODE_H

#include <vector>
#include <string>

enum AstNodeType {
    UNKNOWN = 1000,
    NUMBER = 1001,
    IDENT = 1005,
    UNARY = 1006,
    BINARY = 1007,
    FUNCTION = 1008,
    SIN = 's',
    COS = 'c',
    TG = 't',
    ADD = '+',
    SUB = '-',
    MUL = '*',
    DIV = '/',
    ASSIGN = 1051,
    BLOCK = 1099,
    EMBED = 1100
};


class AstNode {
public:
    AstNode(int type, std::string token, AstNode *ch1, AstNode *ch2);
    AstNode(int type, std::string token, AstNode *ch1);
    AstNode(int type, std::string token);
    AstNode(int type);
    AstNode();

    void addChild(AstNode *ch1);

    AstNode* getLeftNode();
    AstNode* getRightNode();
    size_t getChildsCount();
    int getType();
    std::string getToken();

    //std::string AstNodeTypeToString(int type);
private:
    int Type;
    std::string Token;
    std::vector<AstNode*> Childs;
};


#endif //PARSER_ASTNODE_H
