#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

#include "core/geo/Geometry.h"

using NodeId = uint32_t;

struct CookContext
{
    // later: time, frame, random seed, cancellation, etc.
};

class Node
{
public:
    explicit Node(NodeId id) : m_id(id) {}
    virtual ~Node() = default;

    NodeId id() const { return m_id; }

    // UI display name (instance name)
    const std::string& name() const { return m_name; }
    void setName(std::string n) { m_name = std::move(n); }

    // type name for factory/registry
    virtual const char* typeName() const = 0;

    // SOP nodes: single output geometry
    virtual Geometry cook(const CookContext& ctx,
                          const std::vector<std::shared_ptr<const Geometry>>& inputs) const = 0;

    // Parameters revision: bump when user edits params
    uint64_t paramRevision() const { return m_paramRev; }
    void bumpParamRevision() { ++m_paramRev; }

    // Graph topology revision bump happens in Graph; nodes track only params here.

private:
    NodeId m_id;
    std::string m_name;
    uint64_t m_paramRev = 1;
};