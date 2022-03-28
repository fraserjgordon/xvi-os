#define __XVI_STD_FORMAT_HEADER_ONLY

#include <System/ABI/C++/Demangle.hh>

#include <System/ABI/String/String.hh>

#include <System/C++/Containers/Vector.hh>
#include <System/C++/Format/Format.hh>
#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/Utility/Memory.hh>
#include <System/C++/Utility/Optional.hh>
#include <System/C++/Utility/String.hh>
#include <System/C++/Utility/StringView.hh>
#include <System/C++/Utility/Variant.hh>


extern "C" [[gnu::weak]] void* __realloc(void*, std::size_t) asm("System.Allocator.C.Realloc");


namespace System::ABI::CXX
{


namespace
{


// Defined by the C++ ABI but not given any symbolic name.
static constexpr int DemangleSuccess = 0;
static constexpr int DemangleMemoryAllocFailure = -1;
static constexpr int DemangleInvalidName = -2;
static constexpr int DemangleInvalidArguments = -3;


struct mangled_name;
struct encoding;
struct name;
struct unscoped_name;
struct unscoped_template_name;
struct nested_name;
struct prefix;
struct template_prefix;
struct unqualified_name;
struct source_name;
struct identifier;
struct abi_tag;
struct number;
struct seq_id;
struct operator_name;
struct special_name;
struct call_offset;
struct nv_offset;
struct v_offset;
struct ctor_dtor_name;
struct type;
struct qualified_type;
struct qualifiers;
struct extended_qualifiers;
struct cv_qualifiers;
struct ref_qualifier;
struct builtin_type;
struct function_type;
struct bare_function_type;
struct exception_spec;
struct decltype_expr;
struct class_enum_type;
struct unnamed_type_name;
struct array_type;
struct pointer_to_member_type;
struct template_param;
struct template_template_param;
struct function_param;
struct template_args;
struct template_arg;
struct expression;
struct unresolved_name;
struct unresolved_type;
struct unresolved_qualifier_level;
struct simple_id;
struct base_unresolved_name;
struct destructor_name;
struct expr_primary;
struct braced_expression;
struct initializer;
struct local_name;
struct discriminator;
struct closure_type_name;
struct lambda_sig;
struct local_name;
struct data_member_prefix;
struct substitution;


using substitution_table = std::vector<std::pair<std::size_t, std::size_t>>;


enum class builtin_type_t
{
    _void,
    _wchar_t,
    _bool,
    _char,
    _signed_char,
    _unsigned_char,
    _short,
    _unsigned_short,
    _int,
    _unsigned_int,
    _long,
    _unsigned_long,
    _llong,
    _unsigned_llong,
    _int128,
    _unsigned_int128,
    _float,
    _double,
    _long_double,
    _float128,
    _ellipsis,
    _decimal32,
    _decimal64,
    _decimal128,
    _half_float,
    _floatN,
    _char16_t,
    _char32_t,
    _auto,
    _decltype_auto,
    _nullptr_t,
    _vendor,
};

enum class ref_qualifier_t : char
{
    lvalue      = 'R',
    rvalue      = 'O',
};

enum class substitution_t
{
    ref,
    std,
    std_allocator,
    std_basic_string,
    std_string,
    std_istream,
    std_ostream,
    std_iostream,
};

enum class operator_name_t
{
    _new,
    _new_array,
    _delete,
    _delete_array,
    _co_await,
    _unary_plus,
    _unary_minus,
    _unary_ampersand,
    _unary_asterisk,
    _bit_not,
    _plus,
    _minus,
    _multiply,
    _divide,
    _mod,
    _bit_and,
    _bit_or,
    _bit_xor,
    _assign,
    _assign_add,
    _assign_sub,
    _assign_mul,
    _assign_div,
    _assign_mod,
    _assign_and,
    _assign_or,
    _assign_xor,
    _lshift,
    _rshift,
    _assign_lshift,
    _assign_rshift,
    _equal,
    _not_equal,
    _less,
    _greater,
    _less_equal,
    _greater_equal,
    _spaceship,
    _not,
    _and,
    _or,
    _increment,
    _decrement,
    _comma,
    _arrow,
    _arrow_star,
    _call,
    _index,
    _question,
    _cast,
    _suffix,
    _vendor,
};

enum class ctor_dtor_t
{
    _complete_constructor,
    _base_constructor,
    _complete_allocating_constructor,
    _complete_inheriting_constructor,
    _base_inheriting_constructor,
    _deleting_destructor,
    _complete_destructor,
    _base_destructor,
};

enum class special_name_t
{
    _virtual_table,
    _vtt,
    _typeinfo,
    _typeinfo_name,
    _virtual_thunk,
    _virtual_covariant_thunk,
    _guard_variable,
    _temporary,
    _transaction_safe_entry,
};


static std::unique_ptr<type> parse_type(std::string_view& in);
static std::unique_ptr<encoding> parse_encoding(std::string_view& in);
static std::unique_ptr<name> parse_name(std::string_view& in);
static std::unique_ptr<expression> parse_expression(std::string_view& in);

static void format_type(const type&, substitution_table&, std::string&);
static void format_encoding(const encoding&, substitution_table&, std::string&);
static void format_name(const name&, substitution_table&, std::string&);
static void format_expression(const expression&, substitution_table&, std::string&);


struct ref_qualifier
{
    ref_qualifier_t rq;

    void format(substitution_table&, std::string& out) const
    {
        if (rq == ref_qualifier_t::lvalue)
            out += "&";
        else
            out += "&&";
    }

    static std::optional<ref_qualifier> parse(std::string_view& in)
    {
        if (in[0] == 'R')
        {
            in = in.substr(1);
            return {{ref_qualifier_t::lvalue}};
        }
        else if (in[0] == 'O')
        {
            in = in.substr(1);
            return {{ref_qualifier_t::rvalue}};
        }
        else
        {
            return {};
        }
    }
};

struct cv_qualifiers
{
    bool r = false;     // restrict
    bool V = false;     // volatile
    bool K = false;     // const

    void format(substitution_table&, std::string& out) const
    {
        if (K)
            out += "const ";

        if (V)
            out += "volatile ";

        if (r)
            out += "restrict";
    }

    static cv_qualifiers parse(std::string_view& in)
    {
        cv_qualifiers cvq;
        bool valid = true;
        while (valid)
        {
            switch (in[0])
            {
                case 'r':
                    cvq.r = true;
                    break;

                case 'V':
                    cvq.V = true;
                    break;

                case 'K':
                    cvq.K = true;
                    break;

                default:
                    valid = false;
                    break;
            }

            if (valid)
                in = in.substr(1);
        }

        return cvq;
    }
};

struct number
{
    int             value;

    static std::optional<number> parse(std::string_view& in)
    {
        bool negative;
        if (in.starts_with("n"))
        {
            in = in.substr(1);
            negative = true;
        }

        int value = 0;
        bool accepted = false;
        while (true)
        {
            auto c = in.at(0);
            if (c < '0' || c > '9')
                break;

            if (accepted && c == '0' && value == 0)
                return {};

            accepted = true;
            value = (value * 10) + (c - '0');
            in = in.substr(1);
        }

        if (!accepted)
            return {};

        return number{negative ? -value : value};
    }
};

struct seq_id
{
    unsigned int    index;

    static std::optional<seq_id> parse(std::string_view& in)
    {
        unsigned int val = 0;
        bool accepted = false;
        while (true)
        {
            auto c = in[0];
            if ('0' <= c && c <= '9')
                val = (val * 36) + static_cast<unsigned int>(c - '0');
            else if ('A' <= c && c <= 'Z')
                val = (val * 36) + static_cast<unsigned int>(c - 'A' + 10);
            else
                break;

            accepted = true;
            in = in.substr(1);
        }

        if (accepted)
            return seq_id{val};
        else
            return {};
    }
};

struct substitution
{
    substitution_t      substitution_type;
    unsigned int        ref_index;

    void format(substitution_table& sub, std::string& out) const
    {
        switch (substitution_type)
        {
            case substitution_t::std:
                out += "std";
                break;

            case substitution_t::std_allocator:
                out += "std::allocator";
                break;

            case substitution_t::std_basic_string:
                out += "std::basic_string";
                break;

            case substitution_t::std_string:
                out += "std::string";
                break;

            case substitution_t::std_istream:
                out += "std::istream";
                break;

            case substitution_t::std_ostream:
                out += "std::ostream";
                break;

            case substitution_t::std_iostream:
                out += "std::iostream";
                break;

            case substitution_t::ref:
                if (ref_index >= sub.size())
                    out += "<bad substitution>";
                else
                {
                    auto [offset, len] = sub[ref_index];
                    auto s = out.substr(offset, len);
                    out += s;
                }
                break;
        }
    }

