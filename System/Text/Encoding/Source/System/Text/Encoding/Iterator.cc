#include <System/Text/Encoding/IteratorImpl.hh>


namespace System::Text::Encoding
{


void Iterator::iteratorIncrement(Iterator* i)
{
    IteratorImpl::fromIterator(i)->increment();
}

char32_t Iterator::iteratorRead(const Iterator* i)
{
    return IteratorImpl::fromIterator(i)->read();
}


} // namespace System::Text::Encoding
