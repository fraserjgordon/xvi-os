#ifndef __SYSTEM_FIRMWARE_DEVICETREE_TREE_H
#define __SYSTEM_FIRMWARE_DEVICETREE_TREE_H


#include <System/Firmware/DeviceTree/Private/Config.hh>

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <system_error>


//! @TODO: move status codes etc to a separate header.


namespace System::Firmware::DeviceTree
{


using tree_handle   = struct opaque_tree*;
using node_handle   = struct opaque_node*;

enum class ErrorCode : std::uint32_t 
{
    OK      = 0,
};


__SYSTEM_FIRMWARE_DEVICETREE_EXPORT tree_handle createTree() __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(CreateTree);
__SYSTEM_FIRMWARE_DEVICETREE_EXPORT void        deleteTree() __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(DeleteTree);
__SYSTEM_FIRMWARE_DEVICETREE_EXPORT ErrorCode   importFDT(tree_handle, const std::byte*, std::size_t) __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(ImportFDT);

__SYSTEM_FIRMWARE_DEVICETREE_EXPORT node_handle copyNodeHandle(node_handle) __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(CopyNodeHandle);
__SYSTEM_FIRMWARE_DEVICETREE_EXPORT void        releaseNodeHandle(node_handle) __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(ReleaseNodeHandle);
__SYSTEM_FIRMWARE_DEVICETREE_EXPORT node_handle rootNode(tree_handle) __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(RootNode);
__SYSTEM_FIRMWARE_DEVICETREE_EXPORT node_handle getChild(node_handle, const char*, std::size_t = std::string_view::npos) __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(GetChild);


} // namespace System::Firmware::DeviceTree


#endif /* ifndef __SYSTEM_FIRMWARE_DEVICETREE_H */