    static std::optional<substitution> parse(std::string_view& in)
    {
        if (!in.starts_with("S"))
            return {};

        in.substr(1);

        substitution_t type;
        switch (in.at(0))
        {
            case 't':
                type = substitution_t::std;
                break;

            case 'a':
                type = substitution_t::std_allocator;
                break;

            case 'b':
                type = substitution_t::std_basic_string;
                break;

            case 's':
                type = substitution_t::std_string;
                break;

            case 'i':
                type = substitution_t::std_istream;
                break;

            case 'o':
                type = substitution_t::std_ostream;
                break;

            case 'd':
                type = substitution_t::std_iostream;
                break;

            default:
                type = substitution_t::ref;
                break;
        }

        if (type != substitution_t::ref)
        {
            in = in.substr(1);
            return substitution{type, 0};
        }
        
        unsigned int index = 0;
        if (!in.starts_with("_"))
        {
            auto seq = seq_id::parse(in);
            if (!seq || !in.starts_with("_"))
                return {};
            index = seq->index;
        }

        return substitution{type, index};
    }
};

struct source_name
{
    std::string_view    name;

    void format(substitution_table&, std::string& out) const
    {
        out += name;
    }

    static std::optional<source_name> parse(std::string_view& in)
    {
        auto n = number::parse(in);
        if (!n)
            return {};

        if (n->value <= 0 || n->value > in.size())
            return {};

        auto len = static_cast<std::size_t>(n->value);
        auto name = in.substr(0, len);
        in = in.substr(len);

        return source_name{name};
    }
};

struct expr_primary
{
    //! @TODO: implement.

    void format(substitution_table& sub, std::string& out) const
    {
    }

    static std::optional<expr_primary> parse(std::string_view& in)
    {
        return {};
    }
};

struct template_arg
{
    struct argument_pack
    {
        std::vector<template_arg>   args;
    };

    using variant_t = std::variant
    <
        std::unique_ptr<type>,
        std::unique_ptr<expression>,
        expr_primary,
        argument_pack
    >;

    variant_t   arg;

    void format(substitution_table& sub, std::string& out) const
    {
        std::visit([&]<class T>(const T& t)
        {
            if constexpr (std::is_same_v<T, argument_pack>)
            {
                bool first = true;
                out += "(";
                for (const auto& a : t.args)
                {
                    if (!first)
                        out += ", ";
                    else
                        first = false;

                    a.format(sub, out);
                }
                out += ")...";
            }
            else if constexpr (std::is_same_v<T, std::unique_ptr<type>>)
            {
                format_type(*t, sub, out);
            }
            else if constexpr (std::is_same_v<T, std::unique_ptr<expression>>)
            {
                format_expression(*t, sub, out);
            }
            else if constexpr (std::is_same_v<T, expr_primary>)
            {
                t.format(sub, out);
            }
            else
            {
                static_assert(!std::is_same_v<T, T>);
            }
        }, arg);
    }

    static std::optional<template_arg> parse(std::string_view& in)
    {
        auto in_orig = in;
        
        if (in.starts_with("X"))
        {
            in.remove_prefix(1);

            auto e = parse_expression(in);
            if (!e)
                return {};

            if (!in.starts_with("E"))
                return {};

            in.remove_prefix(1);

            return template_arg{std::move(e)};
        }
        else if (in.starts_with("J"))
        {
            in.remove_prefix(1);

            std::vector<template_arg> pack;
            while (true)
            {
                auto a = template_arg::parse(in);
                if (a)
                    pack.push_back(std::move(*a));
                else
                    break;
            }

            if (!in.starts_with("E"))
                return {};

            in.remove_prefix(1);

            return template_arg{argument_pack{std::move(pack)}};
        }

        if (auto t = parse_type(in); t)
        {
            return template_arg{std::move(t)};
        }

        in = in_orig;

        if (auto ep = expr_primary::parse(in); ep)
        {
            return template_arg{std::move(*ep)};
        }

        return {};
    }
};

struct template_args
{
    std::vector<template_arg>   args;

    void format(substitution_table& sub, std::string& out) const
    {
        out += "<";
        bool first = true;
        for (const auto& ta : args)
        {
            if (!first)
                out += ", ";
            else
                first = false;

            ta.format(sub, out);
        }
        out += ">";
    }

    static std::optional<template_args> parse(std::string_view& in)
    {
        if (!in.starts_with("I"))
            return {};

        std::vector<template_arg> args;
        while (true)
        {
            auto arg = template_arg::parse(in);
            if (!arg)
                return {};

            args.emplace_back(std::move(*arg));

            if (in.starts_with("E"))
                break;
        }

        return template_args{std::move(args)};
    }
};

struct unnamed_type_name
{
    unsigned int    index;

    void format(substitution_table&, std::string& out) const
    {
        out += std::format("<un-named type #{}>", index);
    }

    static std::optional<unnamed_type_name> parse(std::string_view& in)
    {
        if (!in.starts_with("Ut"))
            return {};

        in = in.substr(2);
        auto n = number::parse(in);

        if (!n)
            return unnamed_type_name{0};

        if (n->value < 0)
            return {};

        return unnamed_type_name{static_cast<unsigned int>(n->value) + 1};
    }
};

struct abi_tag
{
    source_name     tag;

    void format(substitution_table& sub, std::string& out) const
    {
        tag.format(sub, out);
    }

    static std::optional<abi_tag> parse(std::string_view& in)
    {
        if (!in.starts_with("B"))
            return {};

        auto tag = source_name::parse(in);
        if (!tag)
            return {};

        return abi_tag{*tag};
    }
};

struct abi_tags
{
    std::vector<abi_tag>    tags;

    void format(substitution_table& sub, std::string& out) const
    {
        for (const auto& t : tags)
        {
            out += " [[abi_tag(";
            t.format(sub, out);
            out += ")]]";
        }
    }

    static std::optional<abi_tags> parse(std::string_view& in)
    {
        std::vector<abi_tag> tags;
        while (true)
        {
            auto tag = abi_tag::parse(in);
            if (!tag)
                break;

            tags.emplace_back(std::move(*tag));
        }

        if (tags.size() == 0)
            return {};

        return abi_tags{std::move(tags)};
    }
};

struct decltype_expr
{
    struct id_expression
    {
        std::unique_ptr<expression> e;
    };

    struct other_expression
    {
        std::unique_ptr<expression> e;
    };

    using variant_t = std::variant
    <
        id_expression,
        other_expression
    >;

    variant_t   expr;

    void format(substitution_table& sub, std::string& out) const
    {
        out += "decltype(";
        std::visit([&](const auto& t)
        {
            format_expression(*t.e, sub, out);
        }, expr);
        out += ")";
    }

    static std::optional<decltype_expr> parse(std::string_view& in)
    {
        if (in.starts_with("Dt"))
        {
            in.remove_prefix(2);

            auto e = parse_expression(in);
            if (!e)
                return {};

            if (!in.starts_with("E"))
                return {};

            in.remove_prefix(1);

            return decltype_expr{id_expression{std::move(e)}};
        }
        else if (in.starts_with("DT"))
        {
            in.remove_prefix(2);

            auto e = parse_expression(in);
            if (!e)
                return {};

            if (!in.starts_with("E"))
                return {};

            in.remove_prefix(1);

            return decltype_expr{other_expression{std::move(e)}};
        }

        return {};
    }
};


struct template_param
{
    unsigned int    index;

    void format(substitution_table&, std::string& out) const
    {
        out += std::format("<template param #{}>", index);
    }

    static std::optional<template_param> parse(std::string_view& in)
    {
        if (!in.starts_with("T"))
            return {};

        in.substr(1);

        unsigned int index = 0;
        if (!in.starts_with("_"))
        {
            auto n = number::parse(in);
            if (!n || n->value < 0 || !in.starts_with("_"))
                return {};

            index = static_cast<unsigned int>(n->value) + 1;
        }

        in = in.substr(1);
        return template_param{index};
    }
};

struct template_template_param
{
    using variant_t = std::variant
    <
        template_param,
        substitution
    >;

    variant_t   ttp;

    static std::optional<template_template_param> parse(std::string_view& in)
    {
        auto in_orig = in;

        auto tp = template_param::parse(in);
        if (tp)
            return template_template_param{*tp};

        in = in_orig;
        auto s = substitution::parse(in);
        if (s)
            return template_template_param{*s};

        return {};
    }
};

struct function_param
{
    struct this_param
    {
    };    

