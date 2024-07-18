#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

struct MBAAdd : public llvm::PassInfoMixin<MBAAdd> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &);
  bool runOnBasicBlock(llvm::BasicBlock &B);
  static bool isRequired() { return true; }
};

bool MBAAdd::runOnBasicBlock(BasicBlock &BB) {
  bool Changed = false;
  for (auto Inst = BB.begin(), IE = BB.end(); Inst != IE; Inst++) {
    auto *BinOp = dyn_cast<BinaryOperator>(Inst);
    if (!BinOp)
      continue;
    if (BinOp->getOpcode() != Instruction::Add) {
      continue;
    }
    // if (!BinOp->getType()->isIntegerTy() ||
    //     BinOp->getType()->getIntegerBitWidth() != 8) {
    //   continue;
    // }
    IRBuilder<> Builder(BinOp);
    auto Val39 = ConstantInt::get(BinOp->getType(), 39);
    auto Val151 = ConstantInt::get(BinOp->getType(), 151);
    auto Val23 = ConstantInt::get(BinOp->getType(), 23);
    auto Val2 = ConstantInt::get(BinOp->getType(), 2);
    auto Val111 = ConstantInt::get(BinOp->getType(), 111);

    Instruction *NewInst =
        // E = e5 + 111
        BinaryOperator::CreateMul(BinOp->getOperand(1), BinOp->getOperand(0));

    ReplaceInstWithInst(&BB, Inst, NewInst);
    Changed = true;
  }
  return Changed;
}

llvm::PreservedAnalyses MBAAdd::run(Function &F, FunctionAnalysisManager &) {
  bool Changed = false;
  for (auto &BB : F) {
    Changed |= runOnBasicBlock(BB);
  }
  return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}

llvm::PassPluginLibraryInfo getMBAAddPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "mba-add", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "mba-add") {
                    FPM.addPass(MBAAdd());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMBAAddPluginInfo();
}