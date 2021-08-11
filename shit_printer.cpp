#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include <fstream>
#include <iostream>
#include <memory>

using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

constexpr char some_id[] = "some_id";
constexpr char public_things_of_given_class[] = "public_things_of_given_class";

std::string function_name{};
std::string class_name{};

constexpr char file_name[] = "sample.cpp";

class FunctionTemplateDeclCallback : public MatchFinder::MatchCallback
{
private:
    virtual void run(const MatchFinder::MatchResult &Result)
    {
        const FunctionTemplateDecl *functionTemplateDecl =
            Result.Nodes.getNodeAs<FunctionTemplateDecl>(some_id);
        if (functionTemplateDecl == nullptr)
            return;

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

class MethodsCallback : public MatchFinder::MatchCallback
{
private:
    virtual void run(const MatchFinder::MatchResult &Result)
    {
        const auto *cxxMethodDecl = Result.Nodes.getNodeAs<clang::CXXMethodDecl>(public_things_of_given_class);
        if (cxxMethodDecl == nullptr)
            return;

        auto parent = cxxMethodDecl->getParent();
        if (parent == nullptr || parent->getNameAsString() != class_name)
            return;

        std::cout << cxxMethodDecl->getNameAsString() << std::endl;
    }
};

class TypesCallback : public MatchFinder::MatchCallback
{
private:
    virtual void run(const MatchFinder::MatchResult &Result)
    {
        const auto *cxxRecordDecl = Result.Nodes.getNodeAs<clang::CXXRecordDecl>(public_things_of_given_class);
        if (cxxRecordDecl == nullptr)
            return;

        auto name = cxxRecordDecl->getNameAsString();
        if (name == class_name)
            return;

        auto parent = cxxRecordDecl->getParent();
        if (parent == nullptr)
            return;

        auto *parentTagDeclPtr = dyn_cast<const clang::TagDecl>(parent);
        if (parentTagDeclPtr == nullptr || parentTagDeclPtr->getNameAsString() != class_name)
            return;

        std::cout << name << std::endl;
    }
};

class TemplateClassCallback : public MatchFinder::MatchCallback
{
private:
    virtual void run(const MatchFinder::MatchResult &Result)
    {
        const auto *classTemplateDecl =
            Result.Nodes.getNodeAs<clang::ClassTemplateDecl>(public_things_of_given_class);
        if (classTemplateDecl == nullptr)
            return;

        auto name = classTemplateDecl->getNameAsString();
        if (name == class_name)
            return;

        auto parent = classTemplateDecl->getTemplatedDecl()->getParent();
        if (parent == nullptr)
            return;

        auto *parentTagDeclPtr = dyn_cast<const clang::TagDecl>(parent);
        if (parentTagDeclPtr == nullptr || parentTagDeclPtr->getNameAsString() != class_name)
            return;

        std::cout << name << std::endl;
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

    DeclarationMatcher publicMethodMatcher =
        clang::ast_matchers::cxxMethodDecl(isPublic(), unless(isImplicit())).bind(public_things_of_given_class);
    DeclarationMatcher publicTypeMatcher =
        clang::ast_matchers::cxxRecordDecl(isPublic()).bind(public_things_of_given_class);
    DeclarationMatcher publicTemplateTypeMatcher =
        clang::ast_matchers::classTemplateDecl(isPublic()).bind(public_things_of_given_class);

    FunctionTemplateDeclCallback functionTemplateDeclCallback;
    MethodsCallback printPublicMethodsCallback;
    TypesCallback printPublicTypesCallback;
    TemplateClassCallback printPublicTemplateClassCallback;

    MatchFinder finder;
    finder.addMatcher(functionTemplateDecl().bind(some_id), &functionTemplateDeclCallback);
    finder.addMatcher(publicMethodMatcher, &printPublicMethodsCallback);
    finder.addMatcher(publicTypeMatcher, &printPublicTypesCallback);
    finder.addMatcher(publicTemplateTypeMatcher, &printPublicTemplateClassCallback);

    std::unique_ptr<FrontendActionFactory> factory = { newFrontendActionFactory(&finder) };

    return clang::tooling::runToolOnCode(factory->create(), src);
}