    struct normal_param
    {
        unsigned int    level;
        unsigned int    index;
        cv_qualifiers   qualifiers;
    };

    using variant_t = std::variant
    <
        this_param,
        normal_param
    >;

    variant_t param;

    static std::optional<function_param> parse(std::string_view& in)
    {
        if (!in.starts_with("f"))
            return {};

        in.remove_prefix(1);

        cv_qualifiers qualifiers;
        unsigned int level;
        unsigned int id;
        switch (in.at(0))
        {
            case 'p':
            {
                in.remove_prefix(1);

                if (in.starts_with("T"))
                {
                    in.remove_prefix(1);
                    return function_param{this_param{}};
                }

                level = 0;
                qualifiers = cv_qualifiers::parse(in);

                auto n = number::parse(in);
                if (n)
                    id = static_cast<unsigned int>(n->value);

                break;
            }

            case 'L':
            {
                in.remove_prefix(1);

                auto l = number::parse(in);
                if (!l)
                    return {};
                level = static_cast<unsigned int>(l->value) + 1;

                if (!in.starts_with("p"))
                    return {};

                in.remove_prefix(1);

                qualifiers = cv_qualifiers::parse(in);
                
                auto n = number::parse(in);
                if (n)
                    id = static_cast<unsigned int>(n->value);

                break;
            }

            default:
                return {};
        }

        if (!in.starts_with("_"))
            return {};

        in.remove_prefix(1);

        return function_param{normal_param{level, id, qualifiers}};
    }
};

struct operator_name
{
    struct vendor
    {
        unsigned int        operand_count;
        source_name         name;
    };
    
    using variant_t = std::variant
    <
        std::monostate,         // Everything not specified below.
        std::unique_ptr<type>,  // Cast operators.
        source_name,            // String suffix operators.
        vendor                  // Vendor operators.
    >;

    operator_name_t op;
    variant_t       on  = {};

    void format(substitution_table&, std::string& out) const
    {
        //! @TODO
    }

    static std::optional<operator_name> parse(std::string_view& in)
    {
        if (in.starts_with("cv"))
        {
            // Cast operator.
            in.remove_prefix(2);

            auto t = parse_type(in);
            if (!t)
                return {};

            return operator_name{operator_name_t::_cast, std::move(t)};
        }
        else if (in.starts_with("li"))
        {
            // Literal suffix operator.
            in.remove_prefix(2);

            auto name = source_name::parse(in);
            if (!name)
                return {};

            return operator_name{operator_name_t::_suffix, *name};
        }
        else if (in.starts_with("v"))
        {
            // Vendor-specific operator.
            in.remove_prefix(1);

            auto digit = in.at(0);
            if (digit < '0' || digit > '9')
                return {};
            auto op_count = static_cast<unsigned int>(digit - '0');

            in.remove_prefix(1);
            auto name = source_name::parse(in);
            if (!name)
                return {};

            return operator_name{operator_name_t::_vendor, vendor{op_count, *name}};
        }

        // All of the remaining operators are idenified by a pair of characters.
        operator_name_t op;
        if (in.starts_with("nw"))
            op = operator_name_t::_new;
        else if (in.starts_with("na"))
            op = operator_name_t::_new_array;
        else if (in.starts_with("dl"))
            op = operator_name_t::_delete;
        else if (in.starts_with("da"))
            op = operator_name_t::_delete_array;
        else if (in.starts_with("aw"))
            op = operator_name_t::_co_await;
        else if (in.starts_with("ps"))
            op = operator_name_t::_unary_plus;
        else if (in.starts_with("ng"))
            op = operator_name_t::_unary_minus;
        else if (in.starts_with("ad"))
            op = operator_name_t::_unary_ampersand;
        else if (in.starts_with("de"))
            op = operator_name_t::_unary_asterisk;
        else if (in.starts_with("co"))
            op = operator_name_t::_bit_not;
        else if (in.starts_with("pl"))
            op = operator_name_t::_plus;
        else if (in.starts_with("mi"))
            op = operator_name_t::_minus;
        else if (in.starts_with("ml"))
            op = operator_name_t::_multiply;
        else if (in.starts_with("dv"))
            op = operator_name_t::_divide;
        else if (in.starts_with("rm"))
            op = operator_name_t::_mod;
        else if (in.starts_with("an"))
            op = operator_name_t::_bit_and;
        else if (in.starts_with("or"))
            op = operator_name_t::_bit_or;
        else if (in.starts_with("eo"))
            op = operator_name_t::_bit_xor;
        else if (in.starts_with("aS"))
            op = operator_name_t::_assign;
        else if (in.starts_with("pL"))
            op = operator_name_t::_assign_add;
        else if (in.starts_with("mI"))
            op = operator_name_t::_assign_sub;
        else if (in.starts_with("mL"))
            op = operator_name_t::_assign_mul;
        else if (in.starts_with("dV"))
            op = operator_name_t::_assign_div;
        else if (in.starts_with("rM"))
            op = operator_name_t::_assign_mod;
        else if (in.starts_with("aN"))
            op = operator_name_t::_assign_and;
        else if (in.starts_with("oR"))
            op = operator_name_t::_assign_or;
        else if (in.starts_with("eO"))
            op = operator_name_t::_assign_xor;
        else if (in.starts_with("ls"))
            op = operator_name_t::_lshift;
        else if (in.starts_with("rs"))
            op = operator_name_t::_rshift;
        else if (in.starts_with("lS"))
            op = operator_name_t::_assign_lshift;
        else if (in.starts_with("rS"))
            op = operator_name_t::_assign_rshift;
        else if (in.starts_with("eq"))
            op = operator_name_t::_equal;
        else if (in.starts_with("ne"))
            op = operator_name_t::_not_equal;
        else if (in.starts_with("lt"))
            op = operator_name_t::_less;
        else if (in.starts_with("gt"))
            op = operator_name_t::_greater;
        else if (in.starts_with("le"))
            op = operator_name_t::_less_equal;
        else if (in.starts_with("ge"))
            op = operator_name_t::_greater_equal;
        else if (in.starts_with("ss"))
            op = operator_name_t::_spaceship;
        else if (in.starts_with("nt"))
            op = operator_name_t::_not;
        else if (in.starts_with("aa"))
            op = operator_name_t::_and;
        else if (in.starts_with("oo"))
            op = operator_name_t::_or;
        else if (in.starts_with("pp"))
            op = operator_name_t::_increment;
        else if (in.starts_with("mm"))
            op = operator_name_t::_decrement;
        else if (in.starts_with("pm"))
            op = operator_name_t::_arrow_star;
        else if (in.starts_with("pt"))
            op = operator_name_t::_arrow;
        else if (in.starts_with("cl"))
            op = operator_name_t::_call;
        else if (in.starts_with("ix"))
            op = operator_name_t::_index;
        else if (in.starts_with("qu"))
            op = operator_name_t::_question;
        else
            return {};

        in.remove_prefix(2);
        return operator_name{op};
    }
};

struct ctor_dtor_name
{
    ctor_dtor_t             name;
    std::unique_ptr<type>   base    = {};   // For inheriting constructors.

    void format(substitution_table&, std::string& out) const
    {
        switch (name)
        {
            case ctor_dtor_t::_complete_inheriting_constructor:
            case ctor_dtor_t::_base_inheriting_constructor:
            case ctor_dtor_t::_complete_constructor:
            case ctor_dtor_t::_base_constructor:
            case ctor_dtor_t::_complete_allocating_constructor:
                out += "<constructor>";
                break;

            case ctor_dtor_t::_deleting_destructor:
            case ctor_dtor_t::_complete_destructor:
            case ctor_dtor_t::_base_destructor:
                out += "<destructor>";
                break;
        }
    }

    static std::optional<ctor_dtor_name> parse(std::string_view& in)
    {
        if (in.starts_with("CI1"))
        {
            // Complete object inheriting constructor.
            in.remove_prefix(3);

            auto t = parse_type(in);
            if (!t)
                return {};

            return ctor_dtor_name{ctor_dtor_t::_complete_inheriting_constructor, std::move(t)};
        }
        else if (in.starts_with("CI2"))
        {
            // Base object inheriting constructor.
            in.remove_prefix(3);

            auto t = parse_type(in);
            if (!t)
                return {};

            return ctor_dtor_name{ctor_dtor_t::_base_inheriting_constructor, std::move(t)};
        }

        // The remaining names are simple pairs.
        ctor_dtor_t t;
        if (in.starts_with("C1"))
            t = ctor_dtor_t::_complete_constructor;
        else if (in.starts_with("C2"))
            t = ctor_dtor_t::_base_constructor;
        else if (in.starts_with("C3"))
            t = ctor_dtor_t::_complete_allocating_constructor;
        else if (in.starts_with("D0"))
            t = ctor_dtor_t::_deleting_destructor;
        else if (in.starts_with("D1"))
            t = ctor_dtor_t::_complete_destructor;
        else if (in.starts_with("D2"))
            t = ctor_dtor_t::_base_destructor;
        else
            return {};

        in.remove_prefix(2);
        return ctor_dtor_name{t};
    }
};

struct unqualified_name
{
    struct structured_binding
    {
        std::vector<source_name>    names;
    };

