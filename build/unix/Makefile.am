warningflags = -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wshadow
includeflags = -I$(srcdir)/../../src
commonflags = -O3 $(MFLAGS) $(warningflags) $(includeflags)
AM_CXXFLAGS = -std=c++11 $(commonflags)
AM_LDFLAGS  = `pkg-config --cflags --libs jack` -l wiringPi -l pthread -no-undefined -avoid-version $(PLUGINLDFLAGS)

bin_PROGRAMS = pedalevite

pedalevite_SOURCES = \
        ../../src/conc/AioAdd.h \
        ../../src/conc/AioAdd.hpp \
        ../../src/conc/AioMax.h \
        ../../src/conc/AioMax.hpp \
        ../../src/conc/AioSub.h \
        ../../src/conc/AioSub.hpp \
        ../../src/conc/AtomicInt.h \
        ../../src/conc/AtomicInt.hpp \
        ../../src/conc/AtomicIntOp.h \
        ../../src/conc/AtomicIntOp.hpp \
        ../../src/conc/AtomicMem.h \
        ../../src/conc/AtomicMem.hpp \
        ../../src/conc/AtomicPtr.h \
        ../../src/conc/AtomicPtr.hpp \
        ../../src/conc/AtomicPtrIntPair.h \
        ../../src/conc/AtomicPtrIntPair.hpp \
        ../../src/conc/CellPool.h \
        ../../src/conc/CellPool.hpp \
        ../../src/conc/def.h \
        ../../src/conc/fnc.h \
        ../../src/conc/fnc.hpp \
        ../../src/conc/Interlocked.h \
        ../../src/conc/Interlocked.hpp \
        ../../src/conc/LockFreeCell.h \
        ../../src/conc/LockFreeQueue.h \
        ../../src/conc/LockFreeQueue.hpp \
        ../../src/conc/LockFreeStack.h \
        ../../src/conc/LockFreeStack.hpp \
        ../../src/conc/ObjFactoryDef.h \
        ../../src/conc/ObjFactoryDef.hpp \
        ../../src/conc/ObjFactoryInterface.h \
        ../../src/conc/ObjFactoryInterface.hpp \
        ../../src/conc/ObjPool.h \
        ../../src/conc/ObjPool.hpp \
        ../../src/fstb/AllocAlign.h \
        ../../src/fstb/AllocAlign.hpp \
        ../../src/fstb/ArrayAlign.h \
        ../../src/fstb/ArrayAlign.hpp \
        ../../src/fstb/CpuId.cpp \
        ../../src/fstb/CpuId.h \
        ../../src/fstb/def.h \
        ../../src/fstb/fnc.cpp \
        ../../src/fstb/fnc.h \
        ../../src/fstb/fnc.hpp \
        ../../src/fstb/SingleObj.h \
        ../../src/fstb/SingleObj.hpp \
        ../../src/fstb/ToolsSimd.cpp \
        ../../src/fstb/ToolsSimd.h \
        ../../src/fstb/ToolsSimd.hpp \
        ../../src/jack/jack.h \
        ../../src/jack/systemdeps.h \
        ../../src/jack/transport.h \
        ../../src/jack/types.h \
        ../../src/jack/weakmacros.h \
        ../../src/mfx/piapi/Event.h \
        ../../src/mfx/piapi/EventNoteOff.h \
        ../../src/mfx/piapi/EventNoteOn.h \
        ../../src/mfx/piapi/EventParam.h \
        ../../src/mfx/piapi/EventTransport.h \
        ../../src/mfx/piapi/EventTs.h \
        ../../src/mfx/piapi/EventType.h \
        ../../src/mfx/piapi/ParamCateg.h \
        ../../src/mfx/piapi/ParamDescInterface.h \
        ../../src/mfx/piapi/ParamDescInterface.hpp \
        ../../src/mfx/piapi/PluginInterface.cpp \
        ../../src/mfx/piapi/PluginInterface.h \
        ../../src/mfx/tuner/FreqAnalyser.cpp \
        ../../src/mfx/tuner/FreqAnalyser.h \
        ../../src/mfx/ui/DisplayInterface.cpp \
        ../../src/mfx/ui/DisplayInterface.h \
        ../../src/mfx/ui/DisplayPi3Pcd8544.cpp \
        ../../src/mfx/ui/DisplayPi3Pcd8544.h \
        ../../src/mfx/ui/Font.cpp \
        ../../src/mfx/ui/Font.h \
        ../../src/mfx/ui/FontDataDefault.cpp \
        ../../src/mfx/ui/FontDataDefault.h \
        ../../src/mfx/ui/LedInterface.cpp \
        ../../src/mfx/ui/LedInterface.h \
        ../../src/mfx/ui/LedPi3.cpp \
        ../../src/mfx/ui/LedPi3.h \
        ../../src/mfx/ui/UserInputInterface.cpp \
        ../../src/mfx/ui/UserInputInterface.h \
        ../../src/mfx/ui/UserInputMsg.cpp \
        ../../src/mfx/ui/UserInputMsg.h \
        ../../src/mfx/ui/UserInputPi3.cpp \
        ../../src/mfx/ui/UserInputPi3.h \
        ../../src/mfx/ui/UserInputType.h \
        ../../src/mfx/GpioPwm.cpp \
        ../../src/mfx/GpioPwm.h \
        ../../src/mfx/Overdrive.cpp \
        ../../src/mfx/Overdrive.h \
        ../../src/mailbox.c \
        ../../src/mailbox.h \
        ../../src/main.cpp
