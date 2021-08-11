#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/Tooling.h"
#include <fstream>
#include <iostream>
#include <memory>

using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

constexpr char id_to_bind[] = "dick";

std::string function_name{};
std::string class_name{};

constexpr char file_name[] = "sample.cpp";

class DumpCallback : public MatchFinder::MatchCallback
{
private:
    virtual void run(const MatchFinder::MatchResult &Result)
    {
        const FunctionTemplateDecl *functionTemplateDecl =
            Result.Nodes.getNodeAs<FunctionTemplateDecl>(id_to_bind);

        auto identifierPtr = functionTemplateDecl->getIdentifier();
        if (identifierPtr == nullptr)
            return;

        if (function_name != identifierPtr->getName().str())
            return;

        auto arrayOfSpecializations = functionTemplateDecl->specializations();
        for (auto specialization : arrayOfSpecializations)
        {
            auto specializationArgs = specialization->getTemplateSpecializationArgs();
            if (specializationArgs == nullptr)
                continue;

            auto specializationArgsArray = specializationArgs->asArray();
            for (auto arg : specializationArgsArray)
            {
                std::cout << "\"" << arg.getAsType().getAsString() << "\""
                          << " ";
            }
            std::cout << std::endl;
        }
    }
};

class PrintPublicShitMethodsCallback : public MatchFinder::MatchCallback
{
private:
    virtual void run(const MatchFinder::MatchResult &Result)
    {
        if (const CXXMethodDecl *md = Result.Nodes.getNodeAs<clang::CXXMethodDecl>("publicShit"))
        {
            if (md->getParent() == nullptr || md->getParent()->getNameAsString() != class_name)
                return;

            std::cout << md->getNameAsString() << std::endl;
        }
    }
};

class PrintPublicShitTypesCallback : public MatchFinder::MatchCallback
{
private:
    virtual void run(const MatchFinder::MatchResult &Result)
    {
        if (const CXXRecordDecl *rd = Result.Nodes.getNodeAs<clang::CXXRecordDecl>("publicShit"))
        {
            if (rd->getParent() == nullptr || rd->getNameAsString() == class_name)
                return;

            if (auto *tagDeclPtr = dyn_cast<const clang::TagDecl>(rd->getParent()))
            {
                if (tagDeclPtr->getNameAsString() == class_name)
                    std::cout << rd->getNameAsString() << std::endl;
            }
        }
    }
};

class PrintPublicTemplateClassCallback : public MatchFinder::MatchCallback
    {
    private:
        virtual void run(const MatchFinder::MatchResult &Result)
        {
            if (const ClassTemplateDecl *td = Result.Nodes.getNodeAs<clang::ClassTemplateDecl>("publicShit"))
            {
                if (td->getTemplatedDecl()->getParent() == nullptr ||
                    td->getNameAsString() == class_name)
                    return;

                std::cout << td->getNameAsString() << std::endl;
            }
        }
    };

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Few arguments" << std::endl;
        return -1;
    }

    std::ifstream f{ file_name };

    if (!f.is_open())
        return -1;

    std::string src;

    f.seekg(0, std::ios::end);
    src.reserve(f.tellg());
    f.seekg(0, std::ios::beg);

    src.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    function_name = argv[1];
    class_name = argv[2];

    std::vector<std::string> types;

    DeclarationMatcher publicMethodMatcher =
        clang::ast_matchers::cxxMethodDecl(isPublic(), unless(isImplicit())).bind("publicShit");

    DeclarationMatcher publicTypeMatcher =
        clang::ast_matchers::cxxRecordDecl(isPublic()).bind("publicShit");

    DeclarationMatcher publicTemplateTypeMatcher =
        clang::ast_matchers::classTemplateDecl(isPublic()).bind("publicShit");

    TypeMatcher typeMatcher = clang::ast_matchers::type();

    DumpCallback callback;
    PrintPublicShitMethodsCallback printPublicShitMethodsCallback;
    PrintPublicShitTypesCallback printPublicShitTypesCallback;
    PrintPublicTemplateClassCallback printPublicTemplateClassCallback;


    MatchFinder finder;
    finder.addMatcher(functionTemplateDecl().bind(id_to_bind), &callback);
    finder.addMatcher(publicMethodMatcher, &printPublicShitMethodsCallback);
    finder.addMatcher(publicTypeMatcher, &printPublicShitTypesCallback);
    finder.addMatcher(publicTemplateTypeMatcher, &printPublicTemplateClassCallback);

    std::unique_ptr<FrontendActionFactory> factory = { newFrontendActionFactory(&finder) };

    return clang::tooling::runToolOnCode(factory->create(), src);
}
