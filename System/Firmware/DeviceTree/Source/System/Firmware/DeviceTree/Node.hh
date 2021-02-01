#ifndef __SYSTEM_FIRMWARE_DEVICETREE_NODE_H
#define __SYSTEM_FIRMWARE_DEVICETREE_NODE_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <span>
#include <string>
#include <vector>


namespace System::Firmware::DeviceTree
{


enum class PHandle : std::uint32_t {};

inline constexpr auto operator<=>(PHandle x, PHandle y)
{
    return static_cast<std::uint32_t>(x) <=> static_cast<std::uint32_t>(y);
}


class Property
{
public:

    enum class Type
    {
        Unknown,
        Empty,
        U32,
        S32,
        U64,
        S64,
        String,
        PHandle,
        StringList,
        PropEncodedArray,

        // Well-known PropEncodedArray specificiations.
        Reg,
        Ranges,
    };


    static bool isValidPropertyName(std::string_view);

    static Type guessTypeFromName(std::string_view);

private:

    std::string_view            m_name          = {};
    std::vector<std::byte>      m_value         = {};
};


class Node :
    public std::enable_shared_from_this<Node>
{
public:

    using property_list         = std::map<std::string_view, Property>;
    using node_list             = std::map<std::string_view, std::shared_ptr<Node>>;


    Node* parent() const
    {
        return m_parent;
    }

    std::string_view name() const
    {
        return m_name;
    }

    std::string_view baseName() const
    {
        auto at = m_name.find_last_of('@');
        return m_name.substr(0, at);
    }

    std::string_view unitName() const
    {
        auto base = baseName();
        return m_name.substr(base.length());
    }

    std::shared_ptr<const Node> findChild(std::string_view name) const
    {
        auto i = m_children.find(name);
        if (i == m_children.end())
            return nullptr;

        return i->second;
    }

    const Property* findProperty(std::string_view name) const
    {
        auto i = m_properties.find(name);
        if (i == m_properties.end())
            return nullptr;

        return &i->second;
    }

    const property_list& properties() const noexcept
    {
        return m_properties;
    }

    const node_list& children() const noexcept
    {
        return m_children;
    }

    std::shared_ptr<Node> addChild(std::string_view name);

    bool removeChild(std::string_view name);

    std::uint32_t numAddressCells() const;

    std::uint32_t numSizeCells() const;


    static bool isValidNodeName(std::string_view);

private:

    Node*                       m_parent        = nullptr;
    std::string                 m_name          = {};
    property_list               m_properties    = {};
    node_list                   m_children      = {};
};


class RootNode :
    public Node
{
public:

    Node* getNode(PHandle);
    Node* getNode(std::string_view path);
    Node* getNodeByPath(std::string_view path);
    Node* getNodeByAlias(std::string_view alias);

    void merge(const RootNode&);
    void merge(RootNode&);
    void merge(RootNode&&);

    bool parseFDT(std::span<std::byte>);
    bool applyOverlay(std::span<std::byte>);

private:

    using alias_table           = std::map<std::string_view, std::weak_ptr<Node>>;
    using phandle_map           = std::map<PHandle, std::weak_ptr<Node>>;
    using string_table          = std::set<std::string>;

    phandle_map                 m_phandles      = {};
    string_table                m_strings       = {};

    // Alias cache.
    mutable alias_table         m_aliases       = {};
};


} // namespace System::Firmware::DeviceTree


#endif /* ifndef __SYSTEM_FIRMWARE_DEVICETREE_NODE_H */
