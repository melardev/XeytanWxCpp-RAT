# WARNING
THIS PROJECT IS STILL IN THE EARLY PHASES OF DEVELOPMENT.
AT THIS MOMENT IN TIME, THE APPLICATION LOOKS HORRIBLE, THERE IS AN ODD BUG (FIRST TODO POINT, SEE BELOW)
THAT HAS TO BE FIXED ASAP.

# Introduction
RAT application written in C++ using wxWidgets framework.

# Features
- Get client's system information.
- Process List
- Reverse Shell
- Explore FileSystem

# TODO
- There is a weird bug, I don't know how to fix, while stepping, it is not triggered, but if
I just launch the app it does, this is a typical sign of a race condition bug but I don't know
how to fix, the bug can be inspected in evtloopconsole.cpp:GetNextMessageTimeout::MsgWaitForMultipleObjects
It gets stuck, even with a message being sent ...
- Graphical User Interface
- Syncrhonization with locks
- Improve Error Handling
- Implement FileSystem - cd command.
- Desktop streaming
- Camera Streaming
- Encryption
