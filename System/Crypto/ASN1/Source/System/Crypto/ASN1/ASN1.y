%code top {
#include <string_view>
}

// Requires features introduced in Bison 3.7.
%require "3.7"

// Use IELR table generation by default.
//
// This isn't needed but is recommended by the Bison manual.
%define lr.type ielr

// Enable lookahead correction to improve error messages.
%define parse.lac full

// Parser generation configuration:
//  - the parser should be C++, not C.
//  - always generate location tracking code.
//  - use std::string_view for wrapping file name strings.
%language "C++"
%locations
%define api.filename.type       {std::string_view}
%define api.namespace           {System::Crypto::Asn1}
%define api.parser.class        {Asn1Parser}
%define api.symbol.prefix       {SYM_}
%define api.token.constructor   true
%define api.token.prefix        {TOK_}
//%define api.value.automove      true
%define api.value.type          variant

// Debugging options:
%define parse.assert            true
%define parse.error             detailed
%define parse.trace             true

// The start symbol for the grammar is a fully-formed plan.
%start ModuleDefinition

// Terminal tokens - keywords.
%token ABSENT "ABSENT"
%token ABSTRACT_SYNTAX "ABSTRACT-SYNTAX"
%token ALL "ALL"
%token APPLICATION "APPLICATION"
%token AUTOMATIC "AUTOMATIC"
%token BEGIN "BEGIN"
%token BIT "BIT"
%token BMPSTRING "BMPString"
%token BOOLEAN "BOOLEAN"
%token BY "BY"
%token CHARACTER "CHARACTER"
%token CHOICE "CHOICE"
%token CLASS "CLASS"
%token COMPONENT "COMPONENT"
%token COMPONENTS "COMPONENTS"
%token CONSTRAINED "CONSTRAINED"
%token CONTAINING "CONTAINING"
%token DATE "DATE"
%token DATE_TIME "DATE-TIME"
%token DEFAULT "DEFAULT"
%token DEFINITIONS "DEFINITIONS"
%token DURATION "DURATION"
%token EMBEDDED "EMBEDDED"
%token ENCODED "ENCODED"
%token ENCODING_CONTROL "ENCODING-CONTROL"
%token END "END"
%token ENUMERATED "ENUMERATED"
%token EXCEPT "EXCEPT"
%token EXPLICIT "EXPLICIT"
%token EXPORTS "EXPORTS"
%token EXTENSIBILITY "EXTENSIBILITY"
%token EXTERNAL "EXTERNAL"
%token FALSE "FALSE"
%token FROM "FROM"
%token GENERALIZED_TIME "GeneralizedTime"
%token GENERAL_STRING "GeneralString"
%token GRAPHIC_STRING "GraphicString"
%token IA5STRING "IA5String"
%token IDENTIFIER "IDENTIFIER"
%token IMPLICIT "IMPLICIT"
%token IMPLIED "IMPLIED"
%token IMPORTS "IMPORTS"
%token INCLUDES "INCLUDES"
%token INSTANCE "INSTANCE"
%token INSTRUCTIONS "INSTRUCTIONS"
%token INTEGER "INTEGER"
%token INTERSECTION "INTERSECTION"
%token ISO646_STRING "ISO646String"
%token MAX "MAX"
%token MIN "MIN"
%token MINUS_INFINITY "MINUS_INFINITY"
%token NOT_A_NUMBER "NOT-A-NUMBER"
%token NULL "NULL"
%token NUMERIC_STRING "NumericString"
%token OBJECT "OBJECT"
%token OBJECT_DESCRIPTOR "ObjectDescriptor"
%token OCTET "OCTET"
%token OF "OF"
%token OID_IRI "OID-IRI"
%token OPTIONAL "OPTIONAL"
%token PATTERN "PATTERN"
%token PDV "PDV"
%token PLUS_INFINITY "PLUS-INFINITY"
%token PRESENT "PRESENT"
%token PRINTABLE_STRING "PrintableString"
%token PRIVATE "PRIVATE"
%token REAL "REAL"
%token RELATIVE_OID "RELATIVE-OID"
%token RELATIVE_OID_IRI "RELATIVE-OID-IRI"
%token SEQUENCE "SEQUENCE"
%token SET "SET"
%token SETTINGS "SETTINGS"
%token SIZE "SIZE"
%token STRING "STRING"
%token SYNTAX "SYNTAX"
%token T61_STRING "T61String"
%token TAGS "TAGS"
%token TELETEX_STRING "TeletexString"
%token TIME "TIME"
%token TIME_OF_DAY "TIME-OF-DAY"
%token TRUE "TRUE"
%token TYPE_IDENTIFIER "TYPE-IDENTIFIER"
%token UNION "UNION"
%token UNIQUE "UNIQUE"
%token UNIVERSAL "UNIVERSAL"
%token UNIVERSAL_STRING "UniversalString"
%token UTC_TIME "UTCTime"
%token UTF8_STRING "UTF8String"
%token VIDEOTEX_STRING "VideotexString"
%token VISIBLE_STRING "VisibleString"
%token WITH "WITH"

