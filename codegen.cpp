//===----------------------------------------------------------------------===//
//
// Philipp Schubert
//
//    Copyright (c) 2021
//    GaZAR UG (haftungsbeschränkt)
//    Bielefeld, Germany
//    philipp@gazar.eu
//
//===----------------------------------------------------------------------===//

#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

namespace {

void addMainFunction(llvm::Module &M) {
  llvm::LLVMContext &Ctx = M.getContext();
  llvm::FunctionType *FunTy =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(Ctx), false);
  llvm::Function *Fun =
      llvm::Function::Create(FunTy, llvm::Function::ExternalLinkage, "main", M);
  llvm::BasicBlock *BB = llvm::BasicBlock::Create(Ctx, "entry", Fun);
  llvm::IRBuilder<> Builder(Ctx);
  Builder.SetInsertPoint(BB);
  auto *VarA = Builder.CreateAlloca(Builder.getInt32Ty());
  auto *VarB = Builder.CreateAlloca(Builder.getInt32Ty());
  auto *VarRes = Builder.CreateAlloca(Builder.getInt32Ty());
  Builder.CreateStore(Builder.getInt32(42), VarA);
  Builder.CreateStore(Builder.getInt32(13), VarB);
  auto *LoadA = Builder.CreateLoad(Builder.getInt32Ty(), VarA);
  auto *LoadB = Builder.CreateLoad(Builder.getInt32Ty(), VarB);
  auto *AddInst =
      Builder.CreateBinOp(llvm::Instruction::BinaryOps::Add, LoadA, LoadB);
  Builder.CreateStore(AddInst, VarRes);
  auto *LoadVarRes = Builder.CreateLoad(Builder.getInt32Ty(), VarRes);
  Builder.CreateRet(LoadVarRes);
}

} // end anonymous namespace

// We could, of course, also compile the above code into a shared object library
// that we can then use as a plugin for LLVM's optimizer 'opt'. But instead,
// here we are going the full do-it-yourself route and set up everything
// ourselves.
int main(int Argc, char **Argv) {
  using namespace std::string_literals;
  if (Argc != 2) {
    llvm::outs() << "usage: <prog> <output file name>\n";
    return 1;
  }
  llvm::LLVMContext Ctx;
  llvm::Module M("My Module", Ctx);
  M.setTargetTriple("x86_64-unknown-linux-gnu");
  addMainFunction(M);
  if (llvm::verifyModule(M, &llvm::errs())) {
    llvm::errs() << "error: generated invalid module\n";
    return 1;
  }
  llvm::outs() << "Generated the following module:\n";
  llvm::outs() << "-------------------------------\n";
  llvm::outs() << M;
  std::string Msg = "Writing LLVM IR to '"s + Argv[1] + "'\n"; // NOLINT
  llvm::outs() << Msg;
  llvm::outs() << std::string((Msg.length() > 0 ? Msg.length() - 1 : 0), '-')
               << '\n';
  std::error_code ECode;
  llvm::raw_fd_ostream Ofs(Argv[1], ECode, // NOLINT
                           llvm::sys::fs::CreationDisposition::CD_CreateAlways);
  WriteBitcodeToFile(M, Ofs);
  Ofs.flush();
  return 0;
}
