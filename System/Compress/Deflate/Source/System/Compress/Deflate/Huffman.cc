#include <System/Compress/Deflate/Huffman.hh>

#include <stdexcept>


namespace System::Compress::Deflate
{


#if !__SYSTEM_COMPRESS_DEFLATE_RUNTIME_STATIC_HUFFMAN_TREE
const HuffmanTree HuffmanTree::s_staticTree = HuffmanTree::buildStaticTree();
#endif


void HuffmanTree::buildTree(std::span<const std::uint8_t> symbolLengths)
{
    buildTreeInternal(symbolLengths);
}


#if __SYSTEM_COMPRESS_DEFLATE_RUNTIME_STATIC_HUFFMAN_TREE
HuffmanTree HuffmanTree::staticTree() noexcept
{
    return buildStaticTree();
}
#endif


constexpr void HuffmanTree::buildTreeInternal(std::span<const std::uint8_t> symbolLengths)
{
    // Count the number of symbols with each bitlength.
    std::array<std::uint16_t, MaxBitLength + 1> lengthCount = {};
    for (auto length : symbolLengths)
    {
        // This should have been checked at an earlier stage of processing so this check is just a last-ditch attempt to
        // prevent anything invalid from getting through (and those invalid lengths should only have happened in the
        // first place if there's a decoding bug). But better safe than sorry when decoding anything!
        if (length > MaxBitLength)
        {
#if defined(__EXCEPTIONS) || defined(__cpp_exceptions)
            throw std::runtime_error("invalid symbol bitlength");
#else
            std::terminate();
#endif
        }

        ++lengthCount[length];
    }

    // For each bitlength, calculate the numeric value of the first used bitstring.
    std::array<std::uint16_t, MaxBitLength + 1> initialCode = {};
    for (std::size_t i = 1; i < MaxBitLength; ++i)
    {
        // The bitstring is the one with the value that comes after the last bitstring of the next-smaller size, with
        // an additional zero bit appended. For example: if the last bitstring of length 4 is 1011, the first bitstring
        // of length 5 will be 11000. As the DEFLATE rules specify that all bitstrings of the same length are allocated
        // consecutively, the value of the last bitstring of a particular length is equal to the value of the first
        // bitstring plus the number of symbols with that length.
        //
        // With the (implied) additional rule that the 0-length bitstring has numeric value zero, we can inductively
        // assign bitstrings to the symbols by knowing only their lengths.
        initialCode[i] = (initialCode[i - 1] + lengthCount[i - 1]) << 1;
    }

    // Complete the process of assigning values to each symbol.
    auto nextCode = initialCode;
    std::array<std::uint16_t, SymbolCount> values = {};
    for (std::size_t i = 0; i < SymbolCount; ++i)
    {
        // We special-case a length of zero as there is no value assigned in that case.
        auto length = symbolLengths[i];
        if (length != 0)
        {
            // As per the rules above, values are assigned consecutively to symbols of the same length.
            values[i] = nextCode[length]++;
        }
    }

    // Now that we know the value for each symbol, we can build the tree.
    //
    // The first entry in the table is reserved for the root node.
    //
    // Next come the leaf nodes that represent complete symbols.
    //
    // Finally, the interior nodes that indicate more bits are needed.

    // Insert the root.
    m_nodes[0] = Node{};

    // Insert the terminals.
    m_usedNodes = 1;
    for (std::size_t i = 0; i < SymbolCount; ++i)
    {
        m_nodes[m_usedNodes++] = Node{static_cast<Symbol>(i), nullptr, nullptr};
    }

    // Insert the interior nodes.
    for (std::size_t i = 0; i < SymbolCount; ++i)
    {
        // Skip any zero-length symbols to avoid arithmetic issues.
        auto length = symbolLengths[i];
        if (length == 0)
            continue;

        // Starting at the most-significant bit, make sure all nodes exist. We stop before reaching the least-
        // significant bit as that requires special handling.
        auto code = values[i];
        Node* node = &m_nodes[0];
        for (std::uint8_t bit = 0; bit < (length - 1); ++bit)
        {
            auto mask = 1U << (length - bit - 1);
            auto one = (code & mask) != 0;

            if (one)
            {
                // Allocate a new interior node, if needed.
                if (node->one() == nullptr)
                {
                    auto& newNode = m_nodes[m_usedNodes++];
                    newNode = {};
                    node->setOne(&newNode);
                }

                node = node->one();
            }
            else
            {
                // Allocate a new interior node, if needed.
                if (node->zero() == nullptr)
                {
                    auto& newNode = m_nodes[m_usedNodes++];
                    newNode = {};
                    node->setZero(&newNode);
                }

                node = node->zero();
            }
        }

        // Based on the least-significant bit of the bitstring, point the interior node to the terminal node.
        if (code & 1)
            node->setOne(&m_nodes[i + 1]);
        else
            node->setZero(&m_nodes[i + 1]);
    }
}


constexpr HuffmanTree HuffmanTree::buildStaticTree()
{
    HuffmanTree staticTree;
    
    auto symbolLengths = expandCodeRange(StaticHuffmanLengthRanges);
    staticTree.buildTreeInternal(std::span{symbolLengths.begin(), symbolLengths.end()});

    return staticTree;
}


} // namespace System::Compress::Deflate
