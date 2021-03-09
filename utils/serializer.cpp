//
// Created by Mengwen Li (ml4643) on 3/7/2021.
//

#include "serializer.h"

#include "log.h"

vector<char>&& Serializer::getBlob() && { return move(m_blob); }

void Serializer::appendSize(int size) {
    log::debug("Calling appendSize with size: ", size);
    // TODO: @mli: Convert size to blob and append it to m_blob.
}