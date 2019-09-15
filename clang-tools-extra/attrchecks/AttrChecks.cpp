// https://clang.llvm.org/docs/LibASTMatchersTutorial.html
// https://clang.llvm.org/docs/RAVFrontendAction.html
//

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Attr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include <chrono>
#include <string>
#include <thread>
#include <vector>

using StringVector = std::vector<std::string>;
using ConstFnDeclVector = std::vector<const clang::FunctionDecl *>;
using DeclVector = std::vector<clang::Decl *>;

using namespace clang;

namespace
{

	class AttrChecks 
	{
	public:
		AttrChecks() {}

		void Initialize() {}
		void Finalize() {}

		static ConstFnDeclVector GetCallerFns(const CallExpr* CallExpr,
											  ASTContext* Context)
		{
			ConstFnDeclVector callers{};

			auto Parents = Context->getParents(*CallExpr);
			while (!Parents.empty()) 
			{
				const ast_type_traits::DynTypedNode& Parent = Parents[0];

				if (auto * fn = Parent.get<FunctionDecl>()) 
				{
					callers.push_back(fn);
					break;
				}

				else if (auto * S = Parent.get<Stmt>())
					Parents = Context->getParents(*S);
			}

			return callers;
		}

		static StringVector GetCustomAttrParams(const Decl* decl) 
		{
			StringVector attrParams{};
			const AttrVec& attrs = decl->getAttrs();
			if (attrs.empty())
				return attrParams;

			for (Attr* attr : attrs)
			{
				const MyCustomAttr* custAttr = dyn_cast<MyCustomAttr>(attr);
				if (custAttr != nullptr)
					GetAttributeParameters(custAttr, attrParams);
			}

			return attrParams;
		}

		static bool HasCustomAttrParams(const CXXMethodDecl* methodDecl)
		{
			return !GetCustomAttrParams(methodDecl).empty();
		}

		static void GetAttributeParameters(const MyCustomAttr* custAttr,
										   StringVector& params) 
		{
			for (const auto& Val : custAttr->attributeParameters())
				params.push_back(Val);
		}

		static bool IsFnTaggedSafe(const FunctionDecl* fn) 
		{
			StringVector values = GetCustomAttrParams(fn);
			return values.size() > 0 && values[0] == "Safe";
		}

		static bool IsFnTaggedUnSafe(const FunctionDecl* fn) 
		{
			StringVector values = GetCustomAttrParams(fn);
			return values.size() > 0 && values[0] == "Unsafe";
		}


		static bool IsMethodInParentTagged(const CXXMethodDecl* md, const ASTContext* Context)
		{
			for (auto md : Context->overridden_methods(md))
			{
				if (HasCustomAttrParams(md))
					return true;
			}

			return false;
		}


		static bool DebugLogging() { return false; }
	};

