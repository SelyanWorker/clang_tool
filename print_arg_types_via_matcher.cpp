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

constexpr char id_to_bind[] = { "dick" };

std::string function_name{ };

class DumpCallback : public MatchFinder::MatchCallback
{
public:
    using callable_type = void(const std::string&);

    template<typename CallableT>
    void setCallable(CallableT&& callable)
    {
        m_callback = std::forward<CallableT>(callable);
    }

private:
    virtual void run(const MatchFinder::MatchResult &Result)
    {
        const FunctionTemplateDecl *functionTemplateDecl =
            Result.Nodes.getNodeAs<FunctionTemplateDecl>(id_to_bind);

        if (function_name != functionTemplateDecl->getIdentifier()->getName().str())
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
                m_callback(arg.getAsType().getAsString());
            }
        }
    }

private:
    std::function<callable_type> m_callback;
};

int main(int argc, char **argv)
{
    if (argc < 3)
        return -1;

    std::ifstream f{ argv[1] };

    if (!f.is_open())
        return -1;

    std::string src;

    f.seekg(0, std::ios::end);
    src.reserve(f.tellg());
    f.seekg(0, std::ios::beg);

    src.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    function_name = argv[2];

    std::vector<std::string> types;

    DumpCallback callback;
    callback.setCallable([&types](const std::string &type) { types.push_back(type); });

    MatchFinder finder;
    finder.addMatcher(functionTemplateDecl().bind(id_to_bind), &callback);

    std::unique_ptr<FrontendActionFactory> factory = { newFrontendActionFactory(&finder) };

    bool runToolOnCodeResult = clang::tooling::runToolOnCode(factory->create(), src);

    if (runToolOnCodeResult)
        for(auto type : types)
            std::cout << type << std::endl;

    return runToolOnCodeResult;
}
