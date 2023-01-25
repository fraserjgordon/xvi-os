#ifndef __SYSTEM_COMPRESS_DEFLATE_HUFFMAN_H
#define __SYSTEM_COMPRESS_DEFLATE_HUFFMAN_H


#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <span>


// For kernel builds, we don't pre-compile the static Huffman tree into the binary as it's rarely needed and consumes a
// fair bit of space (a few kB).
#ifdef XVI_KERNEL
#  define __SYSTEM_COMPRESS_DEFLATE_RUNTIME_STATIC_HUFFMAN_TREE     1
#endif


namespace System::Compress::Deflate
{

class HuffmanTree
{
public:

    enum class Symbol : std::uint16_t {};

    static constexpr Symbol InvalidSymbol = static_cast<Symbol>(0xFFFF);

    using node_value_t      = Symbol;
    using node_pointer_t    = std::int16_t;

    class Node
    {
        friend class HuffmanTree;

    public:

        constexpr Node() = default;
        constexpr Node(const Node&) = default;
        constexpr Node(Node&&) = default;

        ~Node() = default;

        constexpr Node& operator=(const Node&) = default;
        constexpr Node& operator=(Node&&) = default;


        constexpr node_value_t value() const noexcept
        {
            return m_value;
        }

        constexpr bool hasValue() const noexcept
        {
            return value() != InvalidSymbol;
        }

        constexpr const Node* zero() const noexcept
        {
            return m_zero ? this + m_zero : nullptr;
        }

        constexpr const Node* one() const noexcept
        {
            return m_one ? this + m_one : nullptr;
        }

    private:

        constexpr Node(node_value_t value, Node* zero, Node* one) :
            m_value{value},
            m_zero{static_cast<node_pointer_t>(zero ? (zero - this) : 0)},
            m_one{static_cast<node_pointer_t>(one ? (one - this) : 0)}
        {
        }


        constexpr void setZero(Node* node)
        {
            m_zero = static_cast<node_pointer_t>(node ? (node - this) : 0);
        }

        constexpr void setOne(Node* node)
        {
            m_one = static_cast<node_pointer_t>(node ? (node - this) : 0);
        }

        constexpr Node* zero() noexcept
        {
            return m_zero ? (this + m_zero) : nullptr;
        }

        constexpr Node* one() noexcept
        {
            return m_one ? (this + m_one) : nullptr;
        }


        node_value_t        m_value = InvalidSymbol;
        node_pointer_t      m_zero = 0;
        node_pointer_t      m_one = 0;
    };

    // Used as a short-form way of storing the static Huffman tree.
    struct code_range
    {
        std::uint16_t  lastSymbol;     // The last symbol in this range.
        std::uint8_t   bitLength;      // Bit length of symbols in this range.
    };

    static constexpr std::size_t MaxBitLength = 15;

    static constexpr std::size_t SymbolCount = 288;
    using symbol_lengths_t  = std::array<std::uint8_t, SymbolCount>;

    static constexpr std::size_t MaxNodes = 2 * SymbolCount + MaxBitLength;
    using node_array_t      = std::array<Node, MaxNodes>;

    static constexpr std::initializer_list<code_range> StaticHuffmanLengthRanges =
    {  
        // These are specified in the DEFLATE standard.
        {143, 8}, {255, 9}, {279, 7}, {287, 8}
    };


    constexpr HuffmanTree() = default;
    constexpr HuffmanTree(const HuffmanTree&) = default;
    constexpr HuffmanTree(HuffmanTree&&) = default;

    ~HuffmanTree() = default;

    constexpr HuffmanTree& operator=(const HuffmanTree&) = default;
    constexpr HuffmanTree& operator=(HuffmanTree&&) = default;


    void buildTree(std::span<const std::uint8_t> symbolLengths);

    constexpr const Node* root() const noexcept
    {
        return &m_nodes[0];
    }

    // Utility method that expands a short-form length-range form into the full symbol length array.
    static constexpr std::array<std::uint8_t, SymbolCount> expandCodeRange(std::span<const code_range> ranges)
    {
        std::array<std::uint8_t, SymbolCount> lengths = {};
        std::uint16_t nextCode = 0;
        for (const auto& range : ranges)
        {
            for (; nextCode <= range.lastSymbol; ++nextCode)
                lengths[nextCode] = range.bitLength;
        }

        return lengths;
    }


#if __SYSTEM_COMPRESS_DEFLATE_RUNTIME_STATIC_HUFFMAN_TREE
    static HuffmanTree staticTree() noexcept;
#else
    static const HuffmanTree& staticTree() noexcept
    {
        return s_staticTree;
    }
#endif

    static constexpr HuffmanTree makeStaticTree() noexcept
    {
        return buildStaticTree();
    }


private:

    node_array_t        m_nodes;
    std::size_t         m_usedNodes = 0;


    constexpr void buildTreeInternal(std::span<const std::uint8_t> symbolLengths);


#if !__SYSTEM_COMPRESS_DEFLATE_RUNTIME_STATIC_HUFFMAN_TREE
    static const HuffmanTree s_staticTree;
#endif


    static constexpr HuffmanTree buildStaticTree();
};


} // namespace System::Compress::Deflate


#endif /* ifndef __SYSTEM_COMPRESS_DEFLATE_HUFFMAN_H */
