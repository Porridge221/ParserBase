//
// Created by yuri on 10.11.16.
//

#include "AstNode.h"


AstNode::AstNode(int type, std::string token, AstNode *ch1, AstNode *ch2) {
    Type = type;
    Token = token;
    if (ch1 != nullptr)
        this->Childs.push_back(ch1);
    if (ch2 != nullptr)
        this->Childs.push_back(ch2);
}

AstNode::AstNode(int type, std::string token, AstNode *ch1) {
    AstNode(type, token, ch1, nullptr);
}

AstNode::AstNode(int type, std::string token) {
    AstNode(type, token, nullptr, nullptr);
}

AstNode::AstNode(int type) {
    AstNode(type, "");
}

AstNode::AstNode() {

}

void AstNode::addChild(AstNode *ch1) {
    this->Childs.push_back(ch1);
}

AstNode *AstNode::getLeftNode() {
    if (this->Childs.size() && this->Childs[0] != nullptr)
        return this->Childs[0];
    return nullptr;
}

AstNode *AstNode::getRightNode() {
    if (this->Childs.size() == 2 && this->Childs[1] != nullptr)
        return this->Childs[1];
    return nullptr;
}

int AstNode::getType() {
    return this->Type;
}

std::string AstNode::getToken() {
    return this->Token;
}

size_t AstNode::getChildsCount() {
    return this->Childs.size();
}

/*std::string AstNode::AstNodeTypeToString(int type) {
    switch (type) {
        case UNKNOWN:	return "?";
        case NUMBER:	return "NUM";
        case IDENT:		return "ID";
        case UNARY:     return "UnaryOp";
        case BINARY:    return "BinaryOp";
        case FUNCTION:  return "Function";
        case ADD:		return "+";
        case SUB:		return "-";
        case MUL:		return "*";
        case DIV:		return "/";
        case ASSIGN:	return "=";
        case BLOCK:		return "..";
        default:		return "";
    }
}*/