// Terminal tokens - multicharacter punctuation.
%token PUNCT_ASSIGN "::="
%token PUNCT_RANGE ".."
%token PUNCT_ELLIPSIS "..."
%token PUNCT_LVERSION "[["
%token PUNCT_RVERSION "]]"

// Terminal tokens - other.
%token LEX_WORD "word"      // just about anything that looks vaguely like an LEX_WORD.
%token LEX_NUM  "number"    // just about anything that looks vaguely like a number.
%token LEX_STRING "string"  // just about anything that looks vaguely like a string.
%token LEX_XML "xml"        // just about anything that looks vaguely like some XML.

// Non-terminal tokens - LEX_WORD subtypes.
%nterm xmlasn1typename

// Non-terminal tokens - string subtypes.
%nterm cstring
%nterm simplestring 

// Non-terminal tokens - XML subtypes.
%nterm xmlbstring
%nterm xmlcstring
%nterm xmlhstring
%nterm xmltstring

// Non-terminal tokens - misc primitives.
%nterm extendedfalse
%nterm extendedtrue

// Non-terminal tokens - defined outside ITU Rec X.690
%nterm GeneralConstraint
%nterm InstanceOfType
%nterm InstanceOfValue
%nterm ObjectAssignment
%nterm ObjectClassAssignment
%nterm ObjectClassFieldType
%nterm ObjectClassFieldValue
%nterm ObjectSetAssignment
%nterm ObjectSetElements
%nterm ParameterizedAssignment
%nterm ParameterizedReference
%nterm ParameterizedType
%nterm ParameterizedValue
%nterm ParameterizedValueSetType
%nterm TypeFromObject
%nterm ValueFromObject
%nterm ValueSetFromObjects
%nterm XMLInstanceOfValue
%nterm XMLObjectClassFieldValue

// Other non-terminals needing additional support.
%nterm EncodingInstruction
%nterm EncodingInstructionAssignmentList

// Token associativity and precedence (order of declaration, from lowest to highest).

// Semantic types for each of the tokens.

%%

ModuleDefinition:
    ModuleIdentifier DEFINITIONS EncodingReferenceDefault TagDefault ExtensionDefault PUNCT_ASSIGN BEGIN ModuleBody EncodingControlSections END
    ;

ModuleIdentifier:
    LEX_WORD DefinitiveIdentification
    ;

DefinitiveIdentification:
        %empty
    |   DefinitiveOID
    |   DefinitiveOIDandIRI
    ;

DefinitiveOID:
    '{' DefinitiveObjIdComponentList '}'
    ;

DefinitiveOIDandIRI:
    DefinitiveOID IRIValue
    ;

DefinitiveObjIdComponentList:
        DefinitiveObjIdComponent
    |   DefinitiveObjIdComponent DefinitiveObjIdComponentList
    ;

DefinitiveObjIdComponent:
        NameForm
    |   DefinitiveNumberForm
    |   DefinitiveNameAndNumberForm
    ;

DefinitiveNumberForm:
    LEX_NUM
    ;

DefinitiveNameAndNumberForm:
    LEX_WORD '{' DefinitiveNumberForm '}'
    ;

EncodingReferenceDefault:
        %empty
    |   LEX_WORD INSTRUCTIONS
    ;

TagDefault:
        %empty
    |   EXPLICIT TAGS
    |   IMPLICIT TAGS
    |   AUTOMATIC TAGS
    ;

ExtensionDefault:
        %empty
    |   EXTENSIBILITY IMPLIED
    ;

ModuleBody:
        %empty
    |   Exports Imports AssignmentList
    ;

Exports:
        %empty
    |   EXPORTS SymbolsExported ';'
    |   EXPORTS ALL ';'
    ;

SymbolsExported:
        %empty
    |   SymbolList
    ;

Imports:
        %empty
    |   IMPORTS SymbolsImported ';'
    ;

SymbolsImported:
        %empty
    |   SymbolsFromModuleList
    ;

