#include <System/ABI/C++/Unwind/Unwinder.hh>

#include <System/ABI/Dwarf/Arch.hh>
#include <System/ABI/Dwarf/CFI.hh>
#include <System/ABI/Dwarf/Decode.hh>
#include <System/ABI/Dwarf/FDE.hh>
#include <System/ABI/ExecContext/ExecContext.h>
#include <System/C++/Containers/Vector.hh>
#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Array.hh>
#include <System/C++/Utility/Pair.hh>
#include <System/C++/Utility/UniquePtr.hh>


namespace System::ABI::CXX
{


class DwarfUnwinder final :
    public Unwinder
{
public:

    class DwarfContext;
    class DwarfException;

    _Unwind_Reason_Code raise(_Unwind_Exception*) final;
    _Unwind_Reason_Code forcedUnwind(_Unwind_Exception*, _Unwind_Stop_Fn, void*) final;
    _Unwind_Reason_Code backtrace(_Unwind_Trace_Fn, void*) final;

    void* findEnclosingFunction(void* where) final
    {
        // Find the FDE for the given address.
        auto fde = findFDE(reinterpret_cast<std::uintptr_t>(where));
        if (!fde)
            return nullptr;

        // Return the start of the address range covered by the FDE.
        return reinterpret_cast<void*>(fde.getCodeRangeStart());
    }

    bool registerEhFrameHdr(const std::byte*, std::uintptr_t text, std::uintptr_t data);


    static DwarfUnwinder* dwarfInstance()
    {
        return &s_dwarfInstance;
    }

protected:

    void resume(Exception*) final;

private:

    // Statically-allocated memory for unwind table registrations.
    static constexpr std::size_t StaticUnwindRegistrationSlots  = 16;

    // Information required to register unwind data.
    struct unwind_registration_t
    {
        // Bounds of the memory covered by this registration.
        std::uintptr_t  lowAddress  = 0;
        std::uintptr_t  highAddress = 0;

        const std::byte*    ehFrame     = nullptr;      // The .eh_frame section containing the unwind information.
        const std::byte*    ehFrameHdr  = nullptr;      // The .eh_frame_hdr section containing the binary search table.
        const void*         searchTable = nullptr;      // Binary search able for fast unwind lookup.
        std::size_t         tableSize   = 0;            // Number of entries in the search table.
        std::uintptr_t      textBase    = 0;            // Base address for text-relative pointers.
        std::uintptr_t      dataBase    = 0;            // Base address for data-relative pointers.
        std::uint8_t        encoding    = Dwarf::kDwarfPtrOmit; // Encoding of entries in the search table.
    };

    // Binary search table entries and a comparator object for them.
    template <class Encoded> struct eh_frame_bst_entry_t;
    template <class Encoded> struct eh_frame_bst_comparator_t;


    // Space reserved for unwind registrations without resorting to dynamic allocation.
    std::array<unwind_registration_t, StaticUnwindRegistrationSlots> m_staticSlots;


    // Finds the first registration (if any) for the given code address.
    const unwind_registration_t* findRegistration(std::uintptr_t code_address) const;

    // Finds the FDE data (if any) for the given code address.
    Dwarf::DwarfFDE findFDE(std::uintptr_t code_address) const;

    // Scans an .eh_frame section (or any data in the same format) for an FDE.
    Dwarf::DwarfFDE scanEhFrameSection(const unwind_registration_t& registration, std::uintptr_t code_address) const;

    // Runs unwind phase 2 (forced or otherwise) on the given exception.
    _Unwind_Reason_Code unwindPhase2(_Unwind_Exception* exception);

    // Updates the FDE stored within the context after an unwind.
    bool updateContextFDE(DwarfContext&);

    // Unwinds a frame by applying the rules from the FDE.
    bool unwindFrame(const Dwarf::DwarfFDE&, DwarfContext&);


    static DwarfUnwinder s_dwarfInstance;
};

DwarfUnwinder DwarfUnwinder::s_dwarfInstance = {};


class DwarfUnwinder::DwarfContext final :
    public Unwinder::Context
{
public:

    using reg_storage_t = Dwarf::FrameTraitsNative::reg_storage_t;


    DwarfContext(const Dwarf::FrameTraitsNative::reg_storage_t& frame) :
        m_registers(frame)
    {
    }

    ~DwarfContext() final;


    Unwinder*     unwinder() final
    {
        return DwarfUnwinder::dwarfInstance();
    }

    std::uintptr_t getGR(int reg) final;
    void           setGR(int reg, std::uintptr_t value) final;

    std::uintptr_t getIP() final;
    void           setIP(std::uintptr_t value) final;

    std::uintptr_t getLSDA() final
    {
        return reinterpret_cast<std::uintptr_t>(m_fde.getLSDA());
    }

    std::uintptr_t getRegionStart() final
    {
        return m_fde.getCodeRangeStart();
    }

    std::uintptr_t getDataRelBase() final
    {
        return m_fde.getDataBase();
    }

    std::uintptr_t getTextRelBase() final
    {
        return m_fde.getTextBase();
    }

    std::uintptr_t getCFA() final
    {
        return m_registers.GetCFA();
    }

    std::uintptr_t getIPInfo(int*) final;

    bool isForced() final
    {
        return (m_stopfunc != nullptr);
    }


    // Like setIP but doesn't do extra work implicitly.
    void setIPFromUnwind(std::uintptr_t address);

    Dwarf::FrameTraitsNative::reg_storage_t& registers()
    {
        return m_registers;
    }

    const Dwarf::DwarfFDE& getFDE() const
    {
        return m_fde;
    }

    void setFDE(const Dwarf::DwarfFDE& fde)
    {
        m_fde = fde;
    }

    void setStopFunc(_Unwind_Stop_Fn stopfunc, void* stopparam)
    {
        m_stopfunc = stopfunc;
        m_stopparam = stopparam;
    }

    _Unwind_Reason_Code callStopFunction(_Unwind_Action actions, _Unwind_Exception* exception)
    {
        return m_stopfunc(1, actions, exception->exception_class, exception, this, m_stopparam);
    }

    void setArgsSize(std::uintptr_t size)
    {
        m_argsSize = size;
    }

private:

    // FDE associated with the current instruction pointer.
    Dwarf::DwarfFDE m_fde       = {};

    reg_storage_t m_registers   = {};

    // Argument size adjustment to apply to the stack pointer when a personality or landingpad sets the IP.
    std::ptrdiff_t  m_argsSize  = 0;

    // IF non-null, this is a forced unwind using this stop function.
    _Unwind_Stop_Fn m_stopfunc  = nullptr;
    void*           m_stopparam = nullptr;
};


class DwarfUnwinder::DwarfException final :
    public Exception
{
public:

    std::unique_ptr<DwarfContext> takeContext()
    {
        return std::unique_ptr<DwarfContext>{static_cast<DwarfContext*>(Exception::takeContext().release())};
    }

    static DwarfException* from(_Unwind_Exception* e)
    {
        return static_cast<DwarfException*>(Exception::from(e));
    }
};


_Unwind_Reason_Code DwarfUnwinder::raise(_Unwind_Exception* raw_exception)
{
    // Capture the current execution state.
    ExecContext::frame_t raw_frame;
    ExecContext::CaptureContext(&raw_frame, 0);

    // Wrap the exec context into a DWARF frame object.
    Dwarf::FrameTraitsNative::reg_storage_t dwarf_frame;
    dwarf_frame.CaptureFrame(raw_frame);

    // And wrap that DWARF frame into a context object. Note that the new operator for DwarfContext is noexcept so the
    // return value needs to be explicitly checked.
    std::unique_ptr context = std::make_unique<DwarfContext>(dwarf_frame);
    if (!context)
        return _URC_FATAL_PHASE1_ERROR;
    auto unwind_context = reinterpret_cast<_Unwind_Context*>(context.get());

    // Wrap the exception too.
    auto exception = Exception::from(raw_exception);

    // The CFA of the frame that will handle the exception.
    std::uintptr_t handler_cfa = 0;

    // Find the FDE covering the current instruction pointer. We subtract one from the pointer to make sure that it
    // always points within the function.
    auto pc = dwarf_frame.GetReturnAddress() - 1;
    context->setFDE(findFDE(pc));
    auto& starting_fde = context->getFDE();
    if (!starting_fde)
        return _URC_FATAL_PHASE1_ERROR; // Failed to find unwind info.

    // Unwind by a single frame (this will ensure the context has a valid CFA).
    if (!unwindFrame(starting_fde, *context))
        return _URC_FATAL_PHASE1_ERROR;

    // And update the FDE for the frame.
    if (!updateContextFDE(*context))
        return _URC_FATAL_PHASE1_ERROR;

    // Unwind phase 1: search for a handler.
    bool cleanup_needed;
    while (true)
    {
        // Check for the end of the stack.
        if (context->registers().GetCFA() == 0)
            return _URC_END_OF_STACK;

        // Find the personality routine specified in the FDE.
        auto& fde = context->getFDE();
        auto& cie = fde.getCIE();
        using personality_fn = _Unwind_Reason_Code (*)(int, _Unwind_Action, std::uint64_t, _Unwind_Exception*, _Unwind_Context*);
        auto personality = reinterpret_cast<personality_fn>(const_cast<void*>(cie.getPersonalityRoutine()));

        // Call the personality routine (if any) to find out if it wants to handle this exception.
        if (personality)
        {
            auto personality_result = personality(1, _UA_SEARCH_PHASE, exception->header()->exception_class, exception->header(), unwind_context);
            switch (personality_result)
            {
                case _URC_HANDLER_FOUND:
                    // The personality routine wants to handle the exception. Record this information.
                    handler_cfa = context->getCFA();
                    break;

                case _URC_CONTINUE_UNWIND:
                    // We will need to continue unwinding.
                    //
                    // As we'll need to call this personality routine in phase 2, we can't skip re-unwinding.
                    cleanup_needed = true;
                    break;

                default:
                    // The personality routine returned something unexpected. We can't go on.
                    return _URC_FATAL_PHASE1_ERROR;
            }
        }

        // Have we found a handler?
        if (handler_cfa != 0)
            break;

        // Otherwise, virtually unwind (i.e only the context is unwound; the real stack is left unmodified).
        if (!unwindFrame(fde, *context))
            return _URC_FATAL_PHASE1_ERROR;

        // And update the FDE for the frame.
        if (!updateContextFDE(*context))
            return _URC_FATAL_PHASE1_ERROR;
    }

    
    // If we don't need to perform any cleanup actions before the handler frame, we can skip re-unwinding during the
    // second phase. Otherwise, restore the context to the original state.
    if (cleanup_needed)
    {
        dwarf_frame.CaptureFrame(raw_frame);
        *context = DwarfContext(dwarf_frame);
        updateContextFDE(*context);
    }

    // Phase 1 of the search has completed successfully. Record the information that we've found.
    exception->setContext(std::move(context));
    exception->setHandlerCFA(handler_cfa);

    // Run phase 2 (the actual unwind).
    return unwindPhase2(raw_exception);
}

_Unwind_Reason_Code DwarfUnwinder::forcedUnwind(_Unwind_Exception* raw_exception, _Unwind_Stop_Fn stopfunc, void* stopparam)
{
    // Capture the current execution state.
    ExecContext::frame_t raw_frame;
    ExecContext::CaptureContext(&raw_frame, 0);

    // Wrap the exec context into a DWARF frame object.
    Dwarf::FrameTraitsNative::reg_storage_t frame;
    frame.CaptureFrame(raw_frame);

    // And wrap that DWARF frame into a context object.
    auto context = std::make_unique<DwarfContext>(frame);

    // Wrap the exception too.
    auto exception = Exception::from(raw_exception);
    exception->setContext(std::move(context));

    // Find the FDE covering the current instruction pointer. We subtract one from the pointer to make sure that it
    // always points within the function.
    auto pc = frame.GetReturnAddress() - 1;
    context->setFDE(findFDE(pc));
    auto& fde = context->getFDE();
    if (!fde)
        return _URC_FATAL_PHASE2_ERROR; // Failed to find unwind info.

    // Unwind by a single frame (this will ensure the context has a valid CFA).
    if (!unwindFrame(fde, *context))
        return _URC_FATAL_PHASE2_ERROR;

    // And update the FDE for the frame.
    if (!updateContextFDE(*context))
        return _URC_FATAL_PHASE2_ERROR;

    // Store the stop function into the context.
    context->setStopFunc(stopfunc, stopparam);

    // Forced unwinds don't have a phase 1 (the search phase) - they proceed immediately to phase 2 (the unwind phase).
    return unwindPhase2(raw_exception);
}

_Unwind_Reason_Code DwarfUnwinder::backtrace(_Unwind_Trace_Fn trace, void* trace_param)
{
    // Capture the current execution state.
    ExecContext::frame_t raw_frame;
    ExecContext::CaptureContext(&raw_frame, 0);

    // Wrap the exec context into a DWARF frame object.
    Dwarf::FrameTraitsNative::reg_storage_t dwarf_frame;
    dwarf_frame.CaptureFrame(raw_frame);

    // And wrap that DWARF frame into a context object.
    // Note: allocated on the stack as we're not really going to unwind.
    auto context = DwarfContext(dwarf_frame);
    auto unwind_context = reinterpret_cast<_Unwind_Context*>(&context);

    // Find the FDE covering the current instruction pointer. We subtract one from the pointer to make sure that it
    // always points within the function.
    auto pc = dwarf_frame.GetReturnAddress() - 1;
    auto starting_fde = findFDE(pc);
    if (!starting_fde)
        return _URC_FATAL_PHASE1_ERROR; // Failed to find unwind info.

    // Unwind by a single frame (this will ensure the context has a valid CFA).
    if (!unwindFrame(starting_fde, context))
        return _URC_FATAL_PHASE1_ERROR;

    // And update the FDE for the frame.
    if (!updateContextFDE(context))
        return _URC_FATAL_PHASE1_ERROR;

    // Unwind phase 1: search for a handler.
    while (true)
    {
        // Check for the end of the stack.
        if (context.registers().GetCFA() == 0)
            break;

        // Call the trace function.
        auto result = trace(unwind_context, trace_param);
        if (result != _URC_CONTINUE_UNWIND)
            return result;

        // Next, virtually unwind (i.e only the context is unwound; the real stack is left unmodified).
        if (!unwindFrame(context.getFDE(), context))
            return _URC_FATAL_PHASE1_ERROR;

        // And update the FDE for the frame.
        if (!updateContextFDE(context))
            return _URC_FATAL_PHASE1_ERROR;
    }

    // Backtrace complete.
    return _URC_END_OF_STACK;
}

void DwarfUnwinder::resume(Exception* e)
{
    // Resume works by continuing with phase 2 of the unwind.
    unwindPhase2(e->header());
}


template <class E>
struct DwarfUnwinder::eh_frame_bst_entry_t
{
    E code_address;
    E fde_address;
};

template <class E>
struct DwarfUnwinder::eh_frame_bst_comparator_t
{
    // Base addresses for text- or data-relative encodings.
    std::uintptr_t textBase = 0;
    std::uintptr_t dataBase = 0;