    using variant_t = std::variant
    <
        operator_name,
        ctor_dtor_name,
        source_name,
        unnamed_type_name,
        structured_binding
    >;

    variant_t               uqn;
    std::optional<abi_tags> tags    = {};

    void format(substitution_table& sub, std::string& out) const
    {
        std::visit([&]<class T>(const T& t)
        {
            if constexpr (std::is_same_v<T, structured_binding>)
            {
                out += "[";
                bool first = true;
                for (const auto& n : t.names)
                {
                    if (!first)
                        out += ", ";
                    else
                        first = false;

                    n.format(sub, out);
                }
                out += "]";
            }
            else
            {
                t.format(sub, out);
            }

            if (tags)
                tags->format(sub, out);

        }, uqn);
    }

    static std::optional<unqualified_name> parse(std::string_view& in)
    {
        auto in_orig = in;

        if (auto op_name = operator_name::parse(in); op_name)
        {
            auto tags = abi_tags::parse(in);
            return unqualified_name{std::move(*op_name), std::move(tags)};
        }

        in = in_orig;

        if (auto cd_name = ctor_dtor_name::parse(in); cd_name)
        {
            auto tags = abi_tags::parse(in);
            return unqualified_name{std::move(*cd_name), std::move(tags)};
        }

        in = in_orig;

        if (auto s_name = source_name::parse(in); s_name)
        {
            auto tags = abi_tags::parse(in);
            return unqualified_name{*s_name, std::move(tags)};
        }

        in = in_orig;

        if (auto ut_name = unnamed_type_name::parse(in); ut_name)
        {
            auto tags = abi_tags::parse(in);
            return unqualified_name{*ut_name, std::move(tags)};
        }

        in = in_orig;

        if (in.starts_with("DC"))
        {
            // Structured binding declaration.
            in.remove_prefix(2);

            std::vector<source_name> names;
            while (!in.starts_with("E"))
            {
                auto sn = source_name::parse(in);
                if (!sn)
                    return {};

                names.push_back(*sn);
            }

            if (names.size() == 0)
                return {};

            return unqualified_name{structured_binding{std::move(names)}};
        }

        // None of the patterns matched.
        return {};
    }
};

/*struct data_member_prefix
{
    source_name                     member_name;
    std::optional<template_args>    targs;

    static std::optional<data_member_prefix> parse(std::string_view& in)
    {
        auto sn = source_name::parse(in);
        if (!sn)
            return {};

        std::optional<template_args> targs = std::nullopt;
        if (in.at(0) != 'M')
        {
            targs = template_args::parse(in);
            if (!targs || in.at(0) != 'M')
                return {};
        }

        return data_member_prefix{*sn, std::move(targs)};
    }
};*/

/*struct template_prefix
{
    struct global
    {
        unqualified_name            name;
    };

    struct nested
    {
        std::unique_ptr<prefix>     next_prefix;
        unqualified_name            name;
    };

    using variant_t = std::variant
    <
        global,
        nested,
        template_param,
        substitution
    >;

    variant_t       tp;

    static std::optional<template_prefix> parse(std::string_view& in)
    {
        auto in_orig = in;

        if (auto un = unqualified_name::parse(in); un)
        {
            return template_prefix{global{std::move(*un)}};
        }

        in = in_orig;

        if (auto p = parse_prefix(in); p)
        {
            auto un = unqualified_name::parse(in);
            if (!un)
                return {};

            return template_prefix{nested{std::move(p), std::move(*un)}};
        }

        in = in_orig;

        if (auto tp = template_param::parse(in); tp)
        {
            return template_prefix{*tp};
        }

        in = in_orig;

        if (auto s = substitution::parse(in); s)
        {
            return template_prefix{*s};
        }

        return {};
    }
};*/

struct prefix
{
    using component_t = std::variant
    <
        unqualified_name,
        template_args,
        template_param,
        decltype_expr,
        substitution
    >;

    using components_t = std::vector<component_t>;

    components_t components;

    void format(substitution_table& sub, std::string& out) const
    {
        bool first = true;
        for (const auto& c : components)
        {
            std::visit([&]<class T>(const T& t)
            {
                if constexpr (!std::is_same_v<T, template_args>)
                {
                    if (!first)
                        out += "::";
                    else
                        first = false;
                }

                t.format(sub, out);
            }, c);
        }
    }

    static std::optional<component_t> parse_component(std::string_view& in)
    {
        auto in_orig = in;

        if (auto un = unqualified_name::parse(in); un)
        {
            return component_t{std::move(*un)};
        }

        in = in_orig;

        if (auto ta = template_args::parse(in); ta)
        {
            return component_t{std::move(*ta)};
        }

        in = in_orig;

        if (auto tp = template_param::parse(in); tp)
        {
            return component_t{std::move(*tp)};
        }

        in = in_orig;

        if (auto dc = decltype_expr::parse(in); dc)
        {
            return component_t{std::move(*dc)};
        }

        in = in_orig;

        if (auto s = substitution::parse(in); s)
        {
            return component_t{std::move(*s)};
        }

        return {};
    }

    static std::optional<prefix> parse(std::string_view& in)
    {
        components_t components;
        while (true)
        {
            auto c = parse_component(in);
            if (c)
                break;

            components.push_back(std::move(*c));
        }

        if (components.size() == 0)
            return {};

        return prefix{std::move(components)};
    }
};

struct nested_name
{
    cv_qualifiers                   qualifiers;
    std::optional<ref_qualifier>    ref;
    prefix                          name;

    void format(substitution_table& sub, std::string& out) const
    {
        qualifiers.format(sub, out);
        name.format(sub, out);
        if (ref)
            ref->format(sub, out);
    }

    static std::optional<nested_name> parse(std::string_view& in)
    {
        if (!in.starts_with("N"))
            return {};

        in.remove_prefix(1);

        auto qual = cv_qualifiers::parse(in);

        auto rq = ref_qualifier::parse(in);

        auto p = prefix::parse(in);
        if (!p)
            return {};

        if (in.at(0) != 'E')
            return {};

        in.remove_prefix(1);

        return nested_name{qual, rq, std::move(*p)};
    }
};

struct unscoped_name
{
    bool                std_ns;
    unqualified_name    name;

    void format(substitution_table& sub, std::string& out) const
    {
        if (std_ns)
            out += "std::";
        name.format(sub, out);
    }

    static std::optional<unscoped_name> parse(std::string_view& in)
    {
        bool std_ns = false;
        if (in.starts_with("St"))
        {
            std_ns = true;
            in.remove_prefix(2);
        }

        auto un = unqualified_name::parse(in);
        if (!un)
            return {};

        return unscoped_name{std_ns, std::move(*un)};
    }
};

struct unscoped_template_name
{
    using variant_t = std::variant
    <
        unscoped_name,
        substitution
    >;

    variant_t   name;

    void format(substitution_table& sub, std::string& out) const
    {
        std::visit([&](const auto& t)
        {
            t.format(sub, out);
        }, name);
    }

    static std::optional<unscoped_template_name> parse(std::string_view& in)
    {
        auto in_orig = in;

        if (auto un = unscoped_name::parse(in); un)
        {
            return unscoped_template_name{std::move(*un)};
        }

        in = in_orig;

        if (auto s = substitution::parse(in); s)
        {
            return unscoped_template_name{*s};
        }

        return {};
    }
};

struct local_name
{
    std::unique_ptr<encoding>   function;
    unsigned int                id;
    std::unique_ptr<name>       entity;

    void format(substitution_table& sub, std::string& out) const
    {
        format_encoding(*function, sub, out);
        out += "##";
        format_name(*entity, sub, out);
        out += std::format("#{}", id);
    }

