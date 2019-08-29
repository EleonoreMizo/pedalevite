# Pédale Vite audio plug-ins to VST

These plug-ins are GUI-less VST versions of some of the Pédale Vite internal plug-ins.


### How to convert a Pédale Vite plug-in to a VST 2.4 plug-in

1. Identify the innermost namespace of the plug-in and its two main class names (usually `PluginName` and `PluginNameDesc`)
2. Duplicate a project directory from `build/win/pvvst`
3. Rename the directory `pv_namespace` (where the “namespace” part is the name you got at step 1)
4. Rename the contained `.vcxproj` file with `pv_namespace`
5. Open the `.vcxproj` **with a text editor** and rename all the occurences to `pv_namespace`
6. Run a GUID generator, generate a new GUID for the project and replace it (look for the `<ProjectGuid>` tag)
7. Delete all other temporary or output files and sub-directories from the project directory
8. Duplicate a source directory from `src/pvvst`
9. Rename the source directory with you guess what
10. Include the project in the MSVC solution
11. Add `mfxlib` and `piapi2vst` as project dependencies
12. Open `main.cpp` from the `pv_namespace` project
13. Replace the namespace and class names (from step 1) in the `include` and `typedef` statements
14. Save and compile!

I know all these steps could be easily automated but I’m really lazy.
