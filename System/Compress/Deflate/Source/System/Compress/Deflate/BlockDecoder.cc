#include <System/Compress/Deflate/BlockDecoder.hh>


namespace System::Compress::Deflate
{


bool BlockDecoder::process()
{
    // What state are we currently in?
    switch (m_state)
    {
        case DecoderState::Initial:
            // We need to be able to read three bits to get the block type.
            if (!m_input.canReadBits(3))
                return false;

            m_final = m_input.read(1);
            m_type = static_cast<BlockType>(m_input.read(2));

            m_state = DecoderState::HaveBlockType;
            break;

        case DecoderState::HaveBlockType:
        {
            // What type of block are we dealing with?
            switch (m_type)
            {
                case BlockType::Uncompressed:
                    // We need to be able to read 16 bits (after aligning!) to get the block length.
                    m_input.byteAlign();
                    if (!m_input.canReadBits(16))
                        return false;

                    m_uncompressedBytesRemaining = m_input.read(16);

                    m_state = DecoderState::Data;
                    break;

                case BlockType::StaticHuffman:
                    // No metadata is needed. Just set the Huffman tables statically.
                    //
                    // We don't bother constructing the distance tree; we just read 5 bits directly for each distance.
                    m_mainSymbolCount = 288;
                    m_distanceSymbolCount = 32;
                    m_codeLengthSymbolCount = 0;
                    m_codeLengthTree = {};
                    m_mainTree = HuffmanTree::staticTree();
                    m_distanceTree = {};

                    m_state = DecoderState::Data;
                    break;

                case BlockType::DynamicHuffman:
                    // We need to read 5+5+4 = 14 bits to find out the number of symbols of each type.
                    if (!m_input.canReadBits(14))
                        return false;

                    m_mainSymbolCount = 257 + m_input.read(5);
                    m_distanceSymbolCount = 1 + m_input.read(5);
                    m_codeLengthSymbolCount = 4 + m_input.read(4);
                    m_codeLengths = {};
                    m_codeLengthsRead = 0;

                    m_state = DecoderState::CodeLengths;
                    break;

                case BlockType::Invalid:
                    // Shouldn't happen!
                    return false;
            }

            break;
        }

        case DecoderState::CodeLengths:
        {
            // Each code length is 3 bits.
            if (!m_input.canReadBits(3))
                return false;

            while (m_input.canReadBits(3))
            {
                // Read the next code length and insert it into the array.
                auto index = CodeLengthMap[m_codeLengthsRead];
                m_codeLengths[index] = m_input.read(3);

                // Have we finished reading the code lengths?
                if (++m_codeLengthsRead == m_codeLengthSymbolCount)
                {
                    // Construct the Huffman tree.
                    m_codeLengthTree = {};
                    m_codeLengthTree.buildTree(m_codeLengths);

                    // Set things up ready for the next stage.
                    m_huffmanState = m_codeLengthTree.root();
                    m_codeLengthsRead = 0;
                    m_codeLengths = {};

                    m_state = DecoderState::MainTree;
                    break;
                }
            }

            break;
        }

        // These cases are very similar and are handled together.
        case DecoderState::MainTree:
        case DecoderState::DistanceTree:
        {
            // We read Huffman-encoded symbols one bit at a time.
            if (!m_input.canReadBits(1))
                return false;

            while (m_input.canReadBits(1))
            {
                // Get the bit and advance through the tree.
                auto bit = m_input.readBit();
                m_huffmanState = bit ? m_huffmanState->one() : m_huffmanState->zero();

                // If we've reached a null pointer, something has gone badly wrong!
                //! @todo: error handling.
                if (!m_huffmanState)
                    return false;

                // Check for if the symbol is now complete.
                if (m_huffmanState->hasValue())
                {
                    // Insert this length into the code length array.
                    auto length = static_cast<std::uint8_t>(m_huffmanState->value());
                    m_codeLengths[m_codeLengthsRead++] = length;

                    // If we have all the lengths, we can move on to the next stage.
                    auto target = (m_state == DecoderState::MainTree) ? m_mainSymbolCount : m_distanceSymbolCount;
                    if (m_codeLengthsRead == target)
                    {
                        // Construct the Huffman tree.
                        if (m_state == DecoderState::MainTree)
                        {
                            m_mainTree = {};
                            m_mainTree.buildTree(m_codeLengths);
                        }
                        else
                        {
                            m_distanceTree = {};
                            m_distanceTree.buildTree(m_codeLengths);
                        }

                        // Set things up ready for the next stage.
                        m_codeLengthsRead = 0;
                        m_codeLengths = {};
                        if (m_state == DecoderState::MainTree)
                        {
                            // Distance tree comes after main tree.
                            m_huffmanState = m_codeLengthTree.root();

                            m_state = DecoderState::DistanceTree;
                        }
                        else
                        {
                            // Data comes after the distance tree.
                            m_huffmanState = m_mainTree.root();

                            m_state = DecoderState::Data;
                        }

                        break;
                    }

                    // Reset the Huffman state ready for the start of the next symbol.
                    m_huffmanState = m_codeLengthTree.root();
                }
            }

            break;
        }

        case DecoderState::Data:
        {
            // Are we dealing with a compressed tree or an uncompressed tree?
            bool result;
            if (m_type == BlockType::Uncompressed)
                result = processUncompressed();
            else
                result = processCompressed();

            if (!result)
                return false;

            break;
        }

        case DecoderState::End:
            // Nothing left in the block.
            return false;
    }

    // We processed some bits.
    return true;
}


bool BlockDecoder::processUncompressed()
{
    // Determine how much data is available to be copied.
    auto available = m_input.bytesAvailable();
    auto length = std::min<std::size_t>(available, m_uncompressedBytesRemaining);

    // Write the data to the output.
    writeOutput(m_input.bytes(length));
    m_input.skipBytes(length);
    m_uncompressedBytesRemaining -= length;

    if (m_uncompressedBytesRemaining == 0)
        finish();

    return length != 0;
}

  
} // namespace System::Compress::Deflate
