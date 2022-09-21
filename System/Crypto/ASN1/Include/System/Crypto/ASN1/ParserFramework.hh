#ifndef __SYSTEM_CRYPTO_ASN1_PARSERFRAMEWORK_H
#define __SYSTEM_CRYPTO_ASN1_PARSERFRAMEWORK_H


#include <cstddef>
#include <cstdio>
#include <cstdlib>


namespace System::Crypto::ASN1
{


struct SourceRange
{
    std::size_t start;
    std::size_t end;
};


struct ASTNode {};


class ParseContext
{
public:

    char getChar()
    {
        return std::getchar();
    }

    void error()
    {
        throw "oh noes";
    }

    void* allocate(std::size_t size)
    {
        return std::malloc(size);
    }

    void* reallocate(void* ptr, std::size_t size)
    {
        return std::realloc(ptr, size);
    }

    void free(void* ptr)
    {
        std::free(ptr);
    }

    void debug(int event, const char* rule, int level, std::size_t pos, const char* buffer, std::size_t size)
    {
        static const char *dbg_str[] = { "Evaluating rule", "Matched rule", "Abandoning rule" };
        //std::fprintf(stderr, "%*s%s %s @%lu [%.*s]\n", int(level * 2), "", dbg_str[event], rule, pos, int(size), buffer);
    }

    void syntaxError(SourceRange where, const char* desc, ...)
    {
        std::fprintf(stderr, "%zu-%zu: %s\n", where.start, where.end, desc);
    }
};


} // namespace System::Crypto::ASN1


#endif /* ifndef __SYSTEM_CRYPTO_ASN1_PARSERFRAMEWORK_H */
