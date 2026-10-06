#include "IOFileUnit.h"

namespace Storm {

// OFFSET: %0x00A49270
void IORefcount::Release() {
    IORefcount* refcount = this;

    do {
        IORefcount* next = refcount->m_refcount;

        if (--refcount->m_count == 0) {
            delete refcount;
        }

        refcount = next;
    } while (refcount);
}

}
