#include <Tools/Development/Test/Test.hh>

#include <System/Unicode/UTF/UTF-8.hh>


using namespace System::Unicode::UTF;
using namespace System::Unicode::UTF::UTF8;


TEST(UTF8, IteratorAPI)
{
    EXPECT((std::input_iterator<utf8_input_iterator>));
    EXPECT((std::input_iterator<utf8_nul_terminated_input_iterator>));
    EXPECT((!std::sentinel_for<utf8_nul_terminated_input_sentinel, utf8_input_iterator>));
    EXPECT((std::sentinel_for<utf8_nul_terminated_input_sentinel, utf8_nul_terminated_input_iterator>));

    EXPECT((std::ranges::input_range<utf8_input_view>));
    EXPECT((std::ranges::input_range<utf8_nul_terminated_input_view>));
}
