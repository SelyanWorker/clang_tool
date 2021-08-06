#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/Tooling.h"
#include <fstream>
#include <memory>
#include <iostream>

using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

constexpr char id_to_bind[] = "dick";

std::string function_name{ };

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
        for(auto specialization : arrayOfSpecializations)
        {
            auto specializationArgs = specialization->getTemplateSpecializationArgs();
            if (specializationArgs == nullptr)
                continue;

            auto specializationArgsArray = specializationArgs->asArray();
            for(auto arg : specializationArgsArray)
            {
                std::cout << "\"" << arg.getAsType().getAsString() << "\"" << " ";
            }
            std::cout << std::endl;
        }
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

    function_name = argv[1];

    std::vector<std::string> types;

    DumpCallback callback;

    MatchFinder finder;
    finder.addMatcher(functionTemplateDecl().bind(id_to_bind), &callback);

    std::unique_ptr<FrontendActionFactory> factory = { newFrontendActionFactory(&finder) };

    return clang::tooling::runToolOnCode(factory->create(), src);
}