SymbolsFromModuleList:
        SymbolsFromModule
    |   SymbolsFromModuleList SymbolsFromModule
    ;

SymbolsFromModule:
    SymbolList FROM GlobalModuleReference
    ;

GlobalModuleReference:
    LEX_WORD AssignedIdentifier
    ;

AssignedIdentifier:
        %empty
    |   ObjectIdentifierValue
    |   DefinedValue
    ;

SymbolList:
        Symbol
    |   Symbol ',' SymbolList
    ;

Symbol:
        Reference
    |   ParameterizedReference
    ;

Reference:
    LEX_WORD
    ;

AssignmentList:
        Assignment
    |   Assignment AssignmentList
    ;

Assignment:
        TypeAssignment
    |   ValueAssignment
    |   ValueSetTypeAssignment
    |   ObjectClassAssignment
    |   ObjectAssignment
    |   ObjectSetAssignment
    |   ParameterizedAssignment
    ;

DefinedType:
        ExternalTypeReference
    |   LEX_WORD
    |   ParameterizedType
    |   ParameterizedValueSetType
    ;

DefinedValue:
        ExternalValueReference
    |   LEX_WORD
    |   ParameterizedValue
    ;

NonParameterizedTypeName:
        ExternalTypeReference
    |   LEX_WORD
    |   xmlasn1typename
    ;

ExternalTypeReference:
    LEX_WORD '.' LEX_WORD
    ;

ExternalValueReference:
    LEX_WORD '.' LEX_WORD
    ;

AbsoluteReference:
    '@' ModuleIdentifier '.' ItemSpec
    ;

ItemSpec:
        LEX_WORD
    |   ItemId '.' ComponentId
    ;

ItemId:
    ItemSpec
    ;

ComponentId:
        LEX_WORD
    |   LEX_NUM
    |   '*'
    ;

TypeAssignment:
    LEX_WORD PUNCT_ASSIGN Type
    ;

// TODO parse XML properly. (LEX_XML was XMLTypedValue).
ValueAssignment:
        LEX_WORD Type PUNCT_ASSIGN Value
    |   LEX_WORD PUNCT_ASSIGN LEX_XML
    ;

// TODO check no whitespace in tags
XMLTypedValue:
        '<' NonParameterizedTypeName '>' XMLValue '<' '/' NonParameterizedTypeName '>'
    |   '<' NonParameterizedTypeName '/' '>'
    ;

ValueSetTypeAssignment:
    LEX_WORD Type PUNCT_ASSIGN ValueSet
    ;

ValueSet:
    '{' ElementSetSpecs '}'
    ;

Type:
        BuiltinType
    |   ReferencedType
    |   ConstrainedType
    ;

BuiltinType:
        BitStringType
    |   BooleanType
    |   CharacterStringType
    |   ChoiceType
    |   DateType
    |   DateTimeType
    |   DurationType
    |   EmbeddedPDVType
    |   EnumeratedType
    |   ExternalType
    |   InstanceOfType
    |   IntegerType
    |   IRIType
    |   NullType
    |   ObjectClassFieldType
    |   ObjectIdentifierType
    |   OctetStringType
    |   RealType
    |   RelativeIRIType
    |   RelativeOIDType
    |   SequenceType
    |   SequenceOfType
    |   SetType
    |   SetOfType
    |   PrefixedType
    |   TimeType
    |   TimeOfDayType
    ;

ReferencedType:
        DefinedType
    |   SelectionType
    |   TypeFromObject
    |   ValueSetFromObjects
    ;

NamedType:
    LEX_WORD Type
    ;

Value:
        BuiltinValue
    |   ReferencedValue
    |   ObjectClassFieldValue
    ;

XMLValue:
        XMLBuiltinValue
    |   XMLObjectClassFieldValue
    ;

BuiltinValue:
        BitStringValue
    |   BooleanValue
    |   CharacterStringValue
    |   ChoiceValue
    |   EmbeddedPDVValue
    |   EnumeratedValue
    |   ExternalValue
    |   InstanceOfValue
    |   IntegerOrRealValue
    |   IRIValue
    |   NullValue
    |   ObjectIdentifierValue
    |   OctetStringValue
    |   RelativeIRIValue
    |   RelativeOIDValue
    |   SequenceValue
    |   SequenceOfValue
    |   SetValue
    |   SetOfValue
    |   PrefixedValue
    |   TimeValue
    ;

