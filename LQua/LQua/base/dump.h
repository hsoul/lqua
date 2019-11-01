#pragma once

#include "basedef.h"

class TObject;
class ObjectProto;
class LQState;

void DumpObject(TObject* obj, int depth);
void DumpProto(ObjectProto* proto, int depth);
void DumpGlobal(LQState* ls);
void log(const char* fmt, ...);