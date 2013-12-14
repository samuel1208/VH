#include "base-object.hpp"

THandle  Object::_memHandle = 0;

void  sakObject_setMemHandle (THandle memHandle)
{
    Object::setMemHandle(memHandle);
}
