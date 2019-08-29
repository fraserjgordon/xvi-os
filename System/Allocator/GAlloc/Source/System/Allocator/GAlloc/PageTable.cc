#include <System/Allocator/GAlloc/PageTable.hh>


namespace __GALLOC_NS_DECL
{


// Explicit instantations.
template class PageTable<0>;

// The single instance of the page table hierarchy.
TopPageTable* g_topPageTable = nullptr;


TopPageTable* topPageTable()
{
    return g_topPageTable;
}


} // namespace __GALLOC_NS_DECL