    static std::optional<local_name> parse(std::string_view& in)
    {
        if (!in.starts_with("Z"))
            return {};

        in.remove_prefix(1);

        auto f = parse_encoding(in);
        if (!f)
            return {};

        if (!in.starts_with("E"))
            return {};

        in.remove_prefix(1);

        std::unique_ptr<name> entity = nullptr;
        if (in.starts_with("S"))
        {
            in.remove_prefix(1);
        }
        else
        {
            entity = parse_name(in);
            if (!entity)
                return {};
        }

        unsigned int id = 0;
        if (in.starts_with("__"))
        {
            in.remove_prefix(2);

            auto n = number::parse(in);
            if (!n || n->value < 0)
                return {};

            id = static_cast<unsigned int>(n->value) + 1;

            if (!in.starts_with("_"))
                return {};
        }
        else if (in.starts_with("_"))
        {
            in.remove_prefix(1);

            auto digit = in.at(0);
            in.remove_prefix(1);
            if (digit < '0' || digit > '9')
                return {};

            id = static_cast<unsigned int>(digit - '0') + 1;
        }

        return local_name{std::move(f), id, std::move(entity)};
    }
};

struct name
{
    struct unscoped_template
    {
        unscoped_template_name      name;
        template_args               args;
    };
    
    using variant_t = std::variant
    <
        nested_name,
        unscoped_name,
        unscoped_template,
        local_name  
    >;

    variant_t   n;

    void format(substitution_table& sub, std::string& out) const
    {
        std::visit([&]<class T>(const T& t)
        {
            if constexpr (std::is_same_v<T, unscoped_template>)
            {
                t.name.format(sub, out);
                t.args.format(sub, out);
            }
            else
            {
                t.format(sub, out);
            }
        }, n);
    }

    static std::optional<name> parse(std::string_view& in)
    {
        auto in_orig = in;

        if (auto n = nested_name::parse(in); n)
        {
            return name{std::move(*n)};
        }

        in = in_orig;

        if (auto s = substitution::parse(in); s)
        {
            auto args = template_args::parse(in);
            if (!args)
                return {};

            return name{unscoped_template{{*s}, std::move(*args)}};
        }

        in = in_orig;

        if (auto un = unscoped_name::parse(in); un)
        {
            auto args = template_args::parse(in);
            if (args)
                return name{unscoped_template{{std::move(*un)}, std::move(*args)}};

            return name{std::move(*un)};
        }

        if (auto ln = local_name::parse(in); ln)
        {
            return name{std::move(*ln)};
        }

        return {};
    }
};

struct expression
{
    //! @TODO: implement.

    void format(substitution_table& sub, std::string& out) const
    {
    }

    static std::optional<expression> parse(std::string_view& in)
    {
        return {};
    }
};

struct builtin_type
{
    struct vendor_t
    {
        source_name                     sn;
        std::optional<template_args>    ta;
    };
    
    using variant_t = std::variant
    <
        std::monostate,     // Everything except the following.
        unsigned int,       // _floatN: number of bits
        vendor_t            // _vendor: the name of the extended type
    >;
   
    builtin_type_t      t;
    variant_t           data    = {};

    void format(substitution_table& sub, std::string& out) const
    {
        switch (t)
        {
            case builtin_type_t::_void:
                out += "void";
                break;

            case builtin_type_t::_wchar_t:
                out += "wchar_t";
                break;

            case builtin_type_t::_bool:
                out += "bool";
                break;

            case builtin_type_t::_char:
                out += "char";
                break;

            case builtin_type_t::_signed_char:
                out += "signed char";
                break;

            case builtin_type_t::_unsigned_char:
                out += "unsigned char";
                break;

            case builtin_type_t::_short:
                out += "short";
                break;

            case builtin_type_t::_unsigned_short:
                out += "unsigned short";
                break;

            case builtin_type_t::_int:
                out += "int";
                break;

            case builtin_type_t::_unsigned_int:
                out += "unsigned int";
                break;

            case builtin_type_t::_long:
                out += "long";
                break;

            case builtin_type_t::_unsigned_long:
                out += "unsigned long";
                break;

            case builtin_type_t::_llong:
                out += "long long";
                break;

            case builtin_type_t::_unsigned_llong:
                out += "unsigned long long";
                break;

            case builtin_type_t::_int128:
                out += "__int128";
                break;

            case builtin_type_t::_unsigned_int128:
                out += "unsigned __int128";
                break;

            case builtin_type_t::_float:
                out += "float";
                break;

            case builtin_type_t::_double:
                out += "double";
                break;

            case builtin_type_t::_long_double:
                out += "long double";
                break;

            case builtin_type_t::_float128:
                out += "__float128";
                break;

            case builtin_type_t::_ellipsis:
                out += "...";
                break;

            case builtin_type_t::_decimal64:
                out += "_Decimal64";
                break;

            case builtin_type_t::_decimal128:
                out += "_Decimal128";
                break;

            case builtin_type_t::_decimal32:
                out += "_Decimal32";
                break;

            case builtin_type_t::_half_float:
                out += "__half_float";
                break;

            case builtin_type_t::_char16_t:
                out += "char16_t";
                break;

            case builtin_type_t::_char32_t:
                out += "char32_t";
                break;

            case builtin_type_t::_auto:
                out += "auto";
                break;

            case builtin_type_t::_decltype_auto:
                out += "decltype(auto)";
                break;

            case builtin_type_t::_nullptr_t:
                out += "std::nullptr_t";
                break;

            case builtin_type_t::_floatN:
                out += std::format("__float{}", get<1>(data));
                break;

            case builtin_type_t::_vendor:
            {
                const auto& v = get<2>(data);
                v.sn.format(sub, out);
                if (v.ta)
                    v.ta->format(sub, out);
                break;
            }
        }
    }

    static std::optional<builtin_type> parse(std::string_view& in)
    {
        // Extended types.
        builtin_type_t t;
        if (in.starts_with("D"))
        {
            in.remove_prefix(1);

            switch (in.at(0))
            {
                case 'd':
                    t = builtin_type_t::_decimal64;
                    break;

                case 'e':
                    t = builtin_type_t::_decimal128;
                    break;

                case 'f':
                    t = builtin_type_t::_decimal32;
                    break;

                case 'h':
                    t = builtin_type_t::_half_float;
                    break;

                case 'i':
                    t = builtin_type_t::_char16_t;
                    break;

                case 's':
                    t = builtin_type_t::_char32_t;
                    break;

                case 'a':
                    t = builtin_type_t::_auto;
                    break;

                case 'c':
                    t = builtin_type_t::_decltype_auto;
                    break;

                case 'n':
                    t = builtin_type_t::_nullptr_t;
                    break;

                case 'F':
                {
                    // Extended floating point type.
                    in.remove_prefix(1);

                    auto n = number::parse(in);
                    if (!n || n->value <= 0)
                        return {};

                    return builtin_type{builtin_type_t::_floatN, static_cast<unsigned int>(n->value)};
                }

                default:
                    return {};
            }
        }
        else
        {
            switch (in.at(0))
            {
                case 'v':
                    t = builtin_type_t::_void;
                    break;

                case 'w':
                    t = builtin_type_t::_wchar_t;
                    break;

                case 'b':
                    t = builtin_type_t::_bool;
                    break;

                case 'c':
                    t = builtin_type_t::_char;
                    break;

                case 'a':
                    t = builtin_type_t::_signed_char;
                    break;

                case 'h':
                    t = builtin_type_t::_unsigned_char;
                    break;

                case 's':
                    t = builtin_type_t::_short;
                    break;

                case 't':
                    t = builtin_type_t::_unsigned_short;
                    break;

                case 'i':
                    t = builtin_type_t::_int;
                    break;

                case 'j':
                    t = builtin_type_t::_unsigned_int;
                    break;

                case 'l':
                    t = builtin_type_t::_long;
                    break;

                case 'm':
                    t = builtin_type_t::_unsigned_long;
                    break;

                case 'x':
                    t = builtin_type_t::_llong;
                    break;

                case 'y':
                    t = builtin_type_t::_unsigned_llong;
                    break;

                case 'n':
                    t = builtin_type_t::_int128;
                    break;

                case 'o':
                    t = builtin_type_t::_unsigned_int128;
                    break;

                case 'f':
                    t = builtin_type_t::_float;
                    break;

                case 'd':
                    t = builtin_type_t::_double;
                    break;

                case 'e':
                    t = builtin_type_t::_long_double;
                    break;

                case 'g':
                    t = builtin_type_t::_float128;
                    break;

                case 'z':
                    t = builtin_type_t::_ellipsis;
                    break;

                case 'u':
                {
                    // Vendor built-in type.
                    in.remove_prefix(1);

                    auto sn = source_name::parse(in);
                    if (!sn)
                        return {};

                    auto ta = template_args::parse(in);

                    return builtin_type{builtin_type_t::_vendor, vendor_t{std::move(*sn), std::move(ta)}};
                }

                default:
                    return {};
            }
        }

        in.remove_prefix(1);

        return builtin_type{t};
    }
};

struct extended_qualifier
{
    source_name                     sn;
    std::optional<template_args>    ta;

