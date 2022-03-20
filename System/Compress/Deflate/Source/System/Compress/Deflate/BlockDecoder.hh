#ifndef __SYSTEM_COMPRESS_DEFLATE_BLOCKDECODER_H
#define __SYSTEM_COMPRESS_DEFLATE_BLOCKDECODER_H


#include <System/Compress/Deflate/Bitstream.hh>
#include <System/Compress/Deflate/Huffman.hh>
#include <System/Compress/Deflate/LZ77.hh>


namespace System::Compress::Deflate
{


enum class BlockType
{
    Uncompressed    = 0b00,
    StaticHuffman   = 0b01,
    DynamicHuffman  = 0b10,
    Invalid         = 0b11,
};


enum class DecoderState
{
    Initial         = 0,        // Block has not yet been started.
    HaveBlockType,              // We have the block type and are reading metadata.
    CodeLengths,                // Reading Huffman tree code lengths.
    MainTree,                   // Reading main Huffman tree.
    DistanceTree,               // Reading distance Huffman tree.
    Data,                       // Reading block data.
    End,                        // Block is complete.
};


class BlockDecoder
{
public:

    void setInput(std::span<const std::byte> buffer, Bitstream::MoreInputFn moreInput = {});

    void setOutput(std::span<std::byte> buffer, LZ77Decoder::FlushBufferFn flushOutput = {});

private:

    // Code lengths are stored in an unusual order in the bitstream.
    static constexpr std::size_t CodeLengthMap[19] =
    {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15,
    };


    // Decoder state.
    DecoderState        m_state = DecoderState::Initial;

    // Input bitstream.
    Bitstream           m_input;

    // The type of block being decompressed.
    bool                m_final = false;
    BlockType           m_type = BlockType::Invalid;

    // Block dictionary parameters (only relevant to blocks with dynamic Huffman trees).
    std::uint16_t       m_mainSymbolCount = 0;          // Number of symbols present from the main alphabet.
    std::uint8_t        m_distanceSymbolCount = 0;      // Number of symbols present from the distance alphabet.
    std::uint8_t        m_codeLengthSymbolCount = 0;    // Number of symbols present from the code length alphabet.

    // Huffman tree for decoding the code lengths for the other two Huffman trees.
    HuffmanTree         m_codeLengthTree;

    // Huffman tree for decoding the main symbol alphabet.
    HuffmanTree         m_mainTree;

    // Huffman tree for decoding the distance symbol alphabet.
    HuffmanTree         m_distanceTree;

    // LZ77 decoder. Also holds the output buffer information.
    LZ77Decoder         m_lz77;

    // Current (partial) Huffman code, represented as a pointer to a node in one of the trees.
    const HuffmanTree::Node*    m_huffmanState = nullptr;

    // Remaining number of bytes to copy in the current uncompressed block.
    std::uint16_t       m_uncompressedBytesRemaining = 0;

    // Code length array for the Huffman tree being built.
    std::array<std::uint8_t, HuffmanTree::SymbolCount>  m_codeLengths = {};
    std::uint16_t       m_codeLengthsRead = 0;

    // Some framing formats (e.g. zlib) offer a way to declare the limits on various parameters of the stream.
    std::uint32_t       m_maxDistance = 32768;          // Limit on the maximum LZ77 distance ("window size").


    bool process();

    bool processUncompressed();
    bool processCompressed();

    void writeOutput(std::byte);
    void writeOutput(std::span<const std::byte>);

    void finish();
};


} // namespace System::Compress::Deflate


#endif /* ifndef __SYSTEM_COMPRESS_DEFLATE_BLOCKDECODER_H */
