#include "IOFileUnit.h"

namespace Storm {

// OFFSET: 0x00B0A290/0x1408CDD60
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