	class FindNamedClassVisitor
		: public RecursiveASTVisitor<FindNamedClassVisitor> 
	{
	public:
		explicit FindNamedClassVisitor(ASTContext* Context) : Context(Context) 
		{}

		bool VisitCXXRecordDecl(CXXRecordDecl* Declaration)
		{
			if(AttrChecks::DebugLogging())
			{
				using namespace clang;

				FullSourceLoc FullLocation =
					Context->getFullLoc(Declaration->getBeginLoc());
				if (FullLocation.isValid()) 
				{
					llvm::outs() << "Found declaration { " << Declaration->getNameAsString()
						<< " } at " << FullLocation.getSpellingLineNumber() << ":"
						<< FullLocation.getSpellingColumnNumber() << "\n";

					StringVector attrParams = AttrChecks::GetCustomAttrParams(Declaration);

					if (!attrParams.empty()) 
					{
						llvm::outs() << " Attribute Parameters are:\n";
						for (std::string param : attrParams)
							llvm::outs() << "\t " << param << "\n";
					}
				}
			}
			return true;
		}

		bool VisitFunctionDecl(FunctionDecl* fnDecl) 
		{
			if (AttrChecks::DebugLogging())
			{
				llvm::outs() << "Fn declaration: " << fnDecl->getQualifiedNameAsString() << " \n";

				StringVector attrParams = AttrChecks::GetCustomAttrParams(fnDecl);

				if (!attrParams.empty())
				{
					llvm::outs() << " Attribute Parameters are:\n";
					for (std::string param : attrParams)
						llvm::outs() << "\t " << param << "\n";
				}
			}

			return true;
		}

		bool VisitCXXMethodDecl(CXXMethodDecl* methodDecl)
		{
			if (AttrChecks::DebugLogging())
			{
				if (methodDecl->isPure())
				{
					llvm::outs() << "Method: " << methodDecl->getQualifiedNameAsString() << " is PureVirtual\n";
				}
				else if (methodDecl->isVirtual())
				{
					llvm::outs() << "Method: " << methodDecl->getQualifiedNameAsString() << " is virtual\n";

					for (auto md : Context->overridden_methods(methodDecl))
						llvm::outs() << "OverriddenMethod: " << md->getQualifiedNameAsString() << "\n";
				}
			}

			if (methodDecl->isVirtual())
			{
				if (!AttrChecks::HasCustomAttrParams(methodDecl) && AttrChecks::IsMethodInParentTagged(methodDecl, Context))
				{
					auto& DE = Context->getDiagnostics();
					const unsigned ID = DE.getCustomDiagID(clang::DiagnosticsEngine::Error,
						"Attribute Checker Warning: fn: '%0' is not tagged");

					auto daigBuilder = DE.Report(methodDecl->getSourceRange().getBegin(), ID);
					daigBuilder.AddString(methodDecl->getQualifiedNameAsString());
				}
			}

			return true;
		}

		bool VisitCallExpr(CallExpr* callExpr) 
		{
			auto calleeFn = callExpr->getDirectCallee();
			ConstFnDeclVector callers = AttrChecks::GetCallerFns(callExpr, Context);

			bool isCalleeFnSafe = AttrChecks::IsFnTaggedSafe(calleeFn);
			bool isCallerFnSafe = AttrChecks::IsFnTaggedSafe(callers[0]);

			if (isCallerFnSafe && !isCalleeFnSafe)
			{
				auto& DE = Context->getDiagnostics();
				const unsigned ID = DE.getCustomDiagID(clang::DiagnosticsEngine::Error, 
								"Attribute Checker Failure: Unsafe fn: '%0' called from Safe fn: '%1'");

				auto daigBuilder = DE.Report(callExpr->getExprLoc(), ID);
				daigBuilder.AddString(calleeFn->getQualifiedNameAsString());
				daigBuilder.AddString(callers[0]->getQualifiedNameAsString());
			}

			return true;
		}

	private:
		ASTContext* Context{ nullptr };
	};

	class FindNamedClassConsumer : public clang::ASTConsumer
	{
	public:
		explicit FindNamedClassConsumer(ASTContext* Context) : Visitor(Context) 
		{}

		virtual void HandleTranslationUnit(class ASTContext& Context)
		{
			Visitor.TraverseDecl(Context.getTranslationUnitDecl());
		}

	private:
		FindNamedClassVisitor Visitor;
	};

	class FindNamedClassAction : public clang::ASTFrontendAction
	{
	public:
		virtual std::unique_ptr<class ::ASTConsumer>
		CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile)
		{
			return std::unique_ptr<class ::ASTConsumer>(
				new FindNamedClassConsumer(&Compiler.getASTContext()));
		}
	};

} // namespace

int main(int argc, const char **argv) 
{
  using namespace clang::tooling;

  AttrChecks Checker{};
  Checker.Initialize();

  CommonOptionsParser op(argc, argv, llvm::cl::GeneralCategory,
                         llvm::cl::ZeroOrMore,
                         "Clang-based checking tool for attributes check");

  llvm::outs() << "\n\n";

  std::this_thread::sleep_for(std::chrono::seconds(5));

  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  auto frontAction = newFrontendActionFactory<FindNamedClassAction>();
  int result = Tool.run(frontAction.get());

  Checker.Finalize();

  return 0;
}
