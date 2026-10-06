#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CFrameStrataNode.h"

// OFFSET: %0x00511490
CFrameStrataNode::CFrameStrataNode() {
    this->m_pendingFrame = NULL;
    this->m_batchDirty = 0;
}

// OFFSET: %0x0050F5E0
int32_t CFrameStrataNode::RemoveFrame(CSimpleFrame* frame) {
    if (!frame->m_strataLink.IsLinked()) {
        return 0;
    }

    if (frame == this->m_pendingFrame) {
        this->m_pendingFrame = frame->m_strataLink.Next();
    }

    frame->m_strataLink.Unlink();

    if (!(frame->m_frameFlags & 0x2000)) {
        this->m_batchDirty = -1;
    }

    return this->m_batchDirty != 0;
}

// OFFSET: %0x005101C0
void CFrameStrataNode::OnLayerUpdate(float elapsedSec) {
    for (CSimpleFrame* frame = this->m_frames.Head(); frame; frame = this->m_pendingFrame) {
        this->m_pendingFrame = frame->m_strataLink.Next();
        frame->OnLayerUpdate(elapsedSec);
    }

    this->m_frames.Combine(&this->m_pendingFrames, 2, NULL);
}

// OFFSET: %0x005102A0
int32_t CFrameStrataNode::BuildBatches() {
    int32_t batchDirty = this->m_batchDirty;
    this->m_batchDirty = 0;

    if (!batchDirty) {
        return 0;
    }

    for (uint32_t layer = 0; layer < 5; layer++) {
        CRenderBatch* batch = &this->m_batches[layer];

        if (this->m_renderList.IsLinked(batch)) {
            this->m_renderList.UnlinkNode(batch);
        }

        if (batchDirty & (1 << layer)) {
            batch->Clear();

            for (CSimpleFrame* frame = this->m_frames.Head(); frame; frame = this->m_frames.Next(frame)) {
                if (!(frame->m_frameFlags & 0x2000) && !frame->m_loadPending) {
                    frame->OnFrameRender(batch, layer);
                }
            }

            batch->Finish();
        }

        if (batch->m_count) {
            this->m_renderList.LinkToTail(batch);
        }
    }

    if (this->m_batchDirty & 0x20) {
        this->m_batchDirty |= 0x1F;
    }

    return 0;
}

// OFFSET: %0x00511510
CFrameStrataNode::~CFrameStrataNode() {
    this->m_frames.UnlinkAll();
    this->m_renderList.UnlinkAll();
}