    void format(substitution_table& sub, std::string& out) const
    {
        sn.format(sub, out);
        if (ta)
            ta->format(sub, out);
        out += " ";
    }

    static std::optional<extended_qualifier> parse(std::string_view& in)
    {
        if (!in.starts_with("U"))
            return {};

        in.remove_prefix(1);

        auto sn = source_name::parse(in);
        if (!sn)
            return {};

        auto ta = template_args::parse(in);

        return extended_qualifier{*sn, std::move(ta)};
    }
};

struct qualifiers
{
    std::vector<extended_qualifier>     eqs;
    cv_qualifiers                       cvq;

    void format(substitution_table& sub, std::string& out) const
    {
        for (const auto& eq : eqs)
            eq.format(sub, out);

        cvq.format(sub, out);
    }

    static std::optional<qualifiers> parse(std::string_view& in)
    {
        std::vector<extended_qualifier> eqs;
        while (true)
        {
            auto eq = extended_qualifier::parse(in);
            if (eq)
                eqs.push_back(std::move(*eq));
            else
                break;
        }

        cv_qualifiers cvq = cv_qualifiers::parse(in);

        return qualifiers{std::move(eqs), std::move(cvq)};
    }
};

struct qualified_type
{
    qualifiers              q;
    std::unique_ptr<type>   t;

    void format(substitution_table& sub, std::string& out) const
    {
        q.format(sub, out);
        format_type(*t, sub, out);
    }

    static std::optional<qualified_type> parse(std::string_view& in)
    {
        auto q = qualifiers::parse(in);
        if (!q)
            return {};

        // Prevent a recursion in the type parsing.
        if (q->eqs.empty() && !q->cvq.K && !q->cvq.V && !q->cvq.r)
            return {};

        auto t = parse_type(in);
        if (!t)
            return {};

        return qualified_type{std::move(*q), std::move(t)};
    }
};

struct exception_spec
{
    struct non_throwing
    {
    };

    struct computed_noexcept
    {
        expression e;
    };

    struct dynamic
    {
        std::vector<std::unique_ptr<type>>  types;
    };

    using variant_t = std::variant
    <
        non_throwing,
        computed_noexcept,
        dynamic
    >;

    variant_t   spec;

    // Defined after struct type is defined.
    void format(substitution_table& sub, std::string& out) const;

    static std::optional<exception_spec> parse(std::string_view& in)
    {
        if (in.starts_with("Do"))
        {
            in.remove_prefix(2);
            return exception_spec{non_throwing{}};
        }

        if (in.starts_with("DO"))
        {
            in.remove_prefix(2);

            auto e = expression::parse(in);
            if (!e)
                return {};

            if (!in.starts_with("E"))
                return {};

            in.remove_prefix(1);

            return exception_spec{computed_noexcept{std::move(*e)}};
        }

        if (in.starts_with("Dw"))
        {
            in.remove_prefix(2);

            std::vector<std::unique_ptr<type>> types;
            while (true)
            {
                auto t = parse_type(in);
                if (t)
                    types.push_back(std::move(t));
                else
                    break;
            }

            if (types.size() == 0)
                return {};

            if (!in.starts_with("E"))
                return {};

            in.remove_prefix(1);

            return exception_spec{dynamic{std::move(types)}};
        }

        return {};
    }
};

struct bare_function_type
{
    std::vector<std::unique_ptr<type>>  signature;

    static std::optional<bare_function_type> parse(std::string_view& in)
    {
        std::vector<std::unique_ptr<type>> sig;
        while (true)
        {
            auto t = parse_type(in);
            if (t)
                sig.push_back(std::move(t));
            else
                break;
        }

        if (sig.size() == 0)
            return {};

        return bare_function_type{std::move(sig)};
    }
};

struct function_type
{
    cv_qualifiers                   cvq;
    std::optional<exception_spec>   es;
    bool                            transaction_safe;
    bool                            extern_c;
    bare_function_type              bft;
    std::optional<ref_qualifier>    rq;

    void format(substitution_table& sub, std::string& out) const
    {
        if (es)
            out += "extern \"C\" ";
        
        format_type(*bft.signature[0], sub, out);
        out += "(";
        for (std::size_t i = 1; i < bft.signature.size(); ++i)
        {
            if (i != 1)
                out += ", ";

            format_type(*bft.signature[i], sub, out);
        }
        out += ")";

        if (es)
        {
            out += " ";
            es->format(sub, out);
        }

        cvq.format(sub, out);

        if (rq)
            rq->format(sub, out);
    }

    static std::optional<function_type> parse(std::string_view& in)
    {
        auto cvq = cv_qualifiers::parse(in);
        auto es = exception_spec::parse(in);

        bool transaction_safe = in.starts_with("Dx");
        if (transaction_safe)
            in.remove_prefix(2);

        if (!in.starts_with("F"))
            return {};

        in.remove_prefix(1);

        bool extern_c = in.starts_with("Y");
        if (extern_c)
            in.remove_prefix(2);

        auto bft = bare_function_type::parse(in);
        if (!bft)
            return {};

        auto rq = ref_qualifier::parse(in);

        if (!in.starts_with("E"))
            return {};

        in.remove_prefix(1);

        return function_type{cvq, std::move(es), transaction_safe, extern_c, std::move(*bft), std::move(rq)};
    }
};

struct class_enum_type
{
    enum class elaboration
    {
        _none,
        _class,
        _union,
        _enum,
    };

    elaboration     e;
    name            n;

    void format(substitution_table& sub, std::string& out) const
    {
        switch (e)
        {
            case elaboration::_none:
                break;

            case elaboration::_class:
                out += "class ";
                break;

            case elaboration::_union:
                out += "union ";
                break;

            case elaboration::_enum:
                out += "enum ";
                break;
        }

        n.format(sub, out);
    }

    static std::optional<class_enum_type> parse(std::string_view& in)
    {
        elaboration e = elaboration::_none;
        if (in.starts_with("Ts"))
        {
            e = elaboration::_class;
            in.remove_prefix(2);
        }
        else if (in.starts_with("Tu"))
        {
            e = elaboration::_union;
            in.remove_prefix(2);
        }
        else if (in.starts_with("Te"))
        {
            e = elaboration::_enum;
            in.remove_prefix(2);
        }

        auto n = name::parse(in);
        if (!n)
            return {};

        return class_enum_type{e, std::move(*n)};
    }
};

struct array_type
{
    using variant_t = std::variant
    <
        unsigned int,
        std::optional<expression>
    >;

    variant_t               dimension;
    std::unique_ptr<type>   element_type;

    void format(substitution_table& sub, std::string& out) const
    {
        format_type(*element_type, sub, out);
        if (std::holds_alternative<unsigned int>(dimension))
        {
            auto d = get<0>(dimension);
            if (d == 0)
                out += "[]";
            else
                out += std::format("[{}]", d);
        }
        else
        {
            std::string temp;
            get<1>(dimension)->format(sub, temp);
            out += std::format("[{}]", temp);
        }
    }

    static std::optional<array_type> parse(std::string_view& in)
    {
        if (!in.starts_with("A"))
            return {};

        in.remove_prefix(1);

        auto n = number::parse(in);
        if (n)
        {
            if (!in.starts_with("_"))
                return {};

            auto t = parse_type(in);
            if (!t)
                return {};

            return array_type{static_cast<unsigned int>(n->value), std::move(t)};
        }

        auto e = expression::parse(in);

        if (!in.starts_with("_"))
            return {};

        auto t = parse_type(in);
        if (!t)
            return {};

        return array_type{std::move(e), std::move(t)};
    }
};

struct pointer_to_member_type
{
    std::unique_ptr<type>   class_type;
    std::unique_ptr<type>   member_type;

    void format(substitution_table& sub, std::string& out) const
    {
        format_type(*class_type, sub, out);
        out += "::*";
        format_type(*member_type, sub, out);
    }

