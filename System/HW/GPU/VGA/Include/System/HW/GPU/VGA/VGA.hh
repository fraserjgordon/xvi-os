#ifndef __SYSTEM_HW_GPU_VGA_VGA_H
#define __SYSTEM_HW_GPU_VGA_VGA_H


#include <System/HW/GPU/VGA/AttributeController.hh>
#include <System/HW/GPU/VGA/CRTC.hh>
#include <System/HW/GPU/VGA/DAC.hh>
#include <System/HW/GPU/VGA/GraphicsController.hh>
#include <System/HW/GPU/VGA/Sequencer.hh>


namespace System::HW::GPU::VGA
{


class VGADevice
{
public:

    struct vga_ports
    {
        std::uint16_t   misc_output_read        = 0x03CC;
        std::uint16_t   misc_output_write       = 0x03C2;
        std::uint16_t   input_status_0          = 0x03C2;
        std::uint16_t   input_status_1          = 0x03DA;
        std::uint16_t   feature_control_read    = 0x03CA;
        std::uint16_t   feature_control_write   = 0x03DA;
        std::uint16_t   dac_state               = 0x03C7;

        std::uint16_t   attribute_address       = 0x03C0;
        std::uint16_t   attribute_data_read     = 0x03C1;
        std::uint16_t   attribute_data_write    = 0x03C0;
        std::uint16_t   attribute_flipflop_reset= 0x03DA;

        std::uint16_t   sequencer_address       = 0x03C4;
        std::uint16_t   sequencer_data          = 0x03C5;

        std::uint16_t   graphics_control_address= 0x03CE;
        std::uint16_t   graphics_control_data   = 0x03CF;

        std::uint16_t   crtc_address            = 0x03D4;
        std::uint16_t   crtc_data               = 0x03D5;
    };


    VGADevice();
    VGADevice(const vga_ports& ports);

    ~VGADevice();

    AttributeController& attributeController()
    {
        return m_attributeController;
    }

    CRTC& crtc()
    {
        return m_crtc;
    }

    DAC& dac()
    {
        return m_dac;
    }

    GraphicsController& graphicsController()
    {
        return m_graphicsController;
    }

    Sequencer& sequencer()
    {
        return m_sequencer;
    }

private:

    std::uint16_t   m_miscOutputWritePort;
    std::uint16_t   m_miscOutputReadPort;
    std::uint16_t   m_inputStatus0Port;
    std::uint16_t   m_inputStatus1Port;
    std::uint16_t   m_featureControlReadPort;
    std::uint16_t   m_featureControlWritePort;
    std::uint16_t   m_dacStatePort;

    AttributeController     m_attributeController;
    CRTC                    m_crtc;
    DAC                     m_dac;
    GraphicsController      m_graphicsController;
    Sequencer               m_sequencer;
};


} // namespace System::HW::GPU::VGA


#endif /* ifndef __SYSTEM_HW_GPU_VGA_VGA_H */