XMLBuiltinValue:
        XMLBitStringValue
    |   XMLBooleanValue
    |   XMLCharacterStringValue
    |   XMLChoiceValue
    |   XMLEmbeddedPDVValue
    |   XMLEnumeratedValue
    |   XMLExternalValue
    |   XMLInstanceOfValue
    |   XMLIntegerValue
    |   XMLIRIValue
    |   XMLNullValue
    |   XMLObjectIdentifierValue
    |   XMLOctetStringValue
    |   XMLRealValue
    |   XMLRelativeIRIValue
    |   XMLRelativeOIDValue
    |   XMLSequenceValue
    |   XMLSequenceOfValue
    |   XMLSetValue
    |   XMLSetOfValue
    |   XMLPrefixedValue
    |   XMLTimeValue
    ;

ReferencedValue:
        DefinedValue
    |   ValueFromObject
    ;

NamedValue:
    LEX_WORD Value
    ;

// TODO check no whitespace in tags
XMLNamedValue:
    '<' LEX_WORD '>' XMLValue '<' '/' LEX_WORD '>'
    ;

BooleanType:
    BOOLEAN
    ;

BooleanValue:
        TRUE
    |   FALSE
    ;

XMLBooleanValue:
        EmptyElementBoolean
    |   TextBoolean
    ;

// TODO check no whitespace in tags
EmptyElementBoolean:
        '<' "true" '/' '>'
    |   '<' "false" '/' '>'
    ;

TextBoolean:
        extendedtrue
    |   extendedfalse
    ;

IntegerType:
        INTEGER
    |   INTEGER '{' NamedNumberList '}'
    ;

NamedNumberList:
        NamedNumber 
    |   NamedNumber ',' NamedNumberList
    ;

NamedNumber:
        LEX_WORD '(' SignedNumber ')'
    |   LEX_WORD '(' DefinedValue ')'
    ;

SignedNumber:
        LEX_NUM
    |   '-' LEX_NUM
    ;

XMLIntegerValue:
        XMLSignedNumber
    |   EmptyElementInteger
    |   TextInteger
    ;

// TODO check no whitespace in content
XMLSignedNumber:
        LEX_NUM
    |   '-' LEX_NUM
    ;

// TODO check no whitespace in tags
EmptyElementInteger:
    '<' LEX_WORD '/' '>'
    ;

TextInteger:
    LEX_WORD
    ;

EnumeratedType:
    ENUMERATED '{' Enumerations '}'
    ;

Enumerations:
        RootEnumeration
    |   RootEnumeration ',' PUNCT_ELLIPSIS ExceptionSpec
    |   RootEnumeration ',' PUNCT_ELLIPSIS ExceptionSpec ',' AdditionalEnumeration
    ;

RootEnumeration:
    Enumeration
    ;

AdditionalEnumeration:
    Enumeration
    ;

Enumeration:
        EnumerationItem
    |   EnumerationItem ',' Enumeration
    ;

EnumerationItem:
        LEX_WORD
    |   NamedNumber
    ;

EnumeratedValue:
    LEX_WORD
    ;

XMLEnumeratedValue:
        EmptyElementEnumerated
    |   TextEnumerated
    ;

// TODO check no whitespace in tags
EmptyElementEnumerated:
    '<' LEX_WORD '/' '>'
    ;

TextEnumerated:
    LEX_WORD
    ;

RealType:
    REAL
    ;

IntegerOrRealValue:
        NumericValue
    |   SpecialRealValue
    |   LEX_WORD
    ;

// Also SequenceValue (for reals) but that's handled directly.
NumericValue:
        LEX_NUM
    |   '-' LEX_NUM
    ;

SpecialRealValue:
        PLUS_INFINITY
    |   MINUS_INFINITY
    |   NOT_A_NUMBER
    ;

XMLRealValue:
        XMLNumericRealValue
    |   XMLSpecialRealValue
    ;

XMLNumericRealValue:
        LEX_NUM
    |   '-' LEX_NUM
    ;

XMLSpecialRealValue:
        EmptyElementReal
    |   TextReal
    ;

// TODO check no whitespace in tags
EmptyElementReal:
        '<' PLUS_INFINITY '/' '>'
    |   '<' MINUS_INFINITY '/' '>'
    |   '<' NOT_A_NUMBER '/' '>'
    ;

// TODO fixme
TextReal:
        "INF"
    |   "-INF"
    |   "NAN"
    ;

BitStringType:
        BIT STRING
    |   BIT STRING '{' NamedBitList '}'
    ;

NamedBitList:
        NamedBit
    |   NamedBitList ',' NamedBit
    ;

NamedBit:
        LEX_WORD '(' LEX_NUM ')'
    |   LEX_WORD '(' DefinedValue ')'
    ;