    static std::optional<pointer_to_member_type> parse(std::string_view& in)
    {
        if (!in.starts_with("M"))
            return {};

        in.remove_prefix(1);

        auto ct = parse_type(in);

        if (!ct)
            return {};

        auto mt = parse_type(in);

        if (!mt)
            return {};

        return pointer_to_member_type{std::move(ct), std::move(mt)};
    }
};

struct type
{
    enum class decoration : char
    {
        pointer     = 'P',
        lvalref     = 'R',
        rvalref     = 'O',
        complex     = 'C',
        imaginary   = 'G',
    };

    struct decorated_type
    {
        decoration              d;
        std::unique_ptr<type>   t;
    };

    struct template_template
    {
        template_template_param ttp;
        template_args           ta;
    };

    using variant_t = std::variant
    <
        builtin_type,
        qualified_type,
        function_type,
        class_enum_type,
        array_type,
        pointer_to_member_type,
        template_param,
        template_template,
        decltype_expr,
        substitution,
        decorated_type
    >;

    variant_t t;

    void format(substitution_table& sub, std::string& out) const
    {
        std::visit([&]<class T>(const T& t)
        {
            if constexpr (std::is_same_v<T, decorated_type>)
            {
                if (t.d == decoration::complex)
                    out += "_Complex ";
                else if (t.d == decoration::imaginary)
                    out += "_Imaginary ";

                t.t->format(sub, out);

                if (t.d == decoration::pointer)
                    out += "*";
                else if (t.d == decoration::lvalref)
                    out += "&";
                else if (t.d == decoration::rvalref)
                    out += "&&";
            }
            else if constexpr (std::is_same_v<T, template_template>)
            {
                //! @TODO
            }
            else
            {
                t.format(sub, out);
            }
        }, t);
    }

    static std::optional<type> parse(std::string_view& in)
    {
        auto in_orig = in;

        if (in.starts_with("P"))
        {
            in.remove_prefix(1);

            auto t = parse_type(in);
            if (!t)
                return {};

            return type{decorated_type{decoration::pointer, std::move(t)}};
        }
        else if (in.starts_with("R"))
        {
            in.remove_prefix(1);

            auto t = parse_type(in);
            if (!t)
                return {};

            return type{decorated_type{decoration::lvalref, std::move(t)}};
        }
        else if (in.starts_with("O"))
        {
            in.remove_prefix(1);

            auto t = parse_type(in);
            if (!t)
                return {};

            return type{decorated_type{decoration::rvalref, std::move(t)}};
        }
        else if (in.starts_with("C"))
        {
            in.remove_prefix(1);

            auto t = parse_type(in);
            if (!t)
                return {};

            return type{decorated_type{decoration::complex, std::move(t)}};
        }
        else if (in.starts_with("G"))
        {
            in.remove_prefix(1);

            auto t = parse_type(in);
            if (!t)
                return {};

            return type{decorated_type{decoration::imaginary, std::move(t)}};
        }

        if (auto bt = builtin_type::parse(in); bt)
        {
            return type{std::move(*bt)};
        }

        in = in_orig;

        if (auto qt = qualified_type::parse(in); qt)
        {
            return type{std::move(*qt)};
        }

        in = in_orig;

        if (auto ft = function_type::parse(in); ft)
        {
            return type{std::move(*ft)};
        }

        in = in_orig;

        if (auto cet = class_enum_type::parse(in); cet)
        {
            return type{std::move(*cet)};
        }

        in = in_orig;

        if (auto at = array_type::parse(in); at)
        {
            return type{std::move(*at)};
        }

        in = in_orig;

        if (auto pmt = pointer_to_member_type::parse(in); pmt)
        {
            return type{std::move(*pmt)};
        }

        in = in_orig;

        if (auto tp = template_param::parse(in); tp)
        {
            auto ta = template_args::parse(in);
            if (ta)
                return type{template_template{std::move(*tp), std::move(*ta)}};
            else
                return type{std::move(*tp)};
        }

        in = in_orig;

        if (auto de = decltype_expr::parse(in); de)
        {
            return type{std::move(*de)};
        }

        in = in_orig;

        if (auto s = substitution::parse(in); s)
        {
            auto ta = template_args::parse(in);
            if (ta)
                return type{template_template{std::move(*s), std::move(*ta)}};
            else
                return type{std::move(*s)};
        }

        return {};
    }
};

struct nv_offset
{
    int     offset;

    static std::optional<nv_offset> parse(std::string_view& in)
    {
        auto n = number::parse(in);
        if (!n)
            return {};

        return nv_offset{n->value};
    }
};

struct v_offset
{
    int     offset;
    int     virtual_offset;

    static std::optional<v_offset> parse(std::string_view& in)
    {
        auto n = number::parse(in);
        if (!n)
            return {};

        if (!in.starts_with("_"))
            return {};

        in.remove_prefix(1);

        auto v = number::parse(in);
        if (!v)
            return {};

        return v_offset{n->value, v->value};
    }
};

struct call_offset
{
    using variant_t = std::variant
    <
        nv_offset,
        v_offset
    >;

    variant_t   co;

    static std::optional<call_offset> parse(std::string_view& in)
    {
        if (in.starts_with("h"))
        {
            in.remove_prefix(1);

            auto nv = nv_offset::parse(in);
            if (!nv)
                return {};

            if (!in.starts_with("_"))
                return {};

            in.remove_prefix(1);

            return call_offset{std::move(*nv)};
        }

        if (in.starts_with("v"))
        {
            in.remove_prefix(1);

            auto v = v_offset::parse(in);
            if (!v)
                return {};

            if (!in.starts_with("_"))
                return {};

            in.remove_prefix(1);

            return call_offset{std::move(*v)};
        }

        return {};
    }
};

struct special_name
{
    struct virtual_table
    {
        type                        for_type;
    };

    struct vtt
    {
        type                        for_type;
    };

    struct typeinfo
    {
        type                        for_type;
    };

    struct typeinfo_name
    {
        type                        for_type;
    };
    
    struct virtual_thunk
    {
        call_offset                 offset;
        std::unique_ptr<encoding>   base;
    };

    struct virtual_covariant_thunk
    {
        call_offset                 this_offset;
        call_offset                 result_offset;
        std::unique_ptr<encoding>   base;
    };

    struct guard_variable
    {
        name                        var_name;
    };

    struct temporary
    {
        unsigned int                id;
        name                        object;
    };

    struct transaction_safe_entry
    {
        std::unique_ptr<encoding>   entry_for;
    };
    
    using variant_t = std::variant
    <
        virtual_table,
        vtt,
        typeinfo,
        typeinfo_name,
        virtual_thunk,
        virtual_covariant_thunk,
        guard_variable,             // Guard variables.
        temporary,                  // Temporaries.
        transaction_safe_entry      // Transaction-safe entry points.
    >;

    variant_t   sn;

    // Defined after struct encoding is defined.
    void format(substitution_table& sub, std::string& out) const;

    static std::optional<special_name> parse(std::string_view& in)
    {
        if (in.starts_with("TV"))
        {
            in.remove_prefix(2);

            auto t = type::parse(in);
            if (!t)
                return {};

            return special_name{virtual_table{std::move(*t)}};
        }
        else if (in.starts_with("TT"))
        {
            in.remove_prefix(2);

            auto t = type::parse(in);
            if (!t)
                return {};

            return special_name{vtt{std::move(*t)}};
        }
        else if (in.starts_with("TI"))
        {
            in.remove_prefix(2);

            auto t = type::parse(in);
            if (!t)
                return {};

            return special_name{typeinfo{std::move(*t)}};
        }
        else if (in.starts_with("TS"))
        {
            in.remove_prefix(2);

            auto t = type::parse(in);
            if (!t)
                return {};

            return special_name{typeinfo_name{std::move(*t)}};
        }
        else if (in.starts_with("Tc"))
        {
            in.remove_prefix(1);

            auto t = type::parse(in);
            if (!t)
                return {};

            auto ca1 = call_offset::parse(in);
            if (!ca1)
                return {};

            auto ca2 = call_offset::parse(in);
            if (!ca2)
                return {};

            auto e = parse_encoding(in);
            if (!e)
                return {};

            return special_name{virtual_covariant_thunk{std::move(*ca1), std::move(*ca2), std::move(e)}};
        }
        else if (in.starts_with("T"))
        {
            in.remove_prefix(1);

            auto ca = call_offset::parse(in);
            if (!ca)
                return {};

            auto e = parse_encoding(in);
            if (!e)
                return {};

            return special_name{virtual_thunk{std::move(*ca), std::move(e)}};
        }
        else if (in.starts_with("GV"))
        {
            in.remove_prefix(2);

            auto n = name::parse(in);
            if (!n)
                return {};

            return special_name{guard_variable{std::move(*n)}};
        }
        else if (in.starts_with("GR"))
        {
            in.remove_prefix(2);

            auto n = name::parse(in);
            if (!n)
                return {};

            unsigned int id = 0;
            if (!in.starts_with("_"))
            {
                auto s = seq_id::parse(in);
                if (!s)
                    return {};

                id = s->index + 1;
            }

            if (!in.starts_with("_"))
                return {};

            in.remove_prefix(1);

            return special_name{temporary{id, std::move(*n)}};
        }
        else if (in.starts_with("GTt"))
        {
            in.remove_prefix(3);

            auto e = parse_encoding(in);
            if (!e)
                return {};

            return special_name{transaction_safe_entry{std::move(e)}};
        }

        return {};
    }
};

struct encoding
{
    struct function
    {
        name                function_name;
        bare_function_type  signature;
    };

