#include <System/HW/GPU/VGA/VGA.hh>


namespace System::HW::GPU::VGA
{


VGADevice::VGADevice() :
    VGADevice(vga_ports{})
{
}

VGADevice::VGADevice(const vga_ports& ports) :
    m_miscOutputWritePort(ports.misc_output_write),
    m_miscOutputReadPort(ports.misc_output_read),
    m_inputStatus0Port(ports.input_status_0),
    m_inputStatus1Port(ports.input_status_1),
    m_featureControlReadPort(ports.feature_control_read),
    m_featureControlWritePort(ports.feature_control_write),
    m_dacStatePort(ports.dac_state),
    m_attributeController(ports.attribute_address, ports.attribute_data_read, ports.attribute_data_write, ports.attribute_flipflop_reset),
    m_crtc(ports.crtc_address, ports.crtc_data),
    m_dac(),
    m_graphicsController(ports.graphics_control_address, ports.graphics_control_data),
    m_sequencer(ports.sequencer_address, ports.sequencer_data)
{
}

VGADevice::~VGADevice()
{
}


} // namespace System::HW::GPU::VGA
