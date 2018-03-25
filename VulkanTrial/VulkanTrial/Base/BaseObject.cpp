#include "BaseObject.h"

namespace litter {
	static int ObjectCount = 0;

	BaseObject::BaseObject() {
		ObjectCount++;
	}

	BaseObject::~BaseObject() {
		ObjectCount--;
	}
}
