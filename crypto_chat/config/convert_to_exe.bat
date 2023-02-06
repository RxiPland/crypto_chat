pyinstaller --onefile cryptographic_tool.py
cd dist
move /Y cryptographic_tool.exe ..
cd ..
rmdir /s /q dist
rmdir /s /q build
del /q cryptographic_tool.spec