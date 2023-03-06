#ifndef __SYSTEM_IO_HANDLE_HANDLE_H
#define __SYSTEM_IO_HANDLE_HANDLE_H


namespace System::IO::Handle
{


class Handle
{
protected:

    class Impl;
    struct Ops;


    Impl*       m_pImpl = nullptr;
};


struct Handle::Ops
{

};


} // namespace System::IO::Handle


#endif /* ifndef __SYSTEM_IO_HANDLE_HANDLE_H */
