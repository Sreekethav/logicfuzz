pushd MS
conan install --update -if tmp/conan conanfile.txt
call tmp/conan/activate.bat

REM Generate HTML and XML output
if not exist out/ms mkdir out\\ms
SET COMPBUILD_GENERATE_HTML=YES
doxygen.exe

REM Generate GitHub markdown out of XML output
java -jar %DOXYMD_PATH%/doxymd.jar -i out/ms/xml/index.xml -o out/ms/md

call tmp/conan/deactivate.bat
popd
