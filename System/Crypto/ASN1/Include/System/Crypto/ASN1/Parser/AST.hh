#ifndef __SYSTEM_CRYPTO_ASN1_PARSER_AST_H
#define __SYSTEM_CRYPTO_ASN1_PARSER_AST_H


namespace System::Crypto::ASN1::Parser
{


enum class NodeType
{
    Invalid = 0,

    ModuleDefinitions,
    ModuleDefinition,
    ModuleIdentifier,
    DefinitiveIdentification,
    DefinitiveOID,
    DefinitiveObjIdComponentList,
    DefinitiveObjIdComponent,
};


class ASTNode
{
public:

    NodeType type() const noexcept;
};


template <NodeType T>
class ASTNodeImpl :
    public ASTNode
{
public:

    static constexpr NodeType Type = T;
};


template <class T>
T* ASTCast(ASTNode* node)
{
    if (node->type() != T::Type)
    {
        return nullptr;
    }

    return static_cast<T*>(node);
}


} // namespace System::Crypto::ASN1::Parser


#endif /* ifndef __SYSTEM_CRYPTO_ASN1_PARSER_AST_H */