    std::uint8_t encoding;

    constexpr auto operator()(const eh_frame_bst_entry_t<E>& x, std::uintptr_t y) const
    {
        using namespace Dwarf;
        
        auto decoder = DwarfPointerDecoderNative{{.text = textBase, .data = dataBase}};
        auto code_address_x = decoder.decodeVoid(encoding, &x.code_address);

        return code_address_x < y;
    }

    constexpr auto operator()(std::uintptr_t x, const eh_frame_bst_entry_t<E>& y) const
    {
        using namespace Dwarf;
        
        auto decoder = DwarfPointerDecoderNative{{.text = textBase, .data = dataBase}};
        auto code_address_y = decoder.decodeVoid(encoding, &y.code_address);

        return x < code_address_y;
    }
};


bool DwarfUnwinder::registerEhFrameHdr(const std::byte* ptr, std::uintptr_t text_base, std::uintptr_t data_base)
{
    // Parse the header so we can locate the relevant data.
    auto eh_frame_hdr = ptr;
    auto version = static_cast<std::uint8_t>(*ptr++);
    if (version != 1)
    {
        // We don't know how to parse this header.
        return false;
    }

    // Get the DWARF encoding for the various fields in this header.
    auto ptr_enc = static_cast<std::uint8_t>(*ptr++);
    auto count_enc = static_cast<std::uint8_t>(*ptr++);
    auto table_enc = static_cast<std::uint8_t>(*ptr++);
    Dwarf::DwarfPointerDecoderNative decoder = {{.text = text_base, .data = data_base}};
    
    // Get the address of the .eh_frame section this header contains a search table for.
    auto eh_frame_address = decoder.decode(ptr_enc, ptr);

    // Get the number of entries in the search table.
    auto entry_count = decoder.decode(count_enc, ptr);

    // We have all the information we need to register this unwind data.
    //! @TODO: implement the bounding addresses (parse the EH search table?)
    unwind_registration_t registration =
    {
        .lowAddress = 0,
        .highAddress = ~std::uintptr_t(0),
        .ehFrame = reinterpret_cast<const std::byte*>(eh_frame_address),
        .ehFrameHdr = eh_frame_hdr,
        .searchTable = ptr,
        .tableSize = entry_count,
        .textBase = text_base,
        .dataBase = data_base,
        .encoding = table_enc,
    };

    // If there is room in the static entry list, insert the table there.
    //! @TODO: locking.
    for (auto& entry : m_staticSlots)
    {
        // Use this slot if it has no valid unwind data.
        if (entry.ehFrame == nullptr)
        {
            entry = registration;
            return true;
        }
    }

    //! @TODO: allocate a dynamic entry.
    return false;
}

const DwarfUnwinder::unwind_registration_t* DwarfUnwinder::findRegistration(std::uintptr_t address) const
{
    // Search the static entries first.
    //! @TODO: locking.
    for (const auto& reg : m_staticSlots)
    {
        if (reg.lowAddress <= address && address < reg.highAddress)
            return &reg;
    }

    // TODO: search dynamic entries.
    return nullptr;
}

Dwarf::DwarfFDE DwarfUnwinder::findFDE(std::uintptr_t address) const
{
    using namespace Dwarf;    

    // First, find the registration for the code.
    auto registration = findRegistration(address);
    if (!registration)
        return {};

    // Perform a linear scan of the .eh_frame section for the FDE.
    if (registration->encoding == kDwarfPtrOmit)
        return scanEhFrameSection(*registration, address);

    // Table searching function.
    auto search = [registration, address]<class E>(E)
    {
        // Search for the entry greater-than-or-equal-to the target.
        //
        // Note that data-relative pointers are relative to the .eh_frame_hdr section!
        auto textBase = registration->textBase;
        auto dataBase = reinterpret_cast<std::uintptr_t>(registration->ehFrameHdr);
        auto encoding = registration->encoding;
        auto begin = reinterpret_cast<const eh_frame_bst_entry_t<E>*>(registration->searchTable);
        auto end = begin + registration->tableSize;
        auto comparator = eh_frame_bst_comparator_t<E>{.textBase = textBase, .dataBase = dataBase, .encoding = encoding};
        auto entry = std::lower_bound(begin, end, address, comparator);

        // Go back one if we're beyond the target address.
        DwarfPointerDecoderNative decoder {{.text = textBase, .data = dataBase}};
        auto entry_address = decoder.decodeVoid(encoding, &entry->code_address);
        if (entry != begin && entry_address > address)
            entry -= 1;

        return decoder.decodeVoid(encoding, &entry->fde_address);
    };

    // What encoding is used for the table?
    std::uintptr_t fde_address = 0;
    switch (dw_ptr_type(registration->encoding & kDwarfPtrTypeMask))
    {
        case dw_ptr_type::udata2:
        case dw_ptr_type::sdata2:
            fde_address = search(std::uint16_t{});
            break;

        case dw_ptr_type::udata4:
        case dw_ptr_type::sdata4:
            fde_address = search(std::uint32_t{});
            break;

        case dw_ptr_type::udata8:
        case dw_ptr_type::sdata8:
            fde_address = search(std::uint64_t{});
            break;

        default:
            // Unsupported encoding (needs to be fixed-size!)
            break;
    }

    // If we failed to find an entry, fall back to a linear scan.
    if (fde_address == 0)
        return scanEhFrameSection(*registration, address);

    // Decode the FDE.
    auto fde_ptr = reinterpret_cast<const std::byte*>(fde_address);
    auto fde = DwarfCieOrFde::DecodeFrom(fde_ptr, registration->textBase, registration->dataBase, nullptr);

    // Check that the FDE is valid and covers the target address.
    if (!fde.isValid() || !fde.is_fde)
        return {};

    auto fde_start = fde.fde.getCodeRangeStart();
    auto fde_end = fde_start + fde.fde.getCodeRangeSize();
    if (!(fde_start <= address && address < fde_end))
        return {};

    // Target FDE has been found.
    return fde.fde;
}

Dwarf::DwarfFDE DwarfUnwinder::scanEhFrameSection(const unwind_registration_t& registration, std::uintptr_t address) const
{
    // Start decoding the entries in the section. The section is terminated by an invalid CIE (length == 0).
    auto ptr = registration.ehFrame;
    auto entry = Dwarf::DwarfCieOrFde::DecodeFrom(ptr, registration.textBase, registration.dataBase, nullptr);
    while (entry.isValid())
    {
        // Is this an FDE or CIE?
        if (entry.is_fde)
        {
            // Does this FDE cover the code range?
            const auto& fde = entry.fde;
            auto start = fde.getCodeRangeStart();
            auto end = start + fde.getCodeRangeSize();
            if (start <= address && address < end)
                return fde;

            // The sections are sorted so if we've passed the target address, no need to keep going.
            if (start > address)
                break;
        }

        // Get the next entry.
        ptr += entry.getLength();
        entry = Dwarf::DwarfCieOrFde::DecodeFrom(ptr, registration.textBase, registration.dataBase, nullptr);
    }

    // End of the section reached without finding an appropriate entry.
    return {};
}

_Unwind_Reason_Code DwarfUnwinder::unwindPhase2(_Unwind_Exception* raw_exception)
{
    // Extract the context and target CFA from the exception.
    auto* exception = DwarfException::from(raw_exception);
    auto  handler_cfa = exception->handlerCFA();

    // Unique pointer used to force cleanup of the context if we return an error.
    auto context = exception->takeContext();
    if (!context) [[unlikely]]
        return _URC_FATAL_PHASE2_ERROR;

    // The current register state.
    auto& frame = context->registers();

    // Actions to be taken by each personality routine as we call it.
    _Unwind_Action actions = _UA_CLEANUP_PHASE;
    if (context->isForced())
        actions |= _UA_FORCE_UNWIND;
    
    while (true)
    {
        // Have we reached the end of the stack?
        if (frame.GetCFA() == 0)
        {
            // If this is a forced unwind, give it a chance to handle this condition.
            if (context->isForced())
            {
                // Note: this call might not return.
                return context->callStopFunction(actions, raw_exception);
            }

            // This should not have happened for non-forced phase 2 unwinding as we successfully found a handler frame
            // during the phase 1 search. We've probably corrupted the stack...
            return _URC_FATAL_PHASE2_ERROR;
        }

        // The current FDE is cached within the context.
        auto& fde = context->getFDE();

        // Find the personality routine specified in the FDE.
        auto& cie = fde.getCIE();
        using personality_fn = _Unwind_Reason_Code (*)(int, _Unwind_Action, std::uint64_t, _Unwind_Exception*, _Unwind_Context*);
        auto personality = reinterpret_cast<personality_fn>(const_cast<void*>(cie.getPersonalityRoutine()));

        // If this is a forced unwind, call the stop function.
        if (context->isForced())
        {
            // Note: this might not return - if this is the destination frame, the stop function is permitted to jump to
            // whatever handling code it likes without returning to the unwinder. Any cleanup should happen when the
            // stop function calls _Unwind_DeleteException.
            auto result = context->callStopFunction(actions, raw_exception);
            switch (result)
            {
                case _URC_NO_REASON:
                    // This isn't the target frame, unwind normally.
                    break;

                case _URC_END_OF_STACK:
                case _URC_FATAL_PHASE2_ERROR:
                default:
                    // The error codes above are permitted to be returned but there's nothing we can do about them if we
                    // receive them. The same goes for any other error code.
                    return _URC_FATAL_PHASE2_ERROR;
            }
        }

        // Is this the target frame?
        bool is_handler = (handler_cfa == context->getCFA());
        if (is_handler)
            actions |= _UA_HANDLER_FRAME;

        // Call the personality routine (if any) to find out if it wants to handle this exception.
        if (personality)
        {
            auto personality_result = personality(1, actions, exception->header()->exception_class, exception->header(), context.get());
            switch (personality_result)
            {
                case _URC_CONTINUE_UNWIND:
                    // We will need to continue unwinding.
                    break;

                case _URC_INSTALL_CONTEXT:
                {
                    // The personality routine needs to transfer to a landing pad to do some cleanup. How we do this depends
                    // on whether this is the handler frame.
                    //
                    // Regardless, this method will now not return - it is going to jump to the landingpad installed by the
                    // personality routine.
                    auto registers = context->registers();
                    if (is_handler)
                    {
                        // We've found the handler frame. Processing of this exception is finished so we can free any memory
                        // that got allocated.
                        context.release();
                    }
                    else
                    {
                        // We're going to need to keep the context for a little while longer.
                        exception->setContext(std::move(context));
                    }

                    // From the context, create an execution context frame. We use a full frame here (i.e including
                    // registers that are usually not saved according to the ABI) as there is no requirement that the
                    // control transfer from the personality routine to the landingpad uses the normal calling convention.
                    // GCC's __gxx_personality_v0 routine, in particular, uses volatile registers to pass parameters to the
                    // landingpad.
                    ExecContext::full_frame_t landingpad_frame;
                    registers.ConfigureFullFrame(landingpad_frame);

                    // Jump to the landingpad. This call does not return.
                    ExecContext::ResumeContextFull(&landingpad_frame);
                    __builtin_unreachable();
                }

                default:
                    // The personality routine returned something unexpected. We can't go on.
                    return _URC_FATAL_PHASE2_ERROR;
            }
        }

        // Perform the unwind of this frame.
        if (!unwindFrame(fde, *context))
            return _URC_FATAL_PHASE2_ERROR;

        // And update the FDE for the frame.
        if (!updateContextFDE(*context))
            return _URC_FATAL_PHASE2_ERROR;
    }
}

bool DwarfUnwinder::updateContextFDE(DwarfContext& context)
{
    // As always, subtract one from the instruction pointer
    auto pc = context.registers().GetReturnAddress() - 1;
    auto fde = findFDE(pc);
    if (!fde)
        return false;

    context.setFDE(fde);
    return true;
}

bool DwarfUnwinder::unwindFrame(const Dwarf::DwarfFDE& fde, DwarfContext& context)
{
    // Copy of the frame containing the updated register values.
    auto& frame = context.registers();
    auto output = frame;
    
    // Decode the CFI instructions from the FDE to give us the register restoration rules.
    Dwarf::reg_rule_row_native rules;
    if (!Dwarf::ParseCFIRules(fde, frame.GetReturnAddress(), rules))
        return false;

    // Store the value, if any, of the gnu_args_size opcode.
    context.setArgsSize(rules.arguments_size);

    // Apply the CFA rule.
    using rule_type = Dwarf::reg_rule::type;
    std::uintptr_t cfa;
    switch (rules.cfa.rule_type)
    {
        case rule_type::other_reg:
        {
            // The CFA is equal to a register plus an offset.
            auto reg = Dwarf::FrameTraitsNative::IndexToRegister(rules.cfa.params.other_reg.reg);
            auto addend = rules.cfa.params.other_reg.addend;
            cfa = frame.GetGPRegister(reg) + addend;
            break;
        }

        case rule_type::expression:
        case rule_type::expression_value:
            // The CFA can be calculated by evaluating a DWARF expression.
            //! @TODO: implement.
            return false;

        default:
            // Unsupported rule type for the CFA.
            return false;
    }

    output.SetCFA(cfa);

    // Apply the rules for each of the other registers.
    //
    // Note: this loop deliberately accesses one past the end of the array of register rules in order to process the
    // return address rule too.
    for (std::size_t i = 0; i < rules.RegisterCount + 1; ++i)
    {
        bool is_ip = (i == rules.RegisterCount);
        auto& rule = (is_ip) ? rules.return_address : rules.registers[i];
        auto set = [&](std::uintptr_t value)
        {
            bool is_ip = (i == rules.RegisterCount);
            if (is_ip)
            {
                output.SetReturnAddress(value);
            }
            else
            {
                auto which = Dwarf::FrameTraitsNative::IndexToRegister(i);
                output.SetGPRegister(which, value);
            }
        };

        switch (rule.rule_type)
        {
            case rule_type::undefined:
                // There is no way to restore the value of this register. Zero it for consistency.
                set(0);
                break;

            case rule_type::unchanged:
                // The register contents are unchanged.
                break;

            case rule_type::cfa_relative:
            {
                // The register is stored an an offset from the CFA.
                auto where = cfa + rule.params.cfa_relative.offset;
                auto ptr = reinterpret_cast<std::uintptr_t*>(where);
                set(*ptr);
                break;
            }

            case rule_type::cfa_relative_value:
                // The register is equal to the CFA plus an offset.
                set(cfa + rule.params.cfa_relative.offset);
                break;

            case rule_type::other_reg:
            {
                // The register's value is held in another register.
                auto which = Dwarf::FrameTraitsNative::IndexToRegister(rule.params.other_reg.reg);
                auto addend = rule.params.other_reg.addend;
                set(frame.GetGPRegister(which) + addend);
                break;
            }

            case rule_type::expression:
                //! @TODO: implement.
                return false;

            case rule_type::expression_value:
                //! @TODO: implement.
                return false;

            default:
                // Unrecognised rule type.
                return false;
        }
    }

    // All rules were processed successfully. Update the frame.
    frame = output;

    //! @TODO: return the arguments size value (if any).
    return true;
}


DwarfUnwinder::DwarfContext::~DwarfContext()
{
}

std::uintptr_t DwarfUnwinder::DwarfContext::getGR(int reg)
{
    auto which = Dwarf::FrameTraitsNative::IndexToRegister(reg);
    return m_registers.GetGPRegister(which);
}

void DwarfUnwinder::DwarfContext::setGR(int reg, std::uintptr_t value)
{
    auto which = Dwarf::FrameTraitsNative::IndexToRegister(reg);

    if (which == Dwarf::FrameTraitsNative::kInstructionPointerReg)
    {
        // The instruction pointer needs special handling.
        setIP(value);
        return;
    }

    m_registers.SetGPRegister(which, value);
}

std::uintptr_t DwarfUnwinder::DwarfContext::getIP()
{
    return m_registers.GetReturnAddress();
}

void DwarfUnwinder::DwarfContext::setIP(std::uintptr_t ip)
{
    // When setting the instruction pointer from a landing pad, any gnu_args_size opcode needs to be used to adjust the
    // stack pointer.
    constexpr auto SP = Dwarf::FrameTraitsNative::kStackPointerReg;
    constexpr auto Direction = Dwarf::FrameTraitsNative::kStackDirection;
    if (m_argsSize != 0)
    {
        m_registers.SetGPRegister(SP, m_registers.GetGPRegister(SP) + Direction * m_argsSize);
    }

    setIPFromUnwind(ip);
}

void DwarfUnwinder::DwarfContext::setIPFromUnwind(std::uintptr_t ip)
{
    // When the instruction pointer is updated, the cached FDE also needs to be updated.
    m_fde = DwarfUnwinder::dwarfInstance()->findFDE(ip - 1);
    m_argsSize = 0;

    // Set the new instruction pointer.
    m_registers.SetReturnAddress(ip);
}

std::uintptr_t DwarfUnwinder::DwarfContext::getIPInfo(int* ip_before)
{
    // For all frames except signal handler frames, the instruction pointer points after the faulting instruction. For
    // signal frames, however, it points before the instruction.
    bool is_signal = m_fde.getCIE().isSignalFrame();
    *ip_before = is_signal;
    return getIP();
}


#ifdef __SYSTEM_ABI_CXX_UNWIND_DWARF
Unwinder* Unwinder::instance()
{
    return DwarfUnwinder::dwarfInstance();
}
#endif


} // namespace System::ABI::CXX


void _Unwind_XVI_RegisterEhFrameHdr(const std::byte* ptr, std::uintptr_t text_base, std::uintptr_t data_base)
{
    System::ABI::CXX::DwarfUnwinder::dwarfInstance()->registerEhFrameHdr(ptr, text_base, data_base);
}