BitStringValue:
        LEX_NUM
    |   '{' IdentifierList '}'
    |   '{' '}'
    |   CONTAINING Value
    ;

IdentifierList:
        LEX_WORD
    |   IdentifierList ',' LEX_WORD
    ;

// TODO check no whitespace in tags
XMLBitStringValue:
        %empty
    |   XMLTypedValue
    |   xmlbstring
    |   XMLIdentifierList
    ;

XMLIdentifierList:
        EmptyElementList
    |   TextList
    ;

EmptyElementList:
        '<' LEX_WORD '/' '>'
   |    EmptyElementList '<' LEX_WORD '/' '>'
   ;

TextList:
        LEX_WORD
    |   TextList LEX_WORD
    ;

OctetStringType:
    OCTET STRING
    ;

OctetStringValue:
        LEX_NUM
    |   CONTAINING Value
    ;

XMLOctetStringValue:
        XMLTypedValue
    |   xmlhstring
    ;

NullType:
    NULL
    ;

NullValue:
    NULL
    ;

XMLNullValue:
    %empty
    ;

SequenceType:
        SEQUENCE '{' '}'
    |   SEQUENCE '{' ExtensionAndException OptionalExtensionMarker '}'
    |   SEQUENCE '{' ComponentTypeLists '}'
    ;

ExtensionAndException:
        PUNCT_ELLIPSIS
    |   PUNCT_ELLIPSIS ExceptionSpec
    ;

OptionalExtensionMarker:
        %empty
    |   ',' PUNCT_ELLIPSIS
    ;

ComponentTypeLists:
        RootComponentTypeList
    |   RootComponentTypeList ',' ExtensionAndException ExtensionAdditions OptionalExtensionMarker
    |   RootComponentTypeList ',' ExtensionAndException ExtensionAdditions ExtensionEndMarker ',' RootComponentTypeList
    |   ExtensionAndException ExtensionAdditions ExtensionEndMarker ',' RootComponentTypeList
    |   ExtensionAndException ExtensionAdditions OptionalExtensionMarker
    ;

RootComponentTypeList:
    ComponentTypeList
    ;

ExtensionEndMarker:
    ',' PUNCT_ELLIPSIS
    ;

ExtensionAdditions:
        %empty
    |   ',' ExtensionAdditionList
    ;

ExtensionAdditionList:
        ExtensionAddition
    |   ExtensionAdditionList ',' ExtensionAddition
    ;

ExtensionAddition:
        ComponentType
    |   ExtensionAdditionGroup
    ;

ExtensionAdditionGroup:
    PUNCT_LVERSION VersionNumber ComponentTypeList PUNCT_RVERSION
    ;

VersionNumber:
        %empty
    |   LEX_NUM ':'
    ;

ComponentTypeList:
        ComponentType
    |   ComponentTypeList ',' ComponentType
    ;

ComponentType:
        NamedType
    |   NamedType OPTIONAL
    |   NamedType DEFAULT Value
    |   COMPONENTS OF Type
    ;

SequenceValue:
        '{' ComponentValueList '}'
    |   '{' '}'
    ;

ComponentValueList:
        NamedValue
    |   ComponentValueList ',' NamedValue
    ;

XMLSequenceValue:
        %empty
    |   XMLComponentValueList
    ;

XMLComponentValueList:
        XMLNamedValue
    |   XMLComponentValueList XMLNamedValue
    ;

SequenceOfType:
        SEQUENCE OF Type
    |   SEQUENCE OF NamedType
    ;

SequenceOfValue:
        '{' ValueList '}'
    |   '{' NamedValueList ']'
    |   '{' '}'
    ;

ValueList:
        Value
    |   ValueList ',' Value
    ;

NamedValueList:
        NamedValue
    |   NamedValueList ',' NamedValue
    ;

XMLSequenceOfValue:
        %empty
    |   XMLValueList
    |   XMLDelimitedItemList
    ;

// TODO right recursion
XMLValueList:
        XMLValueOrEmpty
    |   XMLValueOrEmpty XMLValueList
    ;

// TODO check no whitespace in tags
XMLValueOrEmpty:
        XMLValue
    |   '<' NonParameterizedTypeName '/' '>'
    ;

XMLDelimitedItemList:
        XMLDelimitedItem
    |   XMLDelimitedItemList XMLDelimitedItem
    ;

// TODO check no whitespace in tags
XMLDelimitedItem:
        '<' NonParameterizedTypeName '>' XMLValue '<' '/' NonParameterizedTypeName '>'
    |   '<' LEX_WORD '>' XMLValue '<' '/' LEX_WORD '>'
    ;

