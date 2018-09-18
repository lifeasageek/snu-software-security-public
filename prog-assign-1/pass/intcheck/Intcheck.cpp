#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/Module.h"
using namespace llvm;

namespace {
  struct IntcheckPass : public FunctionPass {
    static char ID;
    LLVMContext *C;
    Constant *logFunc;
    Type *VoidTy;
    Type *Int32Ty;

    IntcheckPass() : FunctionPass(ID) {}

    bool doInitialization(Module &M) {
      C = &(M.getContext());
      VoidTy = Type::getVoidTy(*C);
      Int32Ty = Type::getInt32Ty(*C);
      logFunc = M.getOrInsertFunction("logop", VoidTy, Int32Ty, NULL);
      return true;
    }

    bool shouldCheckOverflow(Value *I, int depth) {
      // TODO: implement simple dataflow analysis to see if the computed data is
      // flowing into malloc().
      return true;
    }

    Value* getLineNum(Instruction *I) {
      const DebugLoc *debugLoc = &I->getDebugLoc();

      if (debugLoc)
        return ConstantInt::get(Int32Ty, debugLoc->getLine());
      return ConstantInt::get(Int32Ty, -1);
    }

    virtual bool runOnFunction(Function &F) {
      bool res = false;

      for (auto &B : F) {
        for (auto &I : B) {
          if (auto *op = dyn_cast<BinaryOperator>(&I)) {
            // TODO: Implement the shouldCheckOverflow() function.
            if (!shouldCheckOverflow(&I, 0))
              continue;

            errs() << "Instrument: " << I << "\n";

            // Insert call instruction *after* `op`.
            // TODO: Pass more information including operands of computations.
            IRBuilder<> builder(op);
            builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
            Value* args[] = {op, getLineNum(&I)};
            builder.CreateCall(logFunc, args);
            res |= true;
          }
        }
      }
      return res;
    }
  };
}

char IntcheckPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerIntcheckPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new IntcheckPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerIntcheckPass);
