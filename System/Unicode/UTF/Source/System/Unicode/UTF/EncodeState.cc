#include <System/Unicode/UTF/EncodeState.hh>

#include <System/Unicode/UTF/EncodeStateInternal.hh>    
#include <System/Unicode/UTF/UTFInternal.hh>


namespace System::Unicode::UTF
{


bool EncodeStateIsInitial(const encode_state* state)
{
    return EncodeStateInternal::from(state)->isInitialState();
}


} // namespace System::Unicode::UTF
