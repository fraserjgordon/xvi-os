#include <System/HW/GPU/VGA/CRTC.hh>


namespace System::HW::GPU::VGA
{


CRTC::DisplayParameters CRTC::getDisplayParameters() const noexcept
{
    DisplayParameters params;

    // Get the horizontal parameters. They are (mostly) relatively simple.
    params.horizontalTotal = readRegister(Register::HorizontalTotal) + 5;
    params.horizontalDisplay = readRegister(Register::HorizontalDisplayEnableEnd) + 1;
    params.horizontalBlankingStart = readRegister(Register::HorizontalBlankingStart);
    params.horizontalRetraceStart = readRegister(Register::HorizontalRetraceStart);
    
    auto hbe = readRegister(Register::HorizontalBlankingEnd);
    auto hre = readRegister(Register::HorizontalRetraceEnd);

    params.horizontalBlankingEnd = (hbe & 0x1f) | ((hre & 0x80) >> 2);
    params.horizontalRetraceEnd = (hre & 0x1f);

    // Get the vertical parameters. Most of these have extra bits in the overflow register.
    auto vt = readRegister(Register::VerticalTotal);
    auto of = readRegister(Register::Overflow);
    auto msl = readRegister(Register::MaximumScanLineAddress);
    auto vrs = readRegister(Register::VerticalRetraceStart);
    auto vre = readRegister(Register::VerticalRetraceEnd);
    auto vde = readRegister(Register::VerticalDisplayEnableEnd);
    auto vbs = readRegister(Register::VerticalBlankingStart);
    auto vbe = readRegister(Register::VerticalBlankingEnd);

    params.verticalTotal = vt | ((of & 0x01) << 8) | ((of & 0x20) << 4);
    params.verticalRetraceStart = vrs | ((of & 0x04) << 6) | ((of & 0x80) << 2);
    params.verticalRetraceEnd = (vre & 0x0f);
    params.verticalDisplay = (vde | ((of & 0x02) << 7) | ((of & 0x40) << 3)) + 1;
    params.verticalBlankingStart = vbs | ((of & 0x08) << 5) | ((msl & 0x20) << 4);
    params.verticalBlankingEnd = (vbe & 0x7f);

    return params;
}

CRTC::ScanOutParameters CRTC::getScanOutParameters() const noexcept
{
    ScanOutParameters params;

    params.linesPerCharacter = (readRegister(Register::MaximumScanLineAddress) & 0x1f) + 1;
    params.charactersPerRow = readRegister(Register::Offset) * 2;
    params.clockDivisor = 0;
    params.addressShift = 0;

    return params;
}

void CRTC::setStartAddress(std::uint16_t start) noexcept
{
    writeRegister(Register::StartAddressHigh, (start >> 8) & 0xff);
    writeRegister(Register::StartAddressLow, start & 0xff);
}


} // namespace System::HW::GPU::VGA
