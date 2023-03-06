#ifndef __SYSTEM_IO_FILEIO_PATHANDLE_H
#define __SYSTEM_IO_FILEIO_PATHANDLE_H


#include <filesystem>

#include <System/IO/FileIO/Handle.hh>
#include <System/IO/FileIO/Types.hh>


namespace System::IO::FileIO
{


class PathHandle :
    public Handle
{
public:

    constexpr PathHandle() = default;
    PathHandle(const PathHandle&) = delete;
    constexpr PathHandle(PathHandle&&) = default;

    ~PathHandle() override = default;

    PathHandle& operator=(const PathHandle&) = delete;
    constexpr PathHandle& operator=(PathHandle&&) = default;


    PathHandle(const std::filesystem::path&);
    PathHandle(const PathHandle&, const std::filesystem::path&);


    template <std::derived_from<PathHandle> T> PathHandle(T&&) = delete;
    template <std::derived_from<PathHandle> T> void operator=(T&&) = delete;


    result<PathHandle> clone() const noexcept;


    static result<PathHandle> open(const path_type&) noexcept;
    static result<PathHandle> open(const PathHandle&, const path_type&) noexcept;

private:

    explicit PathHandle(int fd) noexcept;
};


using SharedPathHandle = SharedHandle<PathHandle>;
using UniquePathHandle = UniqueHandle<PathHandle>;


} // namespace System::IO::FileIO


#endif /* ifndef __SYSTEM_IO_FILEIO_PATHHANDLE_H */
