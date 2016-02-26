// Copyright (C) 2012-2016 The VPaint Developers.
// See the COPYRIGHT file at the top-level directory of this distribution
// and at https://github.com/dalboris/vpaint/blob/master/COPYRIGHT
//
// This file is part of VPaint, a vector graphics editor. It is subject to the
// license terms and conditions in the LICENSE.MIT file found in the top-level
// directory of this distribution and at http://opensource.org/licenses/MIT

#include "Operator.h"

#include <OpenVAC/VAC.h>
#include <OpenVAC/Topology/KeyVertex.h>
#include <OpenVAC/Topology/KeyEdge.h>
#include <OpenVAC/Operators/CellDataConverter.h>

namespace OpenVAC
{

Operator::Operator(VAC * vac) :
    vac_(vac),
    isValidated_(false),
    isComputed_(false),
    isApplied_(false),
    numIdRequested_(0)
{
}

VAC * Operator::vac() const
{
    return vac_;
}

bool Operator::isValid()
{
    if (!isValidated_)
    {
        valid_ = isValid_();
        isValidated_ = true;
    }
    return valid_;
}

Operator & Operator::compute()
{
    assert(isValid());
    if (!isComputed_)
    {
        compute_();
        isComputed_ = true;
    }
    return *this;
}

bool Operator::isComputed() const
{
    return isComputed_;
}

Operator & Operator::apply()
{
    assert(!isApplied());
    compute();
    apply_();
    isApplied_ = true;
    return *this;
}

bool Operator::isApplied() const
{
    return isApplied_;
}

const std::vector<CellId> & Operator::newCells()
{
    return newCells_;
}

const std::vector<CellId> & Operator::deletedCells()
{
    return deletedCells_;
}

void Operator::apply_()
{
    // Deallocate deleted cells
    for (CellId id: deletedCells_)
    {
        vac_->cellManager_.remove(id);
    }

    // Allocate new cells
    for (CellId id: newCells_)
    {
        CellSharedPtr cell = make_shared(cellsAfter_[id]->type(), id);
        vac_->cellManager_.insert(id, cell);
    }

    // Copy cell data from operator to VAC
    OpCellDataToCellDataConverter converter(vac());
    for (auto & id_data_pair: cellsAfter_)
    {
        CellId id = id_data_pair.first;
        const OpCellData & opCellData = *id_data_pair.second;
        CellData & cellData = vac_->cellManager_[id]->data();

        converter.convert(opCellData, cellData);
    }
}

template <class OpCellDataType>
WeakPtr<OpCellDataType> Operator_newCell_(Operator & self, CellId * outId)
{
    // Get available ID
    CellId id = self.getAvailableId_();
    if (outId)
        *outId = id;

    // Allocate OpCellData
    auto opCellData = std::make_shared<OpCellDataType>();

    // Insert in cellsAfter_
    auto res = self.cellsAfter_.insert(std::make_pair(id, opCellData));

    // Abort if ID already taken
    assert(res.second);

    // Tag this ID as a cell created by the operator
    self.newCells_.push_back(id);

    // Return the allocated OpCellData
    return opCellData;
}

OpKeyVertexDataPtr Operator::newKeyVertex(KeyVertexId * outId)
{
    return Operator_newCell_<OpKeyVertexData>(*this, outId);
}

OpKeyEdgeDataPtr Operator::newKeyEdge(KeyEdgeId * outId)
{
    return Operator_newCell_<OpKeyEdgeData>(*this, outId);
}

CellId Operator::getAvailableId_() const
{
    // Note: calling vac_->cellManager_.getAvailableId() repeatedly would always
    // give the same ID (since we're not inserting a cell after the call).
    // Therefore, for the second call to Operator::getAvailableId(), we actually
    // need to request two IDs, and ignore the first one. The IDs are guaranteed
    // to be always generated deterministically in the same order.
    ++numIdRequested_;
    std::vector<CellId> ids = vac_->cellManager_.getAvailableIds(numIdRequested_);
    return ids[numIdRequested_-1];
}

std::vector<CellId> Operator::getAvailableIds_(unsigned int numIds) const
{
    // Same as above: we re-generate previously generated IDs, but only return
    // the new ones
    numIdRequested_ += numIds;
    std::vector<CellId> ids = vac_->cellManager_.getAvailableIds(numIdRequested_);
    return std::vector<CellId>(&ids[numIdRequested_-numIds], &ids[numIdRequested_]);
}

CellSharedPtr Operator::make_shared(CellType type, CellId id) const
{
    if (type == CellType::KeyVertex)
    {
        return std::make_shared<KeyVertex>(vac(), id);
    }
    else if (type == CellType::KeyEdge)
    {
        return std::make_shared<KeyEdge>(vac(), id);
    }
    else
    {
        // XXX TODO
    }
}

}
