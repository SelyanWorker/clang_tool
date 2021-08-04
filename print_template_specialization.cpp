#include <fstream>
#include <iostream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

class PrintTemplateSpecializationClassVisitor :
  public clang::RecursiveASTVisitor<PrintTemplateSpecializationClassVisitor>
{
public:
    explicit PrintTemplateSpecializationClassVisitor(clang::ASTContext *Context) : Context(Context)
    {
    }

    bool VisitTemplateSpecializationType(
        clang::TemplateSpecializationType *templateSpecializationType)
    {
        for (auto templateSpec : *templateSpecializationType)
        {
            clang::PrintingPolicy pp(Context->getLangOpts());
            std::string typeName = templateSpec.getAsType().getAsString(pp);

            std::cout << typeName << " ";
        }

        std::cout << std::endl;

        return true;
    }

private:
    clang::ASTContext *Context;
};

class PrintTemplateSpecializationConsumer : public clang::ASTConsumer
{
public:
    explicit PrintTemplateSpecializationConsumer(clang::ASTContext *Context) : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context)
    {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    PrintTemplateSpecializationClassVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction
{
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler,
                                                                  llvm::StringRef InFile)
    {
        return std::make_unique<PrintTemplateSpecializationConsumer>(&Compiler.getASTContext());
    }
};

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        std::ifstream f{ argv[1] };

        if (!f.is_open())
            return -1;

        std::string src;

        f.seekg(0, std::ios::end);
        src.reserve(f.tellg());
        f.seekg(0, std::ios::beg);

        src.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

        clang::tooling::runToolOnCode(std::make_unique<FindNamedClassAction>(), src.c_str());
    }
}