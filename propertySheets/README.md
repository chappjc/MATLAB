Visual Studio property sheets for rapidly configuring a project to build a MEX file.

The usage of the property sheets (.props files) is described at http://stackoverflow.com/a/27391300/2778484.  In short, just set the `MATLAB_ROOT` and/or `MATLAB32_ROOT` environment variables (for 32-bit and 64-bit versions, respectively), and include the property sheet (MATLAB.props) into your DLL project:

![add MEX property sheet to Visual Studio project](http://i.stack.imgur.com/pnmDB.png)

The relevant settings configured automatically by the property sheet include:

 1. Adding `$(MATLAB_ROOT)\extern\include` to the `AdditionalIncludeDirectories` paths (with inherited paths from parent configurations) -- the location of mex.h.
 2. Adding `$(MATLAB_ROOT)\extern\lib\win64\microsoft` to the `AdditionalLibraryDirectories` paths -- the location of libmex.lib, etc.
 3. Listing the libraries: `libut.lib;libmx.lib;libmex.lib;libmat.lib`.
 4. Exporting `mexFunction` (it's a shared library): `/EXPORT:mexFunction`.
 5. Setting the output file extention (e.g. `.mexw64` for x64).
 6. Sets MATLAB_MEX_FILE (helpful to signal to your own headers if they are being used in a MEX file).
 7. Turns on generation of data required for profiling.

The files MATLABx64.props and MATLABx86.props are for 64-bit and 32-bit MATLAB, respectively.  MATLAB.props is supposed to work for either 32-bit or 64-bit MATLAB using conditional expressions, but I have not tested it extensively.