SetType:
        SET '{' '}'
    |   SET '{' ExtensionAndException OptionalExtensionMarker '}'
    |   SET '{' ComponentTypeLists '}'
    ;

SetValue:
        '{' ComponentValueList '}'
    |   '{' '}'
    ;

XMLSetValue:
        %empty
    |   XMLComponentValueList
    ;

SetOfType:
        SET OF Type
    |   SET OF NamedType
    ;

SetOfValue:
        '{' ValueList '}'
    |   '{' NamedValueList '}'
    |   '{' '}'
    ;

XMLSetOfValue:
        %empty
    |   XMLValueList
    |   XMLDelimitedItemList
    ;

ChoiceType:
    CHOICE '{' AlternativeTypeLists '}'
    ;

AlternativeTypeLists:
        RootAlternativeTypeList
    |   RootAlternativeTypeList ',' ExtensionAndException ExtensionAdditionAlternatives OptionalExtensionMarker
    ;

RootAlternativeTypeList:
    AlternativeTypeList
    ;

ExtensionAdditionAlternatives:
        %empty
    |   ',' ExtensionAdditionAlternativesList
    ;

ExtensionAdditionAlternativesList:
        ExtensionAdditionAlternative
    |   ExtensionAdditionAlternativesList ',' ExtensionAdditionAlternative
    ;

ExtensionAdditionAlternative:
        ExtensionAdditionAlternativesGroup
    |   NamedType
    ;

ExtensionAdditionAlternativesGroup:
    PUNCT_LVERSION VersionNumber AlternativeTypeList PUNCT_RVERSION
    ;

AlternativeTypeList:
        NamedType
    |   AlternativeTypeList ',' NamedType
    ;

ChoiceValue:
    LEX_WORD ':' Value
    ;

// TODO check no whitespace in tags
XMLChoiceValue:
    '<' LEX_WORD '>' XMLValue '<' '/' LEX_WORD '>'
    ;

SelectionType:
    LEX_WORD '<' Type
    ;

PrefixedType:
        TaggedType
    |   EncodingPrefixedType
    ;

PrefixedValue:
    Value
    ;

XMLPrefixedValue:
    Value
    ;

TaggedType:
        Tag Type
    |   Tag IMPLICIT Type
    |   Tag EXPLICIT Type
    ;

Tag:
    '[' EncodingReference Class ClassNumber ']'
    ;

EncodingReference:
        %empty
    |   LEX_WORD ':'
    ;

ClassNumber:
        LEX_NUM
    |   DefinedValue
    ;

Class:
        %empty
    |   UNIVERSAL
    |   APPLICATION
    |   PRIVATE
    ;

EncodingPrefixedType:
    EncodingPrefix Type
    ;

EncodingPrefix:
    '[' EncodingReference EncodingInstruction ']'
    ;

ObjectIdentifierType:
    OBJECT IDENTIFIER
    ;

ObjectIdentifierValue:
        '{' ObjIdComponentsList '}'
    |   '{' DefinedValue ObjIdComponentsList '}'
    ;

ObjIdComponentsList:
        ObjIdComponents
    |   ObjIdComponentsList ObjIdComponents
    ;

ObjIdComponents:
        NameForm
    |   NumberForm
    |   NameAndNumberForm
    |   DefinedValue
    ;

NameForm:
    LEX_WORD
    ;

NumberForm:
        LEX_NUM
    |   DefinedValue
    ;

NameAndNumberForm:
    LEX_WORD '(' NumberForm ')'
    ;

XMLObjectIdentifierValue:
    XMLObjIdComponentList
    ;

// TODO check no whitespace in tags
XMLObjIdComponentList:
        XMLObjIdComponent
    |   XMLObjIdComponentList '.' XMLObjIdComponent
    ;

XMLObjIdComponent:
        NameForm
    |   XMLNumberForm
    |   XMLNameAndNumberForm
    ;

XMLNumberForm:
    LEX_NUM
    ;

// TODO check no whitespace in tags
XMLNameAndNumberForm:
    LEX_WORD '(' XMLNumberForm ')'
    ;

RelativeOIDType:
    RELATIVE_OID
    ;

RelativeOIDValue:
    '{' RelativeOIDComponentsList '}'
    ;

RelativeOIDComponentsList:
        RelativeOIDComponents
    |   RelativeOIDComponentsList RelativeOIDComponents
    ;

RelativeOIDComponents:
        NumberForm
    |   NameAndNumberForm
    |   DefinedValue
    ;

