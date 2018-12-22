// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// serdefs.h

/**
 * Basic definitions and macros for the serialization layer.
 **/

#ifndef INCLUDED_SERDEFS_H
#define INCLUDED_SERDEFS_H

class Serializer; // fwd
namespace SerializeHelper { } // fwd

/**
 * SerializeTag enumeration
 *
 * The enumeration must fit in a uint8.
 **/

enum SerializeTag
{
  // numeric
  eTagInt8,
  eTagUInt8,
  eTagInt16,
  eTagUInt16,
  eTagInt32,
  eTagUInt32,
  eTagInt64,
  eTagUInt64,
  eTagFloat32,
  eTagFloat64,
  eTagBool,

  // array-like types
  eTagString,
  eTagArray,
  eTagRawBytes,

  // STL containers (including non-standard hashed varieties)
  eTagPair,
  eTagVector,
  eTagList,
  eTagDeque,
  eTagSet,
  eTagHashSet,
  eTagMultiSet,
  eTagHashMultiSet,
  eTagMap,
  eTagHashMap,
  eTagMultiMap,
  eTagHashMultiMap,

  // misc
  eTagCount,

  // builtin classes
  eTagBuffer,
  eTagKeyedSerializer,

  // any user-defined class
  eTagUserClass,

  // Add here, to avoid reordering.  Or, use eTagUserClass for all
  // application-specific classes to avoid any recompiling.

  eTagLast_, // sentinel

}; // enum SerializeTag

// The following macros simplify the declaration and definition of the
// SerializeHelper functions (ComputeTag, PutInto, GetFrom).
//
// This is one of those rare occasions where (ab)using the
// preprocessor can make things more readable.

/**
 * Define inline ComputeTag() function for given type.
 **/

#define SERIALIZE_DEFINE_TAG(typ, tag) \
  namespace SerializeHelper \
  { \
    inline SerializeTag ComputeTag(const typ&) { return tag; } \
  }

/**
 * Forward-declare PutInto() and GetFrom() for given type.
 **/

#define SERIALIZE_DECLARE_PUTGET(typ) \
  namespace SerializeHelper \
  { \
    bool PutInto(Serializer& ser, const typ& obj); \
    bool GetFrom(Serializer& ser, typ& obj); \
  }

/**
 * Declare PutInto() and GetFrom() for the given class as friend
 * functions of the class.  Must be put inside class declaration.
 **/

#define SERIALIZE_DECLARE_FRIEND(typ) \
  friend bool SerializeHelper::PutInto(Serializer&, const typ&); \
  friend bool SerializeHelper::GetFrom(Serializer&, typ&)

/**
 * Define (implement) PutInto() for given type.  Must be followed by
 * code block (i.e. this macro replaces the function header).
 *
 * Usage:
 *
 * SERIALIZE_DEFINE_PUT(Foo, obj, ser)
 * {
 *   // serialize obj (of type const Foo&) into ser (of type Serializer&)
 *   // return a bool
 * }
 **/

#define SERIALIZE_DEFINE_PUT(typ, obj, ser) \
  bool SerializeHelper::PutInto(Serializer& ser, const typ& obj)

/**
 * Define (implement) GetFrom() for given type.  Must be followed by
 * code block (i.e. this macro replaces the function header).
 *
 * Usage:
 *
 * SERIALIZE_DEFINE_GET(Foo, obj, ser)
 * {
 *   // unserialize obj (of type Foo&) from ser (of type Serializer&)
 *   // return a bool
 * }
 **/

#define SERIALIZE_DEFINE_GET(typ, obj, ser) \
  bool SerializeHelper::GetFrom(Serializer& ser, typ& obj)

#endif // !INCLUDED_SERDEFS_H
