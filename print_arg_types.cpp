#include <fstream>
#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

std::string function_name{ "" };

constexpr char file_name[] = "sample.cpp";

class PrintArgTypesVisitor : public clang::RecursiveASTVisitor<PrintArgTypesVisitor>
{
public:
    explicit PrintArgTypesVisitor(clang::ASTContext *Context) : Context(Context) {}

    bool VisitDeclRefExpr(clang::DeclRefExpr *declRefExpr)
    {
        auto fun_name = declRefExpr->getNameInfo().getAsString();

        if (fun_name != function_name)
            return true;

        std::cout << fun_name << ": ";

        auto paramTypes =
            declRefExpr->getType()->getAs<clang::FunctionProtoType>()->getParamTypes();
        clang::PrintingPolicy pp(Context->getLangOpts());
        for (auto t : paramTypes)
            std::cout << t.getAsString(pp) << " ";

        std::cout << std::endl;

        return true;
    }

private:
    clang::ASTContext *Context;
};

class PrintArgTypesConsumer : public clang::ASTConsumer
{
public:
    explicit PrintArgTypesConsumer(clang::ASTContext *Context) : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context)
    {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    PrintArgTypesVisitor Visitor;
};

class PrintArgTypesAction : public clang::ASTFrontendAction
{
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler,
                                                                  llvm::StringRef InFile)
    {
        return std::make_unique<PrintArgTypesConsumer>(&Compiler.getASTContext());
    }
};

int main(int argc, char **argv)
{
    if (argc < 2)
        return -1;

    std::ifstream f{ file_name };

    if (!f.is_open())
        return -1;

    std::string src;

    f.seekg(0, std::ios::end);
    src.reserve(f.tellg());
    f.seekg(0, std::ios::beg);

    src.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    function_name = argv[2];

    clang::tooling::runToolOnCode(std::make_unique<PrintArgTypesAction>(), src.c_str());
}