XMLRelativeOIDValue:
    XMLRelativeOIDComponentList
    ;

// TODO check no whitespace in tags
XMLRelativeOIDComponentList:
        XMLRelativeOIDComponent
    |   XMLRelativeOIDComponentList '.' XMLRelativeOIDComponent
    ;

XMLRelativeOIDComponent:
        XMLNumberForm
    |   XMLNameAndNumberForm
    ;

IRIType:
    OID_IRI
    ;

IRIValue:
    '"' FirstArcIdentifier SubsequentArcIdentifier '"'
    ;

FirstArcIdentifier:
    '/' ArcIdentifier
    ;

// FIXME right recursion
SubsequentArcIdentifier:
        %empty
    |   '/' ArcIdentifier SubsequentArcIdentifier
    ;

ArcIdentifier:
        LEX_NUM
    |   LEX_WORD
    ;

XMLIRIValue:
    FirstArcIdentifier SubsequentArcIdentifier
    ;

RelativeIRIType:
    RELATIVE_OID_IRI
    ;

RelativeIRIValue:
    '"' FirstRelativeArcIdentifier SubsequentArcIdentifier '"'
    ;

FirstRelativeArcIdentifier:
    ArcIdentifier
    ;

XMLRelativeIRIValue:
    FirstRelativeArcIdentifier SubsequentArcIdentifier
    ;

EmbeddedPDVType:
    EMBEDDED PDV
    ;

EmbeddedPDVValue:
    SequenceValue
    ;

XMLEmbeddedPDVValue:
    XMLSequenceValue
    ;

ExternalType:
    EXTERNAL
    ;

ExternalValue:
    SequenceValue
    ;

XMLExternalValue:
    XMLSequenceValue
    ;

TimeType:
    TIME
    ;

TimeValue:
    LEX_NUM
    ;

XMLTimeValue:
    xmltstring
    ;

DateType:
    DATE
    ;

TimeOfDayType:
    TIME_OF_DAY
    ;

DateTimeType:
    DATE_TIME
    ;

DurationType:
    DURATION
    ;

CharacterStringType:
        RestrictedCharacterStringType
    |   UnrestrictedCharacterStringType
    ;

CharacterStringValue:
        RestrictedCharacterStringValue
    |   UnrestrictedCharacterStringValue
    ;

XMLCharacterStringValue:
        XMLRestrictedCharacterStringValue
    |   XMLUnrestrictedCharacterStringValue
    ;

RestrictedCharacterStringType:
        BMPSTRING
    |   GENERAL_STRING
    |   GRAPHIC_STRING
    |   IA5STRING
    |   ISO646_STRING
    |   NUMERIC_STRING
    |   PRINTABLE_STRING
    |   TELETEX_STRING
    |   T61_STRING
    |   UNIVERSAL_STRING
    |   UTF8_STRING
    |   VIDEOTEX_STRING
    |   VISIBLE_STRING
    ;

RestrictedCharacterStringValue:
        cstring
    |   CharacterStringList
    |   Quadruple
    |   Tuple
    ;

CharacterStringList:
    '{' CharSyms '}'
    ;

CharSyms:
        CharsDefn
    |   CharSyms ',' CharsDefn
    ;

CharsDefn:
        cstring
    |   Quadruple
    |   Tuple
    |   DefinedValue
    ;

Quadruple:
    '{' Group ',' Plane ',' Row ',' Cell '}'
    ;

Group:
    LEX_NUM
    ;

Plane:
    LEX_NUM
    ;

Row:
    LEX_NUM
    ;

Cell:
    LEX_NUM
    ;

Tuple:
    '{' TableColumn ',' TableRow '}'
    ;

TableColumn:
    LEX_NUM
    ;

TableRow:
    LEX_NUM
    ;

XMLRestrictedCharacterStringValue:
    xmlcstring
    ;

UnrestrictedCharacterStringType:
    CHARACTER STRING
    ;

UnrestrictedCharacterStringValue:
    SequenceValue
    ;

XMLUnrestrictedCharacterStringValue:
    XMLSequenceValue
    ;

ConstrainedType:
        Type Constraint
    |   TypeWithConstraint
    ;

TypeWithConstraint:
        SET Constraint OF Type
    |   SET SizeConstraint OF Type
    |   SEQUENCE Constraint OF Type
    |   SEQUENCE SizeConstraint OF Type
    |   SET Constraint OF NamedType
    |   SET SizeConstraint OF NamedType
    |   SEQUENCE Constraint OF NamedType
    |   SEQUENCE SizeConstraint OF NamedType
    ;