    struct data
    {
        name                data_name;
    };

    using variant_t = std::variant
    <
        function,
        data,
        special_name
    >;

    variant_t   enc;

    void format(substitution_table& sub, std::string& out) const
    {
        std::visit([&]<class T>(const T& t)
        {
            if constexpr (std::is_same_v<T, function>)
            {
                t.signature.signature[0]->format(sub, out);
                out += " ";
                t.function_name.format(sub, out);
                out += "(";
                for (std::size_t i = 1; i < t.signature.signature.size(); ++i)
                {
                    const auto& tt = *t.signature.signature[i];
                    if (i != 1)
                        out += ", ";

                    tt.format(sub, out);
                }
                out += ")";
            }
            else if constexpr (std::is_same_v<T, data>)
            {
                t.data_name.format(sub, out);
            }
            else if constexpr (std::is_same_v<T, special_name>)
            {
                t.format(sub, out);
            }
            else
            {
                static_assert(!std::is_same_v<T, T>);
            }
        }, enc);
    }

    static std::optional<encoding> parse(std::string_view& in)
    {
        auto in_orig = in;

        if (auto sn = special_name::parse(in); sn)
        {
            return encoding{std::move(*sn)};
        }

        in = in_orig;

        auto n = name::parse(in);
        if (!n)
            return {};

        auto bft = bare_function_type::parse(in);

        if (bft)
            return encoding{function{std::move(*n), std::move(*bft)}};
        else
            return encoding{data{std::move(*n)}};
    }
};

struct mangled_name
{
    encoding                        name;
    std::optional<std::string_view> vendor_suffix;

    void format(substitution_table& sub, std::string& out) const
    {
        name.format(sub, out);
    }

    static std::optional<mangled_name> parse(std::string_view& in)
    {
        if (!in.starts_with("_Z"))
            return {};

        auto e = encoding::parse(in);
        if (!e)
            return {};

        std::optional<std::string_view> suffix;
        if (in.starts_with("."))
        {
            in.remove_prefix(1);
            suffix = in;
            in = {};
        }

        if (in.size() > 0)
            return {};

        return mangled_name{std::move(*e), std::move(suffix)};
    }
};


void exception_spec::format(substitution_table& sub, std::string& out) const
{
    std::visit([&]<class T>(const T& t)
    {
        if constexpr (std::is_same_v<T, non_throwing>)
        {
            out += "noexcept ";
        }
        else if constexpr (std::is_same_v<T, computed_noexcept>)
        {
            out += "noexcept(";
            t.e.format(sub, out);
            out += ") ";
        }
        else if constexpr (std::is_same_v<T, dynamic>)
        {
            bool first = true;
            out += "throw(";
            for (const auto& p : t.types)
            {
                if (!first)
                    out += ", ";
                else
                    first = false;

                p->format(sub, out);
            }
            out += ") ";
        }
        else
        {
            static_assert(!std::is_same_v<T, T>);
        }
    }, spec);
}

void special_name::format(substitution_table& sub, std::string& out) const
{
    std::visit([&]<class T>(const T& t)
    {
        if constexpr (std::is_same_v<T, virtual_table>)
        {
            out += "vtable for ";
            t.for_type.format(sub, out);
        }
        else if constexpr (std::is_same_v<T, vtt>)
        {
            out += "vtt for ";
            t.for_type.format(sub, out);
        }
        else if constexpr (std::is_same_v<T, typeinfo>)
        {
            out += "typeinfo for ";
            t.for_type.format(sub, out);
        }
        else if constexpr (std::is_same_v<T, typeinfo_name>)
        {
            out += "typeinfo name for ";
            t.for_type.format(sub, out);
        }
        else if constexpr (std::is_same_v<T, virtual_thunk>)
        {
            out += "non-virtual thunk to ";
            t.base->format(sub, out);
        }
        else if constexpr (std::is_same_v<T, virtual_covariant_thunk>)
        {
            out += "virtual thunk to ";
            t.base->format(sub, out);
        }
        else if constexpr (std::is_same_v<T, guard_variable>)
        {
            out += "guard variable for ";
            t.var_name.format(sub, out);
        }
        else if constexpr (std::is_same_v<T, temporary>)
        {
            out += std::format("temporary #{} for ", t.id);
            t.object.format(sub, out);
        }
        else if constexpr (std::is_same_v<T, transaction_safe_entry>)
        {
            out += "transaction safe entry for ";
            t.entry_for->format(sub, out);
        }
        else
        {
            static_assert(!std::is_same_v<T, T>);
        }
    }, sn);
}


static std::unique_ptr<type> parse_type(std::string_view& in)
{
    auto t = type::parse(in);
    if (t)
        return std::make_unique<type>(std::move(*t));

    return nullptr;
}

static std::unique_ptr<encoding> parse_encoding(std::string_view& in)
{
    auto e = encoding::parse(in);
    if (e)
        return std::make_unique<encoding>(std::move(*e));

    return nullptr;
}

static std::unique_ptr<name> parse_name(std::string_view& in)
{
    auto n = name::parse(in);
    if (n)
        return std::make_unique<name>(std::move(*n));

    return nullptr;
}

static std::unique_ptr<expression> parse_expression(std::string_view& in)
{
    auto e = expression::parse(in);
    if (e)
        return std::make_unique<expression>(std::move(*e));

    return nullptr;
}


static void format_type(const type& t, substitution_table& sub, std::string& out)
{
    t.format(sub, out);
}

static void format_encoding(const encoding& e, substitution_table& sub, std::string& out)
{
    e.format(sub, out);
}

static void format_name(const name& n, substitution_table& sub, std::string& out)
{
    n.format(sub, out);
}

static void format_expression(const expression& e, substitution_table& sub, std::string& out)
{
    e.format(sub, out);
}


static int demangleType(std::string_view input, std::string& output) try
{
    auto t = type::parse(input);
    if (!t)
        return DemangleInvalidName;
    
    substitution_table sub = {};
    t->format(sub, output);
    return DemangleSuccess;
}
catch (std::bad_alloc&)
{
    return DemangleMemoryAllocFailure;
}
catch (...)
{
    return DemangleInvalidName;
}


static int demangleName(std::string_view input, std::string& output) try
{
    auto m = mangled_name::parse(input);
    if (!m)
        return DemangleInvalidName;
    
    substitution_table sub = {};
    m->format(sub, output);
    return DemangleSuccess;
}
catch (std::bad_alloc&)
{
    return DemangleMemoryAllocFailure;
}
catch (...)
{
    return DemangleInvalidName;
}


} // anonymous namespace


} // namespace System::ABI::CXX


namespace __cxxabiv1
{


char* __cxa_demangle(const char* mangled_name, char* buf, std::size_t* n, int* status)
{
    using namespace System::ABI::CXX;

    int dummy_status;
    if (!status)
        status = &dummy_status;

    if (buf != nullptr && n == nullptr)
    {
        *status = DemangleInvalidArguments;
        return nullptr;
    }

    std::string output;
    std::string_view input {mangled_name};

    auto result = demangleName(input, output);
    if (result == DemangleInvalidName)
        result = demangleType(input, output);

    if (result != DemangleSuccess)
    {
        *status = result;
        return nullptr;
    }

    auto required_size = output.size() + 1;
    if (buf == nullptr || *n < required_size)
    {
        if (!__realloc)
        {
            *status = DemangleMemoryAllocFailure;
            return nullptr;
        }

        buf = reinterpret_cast<char*>(__realloc(buf, required_size));
        if (!buf)
        {
            *status = DemangleMemoryAllocFailure;
            return nullptr;
        }

        if (n)
            *n = required_size;
    }

    __memcpy(buf, output.data(), required_size);
    *status = DemangleSuccess;
    return buf;
}


} // namespace __cxxabiv1
