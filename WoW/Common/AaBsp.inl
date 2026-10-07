#ifndef WOW_COMMON_AABSP_INL
#define WOW_COMMON_AABSP_INL

#include "WoW/Common/Types.h"

// INLINED
inline CAaBspDigestCache::~CAaBspDigestCache() {
    this->Reset();
}

// OFFSET: [0x00791300<BspQuery_Segment>]
template <class T>
void CAaBsp_Query<T>::GetFaceIndices(const CAaBspNode* node) {
    if (this->m_f->GetFaceIndicesUsingCache(*this->m_aaBsp, node)) {
        return;
    }

    const uint16_t* faces = &this->m_aaBsp->m_nodeFaceIndices[node->m_faceStart];

    for (uint32_t i = 0; i < node->m_nFaces; i++) {
        (*this->m_f)(faces[i]);
    }
}

// OFFSET: [0x00791B40<BspQuery_Segment>]
template <class T>
void CAaBsp_Query_Segment<T>::GetFaceIndices(uint32_t nodeIndex, const C3Segment& seg, const CAaBox& box) {
    const CAaBspNode* node = &this->m_aaBsp->m_nodes[nodeIndex];

    if (!node) {
        return;
    }

    if (node->m_flags & 0x4) {
        this->CAaBsp_Query<T>::GetFaceIndices(node);
        return;
    }

    uint32_t axis = node->m_flags & 0x3;
    const float* segB = &seg.b.x;
    const float* segT = &seg.t.x;
    const float* boxB = &box.b.x;
    const float* boxT = &box.t.x;

    if (!(segB[axis] - boxB[axis] >= -0.0099999998f || segT[axis] - boxB[axis] >= -0.0099999998f)) {
        return;
    }

    if (!(segB[axis] - boxT[axis] <= 0.0099999998f || segT[axis] - boxT[axis] <= 0.0099999998f)) {
        return;
    }

    CAaBox posBox = box;
    (&posBox.b.x)[axis] = node->m_planeDist;
    CAaBox negBox = box;
    (&negBox.t.x)[axis] = node->m_planeDist;

    float distB = segB[axis] - node->m_planeDist;
    float distT = segT[axis] - node->m_planeDist;

    if ((distB >= -0.0099999998f && distB <= 0.0099999998f) || (distT >= -0.0099999998f && distT <= 0.0099999998f)) {
        if (node->m_posChild != 0xFFFF) {
            this->GetFaceIndices(node->m_posChild, seg, posBox);
        }

        if (node->m_negChild != 0xFFFF) {
            this->GetFaceIndices(node->m_negChild, seg, negBox);
        }

        return;
    }

    if (distB > 0.0099999998f && distT > 0.0099999998f) {
        if (node->m_posChild != 0xFFFF) {
            this->GetFaceIndices(node->m_posChild, seg, posBox);
        }

        return;
    }

    if (distB < -0.0099999998f && distT < -0.0099999998f) {
        if (node->m_negChild != 0xFFFF) {
            this->GetFaceIndices(node->m_negChild, seg, negBox);
        }

        return;
    }

    float ratio = distB / (distB - distT);
    C3Vector split;
    split.x = seg.b.x + (seg.t.x - seg.b.x) * ratio;
    split.y = seg.b.y + (seg.t.y - seg.b.y) * ratio;
    split.z = seg.b.z + (seg.t.z - seg.b.z) * ratio;

    C3Segment part;

    if (distB <= 0.0f) {
        if (node->m_negChild != 0xFFFF) {
            part.b = seg.b;
            part.t = split;
            this->GetFaceIndices(node->m_negChild, part, negBox);
        }

        if (node->m_posChild != 0xFFFF) {
            part.b = split;
            part.t = seg.t;
            this->GetFaceIndices(node->m_posChild, part, posBox);
        }
    } else {
        if (node->m_posChild != 0xFFFF) {
            part.b = seg.b;
            part.t = split;
            this->GetFaceIndices(node->m_posChild, part, posBox);
        }

        if (node->m_negChild != 0xFFFF) {
            part.b = split;
            part.t = seg.t;
            this->GetFaceIndices(node->m_negChild, part, negBox);
        }
    }
}

#endif