Constraint:
    '(' ConstraintSpec ExceptionSpec ')'
    ;

ConstraintSpec:
        SubtypeConstraint
    |   GeneralConstraint
    ;

SubtypeConstraint:
    ElementSetSpecs
    ;

ElementSetSpecs:
        RootElementSetSpec
    |   RootElementSetSpec ',' PUNCT_ELLIPSIS
    |   RootElementSetSpec ',' PUNCT_ELLIPSIS ',' AdditionalElementSetSpec
    ;

RootElementSetSpec:
    ElementSetSpec
    ;

AdditionalElementSetSpec:
    ElementSetSpec
    ;

ElementSetSpec:
        Unions
    |   ALL Exclusions
    ;

Unions:
        Intersections
    |   UElems UnionMark Intersections
    ;

UElems:
    Unions
    ;

Intersections:
        IntersectionElements
    |   IElems IntersectionMark IntersectionElements
    ;

IElems:
    Intersections
    ;

IntersectionElements:
        Elements
    |   Elems Exclusions
    ;

Elems:
    Elements
    ;

Exclusions:
    EXCEPT Elements
    ;

UnionMark:
        '|'
    |   UNION
    ;

IntersectionMark:
        '^'
    |   INTERSECTION
    ;

Elements:
        SubtypeElements
    |   ObjectSetElements
    |   '(' ElementSetSpec ')'
    ;

SubtypeElements:
        SingleValue
    |   ContainedSubtypeOrTypeConstraint
    |   ValueRange
    |   PermittedAlphabet
    |   SizeConstraint
    |   InnerTypeConstraints
    |   PatternConstraint
    |   PropertySettings
    |   DurationRange
    |   TimePointRange
    |   RecurrenceRange
    ;

SingleValue:
    Value
    ;

ContainedSubtypeOrTypeConstraint:
    Includes Type
    ;

Includes:
        %empty
    |   INCLUDES
    ;

ValueRange:
    LowerEndpoint PUNCT_RANGE UpperEndpoint
    ;

LowerEndpoint:
        LowerEndValue
    |   LowerEndValue '<'
    ;
    
UpperEndpoint:
        UpperEndValue
    |   '<' UpperEndValue
    ;

LowerEndValue:
        Value
    |   MIN
    ;

UpperEndValue:
        Value
    |   MAX
    ;

SizeConstraint:
    SIZE Constraint
    ;

PermittedAlphabet:
    FROM Constraint
    ;

InnerTypeConstraints:
        WITH COMPONENT SingleTypeConstraint
    |   WITH COMPONENTS MultipleTypeConstraints
    ;

SingleTypeConstraint:
    Constraint
    ;

MultipleTypeConstraints:
        FullSpecification
    |   PartialSpecification
    ;

FullSpecification:
    '{' TypeConstraints '}'
    ;

PartialSpecification:
    '{' PUNCT_ELLIPSIS ',' TypeConstraints '}'
    ;

TypeConstraints:
        NamedConstraint
    |   TypeConstraints ',' NamedConstraint
    ;

NamedConstraint:
    LEX_WORD ComponentConstraint
    ;

ComponentConstraint:
    ValueConstraint PresenceConstraint
    ;

ValueConstraint:
        %empty
    |   Constraint
    ;

PresenceConstraint:
        %empty
    |   PRESENT
    |   ABSENT
    |   OPTIONAL
    ;

PatternConstraint:
    PATTERN Value
    ;

PropertySettings:
    SETTINGS simplestring
    ;

PropertySettingsList:
        PropertyAndSettingPair
    |   PropertySettingsList PropertyAndSettingPair
    ;

PropertyAndSettingPair:
    PropertyName '=' SettingName
    ;

PropertyName:
    LEX_WORD
    ;

SettingName:
    LEX_WORD
    ;

DurationRange:
    ValueRange
    ;

TimePointRange:
    ValueRange
    ;

RecurrenceRange:
    ValueRange
    ;

ExceptionSpec:
        %empty
    |   '!' ExceptionIdentification
    ;

ExceptionIdentification:
        SignedNumber
    |   DefinedValue
    |   Type ':' Value
    ;

EncodingControlSections:
        %empty
    |   EncodingControlSections EncodingControlSection
    ;

EncodingControlSection:
    ENCODING_CONTROL LEX_WORD EncodingInstructionAssignmentList
    ;


// ---------- pseudo-terminals ---------- //


xmlasn1typename:
    LEX_WORD
    ;
