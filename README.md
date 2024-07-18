# Example LLVM Pass Plugin
An out-of-tree transormation pass that replaces addition with multiplication.

## Running the pass
1. Install LLVM.
> [Getting started with LLVM (external guide)](https://www.cs.utexas.edu/~pingali/CS380C/2019/assignments/llvm-guide.html)
> [Getting Started with the LLVM System (LLVM.org)](https://llvm.org/docs/GettingStarted.html)
2. Run
```bash
cmake -S . -B build
cmake --build build
```
3. Run the pass with `opt`
```bash
opt -load-pass-plugin ./build/libMBAAdd.so -passes=mba-add a.ll -S -o a_out.ll
```

## About the pass plugin framework
A short explanation of how the flow of running the plugins works in the `opt` tool in LLVM. All the code below is from the `llvm-project/llvm/tools/opt` directory.

### `PassPlugin`
All plugins you write are to be bundled as an instance of this struct:

```cpp PassPlugin.h
struct PassPluginLibraryInfo {
	uint32_t APIVersion;
	const char* PluginName;
	const char* PluginVersion;
	void (*RegisterPassBuilderCallbacks)(PassBuilder&);
}
```

You specify the pass you want to load with the `-load-pass-plugin` cli argument as a list of paths of the passes' dynamic library locations. This creates `PassPlugin` objects that load the library.
This finds the address of the symbol `llvmGetPassPluginInfo` and calls it to get the `PassPluginLibraryInfo` from your library *(shown in the below snippet)*.
- After this the plugin library info version is checked.

 > [!info] From the `PassPlugin::Load` method
  >```cpp
>  // llvmGetPassPluginInfo should be resolved to the definition from the plugin
  > // we are currently loading.
  > intptr_t getDetailsFn =
 >     (intptr_t)Library.getAddressOfSymbol("llvmGetPassPluginInfo");
 > P.Info = reinterpret_cast<decltype(llvmGetPassPluginInfo) *>(getDetailsFn)();
 >```

Before calling the `runPassPipeline` function, we have the list of all such `PassPlugins`.

### `runPassPipeline`
This function is the main function called when `opt` is run. The parade travels through as follows:
1. Construct the `PassBuilder` object `PB`.
2. For every `PassPlugin` (that was already loaded), call the plugin's `RegisterPassBuilderCallbacks(PB)` method (that is in the `PassPluginLibraryInfo` struct).
3. Register all basic analyses with the managers (with `PB.register<IRUnitLevel>Analyses(<theAnalysisManager>))`)
4. `ModulePassManager MPM` is a top level pass created. Debugify pass is added to this.
5. Add passes according to the `PassPipeline` passes taken from the cli option `-passes=<>` by doing `PB.parsePassPipeline(MPM, PassPipeline)`.
6. Add the verifier pass, debugify pass to `MPM`
7. Add relevant output pass (output assembly/bitcode/thinLTOBitcode)
8. Print the pipeline passes if requested.
9. **Run the passes: `MPM.run(M, MAM)`** where `M` is the main Module and MAM is the module analysis manager.
10. Return.


### How your pass ends up in the `PassBuilder`'s FPM
1. `PB.registerPipelineParsingCallback(FPMHook)` is called when step `2` is run (in the above `runPassPipeline` flow).
2. This pushes the `FPMHook` callback into a vector `FunctionPipelineParsingCallbacks`
3. This is called for every `Name` in the `Pipeline` (which is a list of strings)
4. The `FPM` passed into `FPMHook` is created by `parseModulePass` which adds it to the `ModulePassManager MPM` through a `ModuleToFunctionPassAdaptor` module level pass.
5. The above came from invoking `PB.parsePassPipeline` with a `ModulePassManager MPM` (which is the same in step `4`).
6. This `MPM` is constructed in step `4` of the `runPassPipeline` method which is `opt`'s driver function.

