/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Json.h
 * A simple set of classes for generating JSON.
 * See http://www.json.org/
 * Copyright (C) 2012 Simon Newton
 */

/**
 * @addtogroup json
 * @{
 * @file Json.h
 * @brief Basic data types used to represent elements in a JSON document.
 * @}
 */

#ifndef INCLUDE_OLA_WEB_JSON_H_
#define INCLUDE_OLA_WEB_JSON_H_

#include <ola/StringUtils.h>
#include <ola/base/Macro.h>
#include <ola/web/JsonPointer.h>
#include <stdint.h>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace ola {
namespace web {

/**
 * @addtogroup json
 * @{
 */

class JsonValueVisitorInterface;
class JsonObjectPropertyVisitor;

class JsonArray;
class JsonBoolValue;
class JsonDoubleValue;
class JsonInt64Value;
class JsonIntValue;
class JsonNullValue;
class JsonNumberValue;
class JsonObject;
class JsonRawValue;
class JsonStringValue;
class JsonUInt64Value;
class JsonUIntValue;

/**
 * @brief The base class for JSON values.
 */
class JsonValue {
 public:
  virtual ~JsonValue() {}

  /**
   * @brief Locate the JsonValue referred to by the JSON Pointer.
   */
  virtual JsonValue* LookupElement(const JsonPointer &pointer);

  /**
   * @brief Equality operator.
   *
   * This implements equality as defined in section 3.6 of the JSON Schema Core
   * document.
   */
  virtual bool operator==(const JsonValue &other) const = 0;

  /**
   * @brief Not-equals operator.
   */
  virtual bool operator!=(const JsonValue &other) const {
    return !(*this == other);
  }

  /**
   * @brief The Accept method for the visitor pattern.
   *
   * This can be used to traverse the Json Tree in a type-safe manner.
   */
  virtual void Accept(JsonValueVisitorInterface *visitor) const = 0;

  /**
   * @brief Make a copy of this JsonValue.
   */
  virtual JsonValue* Clone() const = 0;

  /**
   * @privatesection
   */

  /**
   * @brief Lookup the Value referred to by the Iterator.
   *
   * This is used by recursively by JsonValue classes. You should call
   * LookupElement() instead.
   */
  virtual JsonValue* LookupElementWithIter(JsonPointer::Iterator *iterator) = 0;

  /**
   * @brief Check if this JsonValue equals a JsonStringValue.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonStringValue &) const { return false; }

  /**
   * @brief Check if this JsonValue equals a JsonUIntValue.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonUIntValue &) const { return false; }

  /**
   * @brief Check if this JsonValue equals a JsonIntValue.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonIntValue &) const { return false; }

  /**
   * @brief Check if this JsonValue equals a JsonUInt64Value.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonUInt64Value &) const { return false; }

  /**
   * @brief Check if this JsonValue equals a JsonInt64Value.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonInt64Value &) const { return false; }

  /**
   * @brief Check if this JsonValue equals a JsonBoolValue.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonBoolValue &) const { return false; }

  /**
   * @brief Check if this JsonValue equals a JsonNullValue.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonNullValue &) const { return false; }

  /**
   * @brief Check if this JsonValue equals a JsonDoubleValue.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonDoubleValue &) const { return false; }

  /**
   * @brief Check if this JsonValue equals a JsonRawValue.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonRawValue &) const { return false; }

  /**
   * @brief Check if this JsonValue equals a JsonObject.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonObject &) const { return false; }

  /**
   * @brief Check if this JsonValue equals a JsonArray.
   * @returns true if the two values are equal, false otherwise.
   */
  virtual bool Equals(const JsonArray &) const { return false; }

  /**
   * @endsection
   */

  /**
   * @brief Given a variable, create a new JsonValue of the appropriate type.
   */
  template <typename T>
  static JsonValue* NewValue(const T &value);

  /**
   * @brief Given a variable, create a new JsonNumberValue of the appropriate
   * type.
   */
  template <typename T>
  static JsonNumberValue* NewNumberValue(const T &value);
};


/**
 * @brief A base class used to describe values which are leafs of a JSON tree.
 *
 * Leaf values are those which don't contain other values. All values except
 * JsonObject and JsonArray are leaf values.
 */
class JsonLeafValue : public JsonValue {
 public:
  /**
   * @privatesection
   */
  JsonValue* LookupElementWithIter(JsonPointer::Iterator *iter);
  /**
   * @endsection
   */
};

/**
 * @brief A string value.
 */
class JsonStringValue: public JsonLeafValue {
 public:
  /**
   * @brief Create a new JsonStringValue
   * @param value the string to use.
   */
  explicit JsonStringValue(const std::string &value) : m_value(value) {}

  bool operator==(const JsonValue &other) const { return other.Equals(*this); }

  /**
   * @brief Return the string value.
   */
  const std::string& Value() const { return m_value; }

  void Accept(JsonValueVisitorInterface *visitor) const;

  JsonValue* Clone() const { return new JsonStringValue(m_value); }

  /**
   * @privatesection
   */
  bool Equals(const JsonStringValue &other) const {
    return m_value == other.m_value;
  }
  /**
   * @endsection
   */

 private:
  const std::string m_value;

  DISALLOW_COPY_AND_ASSIGN(JsonStringValue);
};


/**
 * @brief JsonNumberValue is the base class for various integer / number
 * classes.
 *
 * This allows inequality comparisons between values that represent numbers.
 */
class JsonNumberValue : public JsonLeafValue {
 public:
  /**
   * @brief Less than operator.
   */
  virtual bool operator<(const JsonNumberValue &other) const = 0;

  /**
   * @brief Less than or equals operator.
   */
  bool operator<=(const JsonNumberValue &other) const {
    return *this == other || *this < other;
  }

  /**
   * @brief Greater than operator.
   */
  bool operator>(const JsonNumberValue &other) const {
    return !(*this <= other);
  }

  /**
   * @brief Greater than or equals operator.
   */
  bool operator>=(const JsonNumberValue &other) const {
    return !(*this < other);
  }

  /**
   * @privatesection
   */
  virtual int Compare(const JsonUIntValue &value) const = 0;
  virtual int Compare(const JsonIntValue &value) const = 0;
  virtual int Compare(const JsonUInt64Value &value) const = 0;
  virtual int Compare(const JsonInt64Value &value) const = 0;
  virtual int Compare(const JsonDoubleValue &value) const = 0;
  /**
   * @endsection
   */
};

/**
 * @brief An unsigned 32bit int value.
 */
class JsonUIntValue: public JsonNumberValue {
 public:
  /**
   * @brief Create a new JsonUIntValue
   * @param value the unsigned int to use.
   */
  explicit JsonUIntValue(unsigned int value) : m_value(value) {}

  bool operator==(const JsonValue &other) const { return other.Equals(*this); }

  bool operator<(const JsonNumberValue &other) const {
    return other.Compare(*this) == 1;
  }

  void Accept(JsonValueVisitorInterface *visitor) const;

  JsonValue* Clone() const { return new JsonUIntValue(m_value); }

  /**
   * @brief Return the uint32_t value.
   */
  unsigned int Value() const { return m_value; }

  /**
   * @privatesection
   */
  bool Equals(const JsonUIntValue &other) const {
    return m_value == other.m_value;
  }

  bool Equals(const JsonIntValue &other) const;
  bool Equals(const JsonUInt64Value &other) const;
  bool Equals(const JsonInt64Value &other) const;

  int Compare(const JsonUIntValue &value) const;
  int Compare(const JsonIntValue &value) const;
  int Compare(const JsonUInt64Value &value) const;
  int Compare(const JsonInt64Value &value) const;
  int Compare(const JsonDoubleValue &value) const;
  /**
   * @endsection
   */

 private:
  const unsigned int m_value;

  DISALLOW_COPY_AND_ASSIGN(JsonUIntValue);
};


/**
 * @brief A signed 32bit int value.
 */
class JsonIntValue: public JsonNumberValue {
 public:
  /**
   * @brief Create a new JsonIntValue
   * @param value the int to use.
   */
  explicit JsonIntValue(int value) : m_value(value) {}

  bool operator==(const JsonValue &other) const { return other.Equals(*this); }

  bool operator<(const JsonNumberValue &other) const {
    return other.Compare(*this) == 1;
  }

  void Accept(JsonValueVisitorInterface *visitor) const;

  JsonValue* Clone() const { return new JsonIntValue(m_value); }

  /**
   * @brief Return the int32_t value.
   */
  int Value() const { return m_value; }

  /**
   * @privatesection
   */
  bool Equals(const JsonIntValue &other) const {
    return m_value == other.m_value;
  }

  bool Equals(const JsonUIntValue &other) const;
  bool Equals(const JsonUInt64Value &other) const;
  bool Equals(const JsonInt64Value &other) const;

  int Compare(const JsonUIntValue &value) const;
  int Compare(const JsonIntValue &value) const;
  int Compare(const JsonUInt64Value &value) const;
  int Compare(const JsonInt64Value &value) const;
  int Compare(const JsonDoubleValue &value) const;
  /**
   * @endsection
   */
 private:
  const int m_value;

  DISALLOW_COPY_AND_ASSIGN(JsonIntValue);
};


/**
 * @brief An unsigned 64bit int value.
 */
class JsonUInt64Value: public JsonNumberValue {
 public:
  /**
   * @brief Create a new JsonUInt64Value
   * @param value the unsigned int 64 to use.
   */
  explicit JsonUInt64Value(uint64_t value) : m_value(value) {}

  bool operator==(const JsonValue &other) const {
    return other.Equals(*this);
  }

  bool operator<(const JsonNumberValue &other) const {
    return other.Compare(*this) == 1;
  }

  void Accept(JsonValueVisitorInterface *visitor) const;

  JsonValue* Clone() const { return new JsonUInt64Value(m_value); }

  /**
   * @brief Return the uint64_t value.
   */
  uint64_t Value() const { return m_value; }

  /**
   * @privatesection
   */
  bool Equals(const JsonUInt64Value &other) const {
    return m_value == other.m_value;
  }

  bool Equals(const JsonUIntValue &other) const;
  bool Equals(const JsonIntValue &other) const;
  bool Equals(const JsonInt64Value &other) const;

  int Compare(const JsonUIntValue &value) const;
  int Compare(const JsonIntValue &value) const;
  int Compare(const JsonUInt64Value &value) const;
  int Compare(const JsonInt64Value &value) const;
  int Compare(const JsonDoubleValue &value) const;
  /**
   * @endsection
   */
 private:
  const uint64_t m_value;

  DISALLOW_COPY_AND_ASSIGN(JsonUInt64Value);
};


/**
 * @brief A signed 64bit int value.
 */
class JsonInt64Value: public JsonNumberValue {
 public:
  /**
   * @brief Create a new JsonInt64Value
   * @param value the int 64 to use.
   */
  explicit JsonInt64Value(int64_t value) : m_value(value) {}

  bool operator==(const JsonValue &other) const {
    return other.Equals(*this);
  }

  bool operator<(const JsonNumberValue &other) const {
    return other.Compare(*this) == 1;
  }

  void Accept(JsonValueVisitorInterface *visitor) const;

  JsonValue* Clone() const { return new JsonInt64Value(m_value); }

  /**
   * @brief Return the int64_t value.
   */
  int64_t Value() const { return m_value; }

  /**
   * @privatesection
   */
  bool Equals(const JsonInt64Value &other) const {
    return m_value == other.m_value;
  }

  bool Equals(const JsonUIntValue &other) const;
  bool Equals(const JsonIntValue &other) const;
  bool Equals(const JsonUInt64Value &other) const;

  int Compare(const JsonUIntValue &value) const;
  int Compare(const JsonIntValue &value) const;
  int Compare(const JsonUInt64Value &value) const;
  int Compare(const JsonInt64Value &value) const;
  int Compare(const JsonDoubleValue &value) const;
  /**
   * @endsection
   */
 private:
  const int64_t m_value;

  DISALLOW_COPY_AND_ASSIGN(JsonInt64Value);
};


/**
 * @brief A double value.
 *
 * Double Values represent numbers which are not simple integers. They can have
 * a fractional and/or exponent. A double
 * value takes the form: [full].[fractional]e<sup>[exponent]</sup>.
 * e.g 23.00456e<sup>-3</sup>.
 */
class JsonDoubleValue: public JsonNumberValue {
 public:
  /**
   * @struct DoubleRepresentation
   * @brief Represents a JSON double value broken down as separate components.
   *
   * For the value 23.00456e<sup>-3</sup>:
   *   full: 23
   *   leading_fractional_zeros: 2
   *   fractional: 456
   *   exponent: -3
   */
  struct DoubleRepresentation {
    /** The sign of the double, true is negative, false is postive */
    bool is_negative;
    /** The number to the left of the decimal point */
    uint64_t full;
    /** The number of leading 0s in the fractional part of the double */
    uint32_t leading_fractional_zeros;
    /** The fractional part of the double, without the leading 0s */
    uint64_t fractional;
    /** The exponent, or 0 if there isn't one. */
    int32_t exponent;
  };

  /**
   * @brief Create a new JsonDoubleValue
   * @param value the double to use.
   */
  explicit JsonDoubleValue(double value);

  /**
   * @brief Create a new JsonDoubleValue from separate components.
   */
  explicit JsonDoubleValue(const DoubleRepresentation &rep);

  bool operator==(const JsonValue &other) const {
    return other.Equals(*this);
  }

  bool operator<(const JsonNumberValue &other) const {
    return other.Compare(*this) == 1;
  }

  void Accept(JsonValueVisitorInterface *visitor) const;

  /**
   * @brief Return the double value as a string.
   */
  const std::string& ToString() const { return m_as_string; }

  /**
   * @brief Returns the value as a double.
   *
   * This may be incorrect if the value exceeds the storage space of the double.
   */
  double Value() const { return m_value; }

  /**
   * @brief Convert a DoubleRepresentation to a double value.
   * @param rep the DoubleRepresentation
   * @param[out] The value stored as a double.
   * @returns false if the DoubleRepresentation can't fit in a double.
   */
  static bool AsDouble(const DoubleRepresentation &rep, double *out);

  /**
   * @brief Convert the DoubleRepresentation to a string.
   * @param rep the DoubleRepresentation
   * @returns The string representation of the DoubleRepresentation.
   */
  static std::string AsString(const DoubleRepresentation &rep);

  JsonValue* Clone() const {
    // This loses precision, maybe we should fix it?
    return new JsonDoubleValue(m_value);
  }

  /**
   * @privatesection
   */
  bool Equals(const JsonDoubleValue &other) const {
    // This is sketchy. The standard says "have the same mathematical value"
    // TODO(simon): Think about this some more.
    return m_value == other.m_value;
  }

  int Compare(const JsonUIntValue &value) const;
  int Compare(const JsonIntValue &value) const;
  int Compare(const JsonUInt64Value &value) const;
  int Compare(const JsonInt64Value &value) const;
  int Compare(const JsonDoubleValue &value) const;
  /**
   * @endsection
   */
 private:
  double m_value;
  std::string m_as_string;

  DISALLOW_COPY_AND_ASSIGN(JsonDoubleValue);
};


/**
 * @brief A Bool value
 */
class JsonBoolValue: public JsonLeafValue {
 public:
  /**
   * @brief Create a new JsonBoolValue
   * @param value the bool to use.
   */
  explicit JsonBoolValue(bool value) : m_value(value) {}

  bool operator==(const JsonValue &other) const {
    return other.Equals(*this);
  }

  void Accept(JsonValueVisitorInterface *visitor) const;

  JsonValue* Clone() const { return new JsonBoolValue(m_value); }

  /**
   * @brief Return the bool value.
   */
  bool Value() const { return m_value; }

  /**
   * @privatesection
   */
  bool Equals(const JsonBoolValue &other) const {
    return m_value == other.m_value;
  }
  /**
   * @endsection
   */
 private:
  const bool m_value;

  DISALLOW_COPY_AND_ASSIGN(JsonBoolValue);
};


/**
 * @brief The null value
 */
class JsonNullValue: public JsonLeafValue {
 public:
  /**
   * @brief Create a new JsonNullValue
   */
  explicit JsonNullValue() {}

  void Accept(JsonValueVisitorInterface *visitor) const;

  JsonValue* Clone() const { return new JsonNullValue(); }

  bool operator==(const JsonValue &other) const { return other.Equals(*this); }

  /**
   * @privatesection
   */
  bool Equals(const JsonNullValue &) const { return true; }
  /**
   * @endsection
   */
 private:
  DISALLOW_COPY_AND_ASSIGN(JsonNullValue);
};


/**
 * @brief A raw value, useful if you want to cheat.
 */
class JsonRawValue: public JsonLeafValue {
 public:
  /**
   * @brief Create a new JsonRawValue
   * @param value the raw data to insert.
   */
  explicit JsonRawValue(const std::string &value) : m_value(value) {}

  bool operator==(const JsonValue &other) const { return other.Equals(*this); }

  void Accept(JsonValueVisitorInterface *visitor) const;

  JsonValue* Clone() const { return new JsonRawValue(m_value); }

  /**
   * @brief Return the raw value  as a string.
   */
  const std::string& Value() const { return m_value; }

  /**
   * @privatesection
   */
  bool Equals(const JsonRawValue &other) const {
    return m_value == other.m_value;
  }
  /**
   * @endsection
   */

 private:
  const std::string m_value;

  DISALLOW_COPY_AND_ASSIGN(JsonRawValue);
};


/**
 * @brief A JSON object.
 * JSON Objects are key : value mappings, similar to dictionaries in Python.
 *
 * If the same key is added more than once, the latest value wins.
 *
 * @todo Since key names tend to reuse the same strings, it would be nice to
 * intern the strings here. That's a future optimization for someone.
 */
class JsonObject: public JsonValue {
 public:
  /**
   * @brief Create a new JsonObject
   */
  JsonObject() {}
  ~JsonObject();

  JsonValue* LookupElementWithIter(JsonPointer::Iterator *iter);

  bool operator==(const JsonValue &other) const {
    return other.Equals(*this);
  }

  bool Equals(const JsonObject &other) const;

  /**
   * @brief Add a key to string mapping.
   * @param key the key to set.
   * @param value the value to add.
   */
  void Add(const std::string &key, const std::string &value);

  /**
   * @brief Set the given key to a string value.
   * @param key the key to set.
   * @param value the value to add
   */
  void Add(const std::string &key, const char *value);

  /**
   * @brief Set the given key to a unsigned int value.
   * @param key the key to set.
   * @param i the value to add
   */
  void Add(const std::string &key, unsigned int i);

  /**
   * @brief Set the given key to a int value.
   * @param key the key to set.
   * @param i the value to add
   */
  void Add(const std::string &key, int i);

  /**
   * @brief Set the given key to a double value.
   * @param key the key to set.
   * @param d the value to add
   */
  void Add(const std::string &key, double d);

  /**
   * @brief Set the given key to a bool value.
   * @param key the key to set.
   * @param value the value to add
   */
  void Add(const std::string &key, bool value);

  /**
   * @brief Set the given key to a null value.
   * @param key the key to set.
   */
  void Add(const std::string &key);

  /**
   * @brief Set the given key to a JsonObject.
   * @param key the key to add
   * @returns the new JsonObject.
   */
  JsonObject* AddObject(const std::string &key);

  /**
   * @brief Set the given key to a JsonArray.
   * @param key the key to add
   * @returns the new JsonObject.
   */
  class JsonArray* AddArray(const std::string &key);

  /**
   * @brief Set the key to the supplied JsonValue.
   * @param key the key to add
   * @param value the JsonValue object, ownership is transferred.
   */
  void AddValue(const std::string &key, JsonValue *value);

  /**
   * @brief Set the given key to a raw value.
   * @param key the key to add
   * @param value the raw value to append.
   */
  void AddRaw(const std::string &key, const std::string &value);

  void Accept(JsonValueVisitorInterface *visitor) const;

  JsonValue* Clone() const;

  /**
   * @brief Check if there are properties within the object
   * @returns true if the object is empty, false if there are properties.
   */
  bool IsEmpty() const { return m_members.empty(); }

  unsigned int Size() const { return m_members.size(); }

  /**
   * @brief Visit each of the properties in this object.
   *
   * For each property : value, the visitor is called.
   */
  void VisitProperties(JsonObjectPropertyVisitor *visitor) const;

 private:
  typedef std::map<std::string, JsonValue*> MemberMap;
  MemberMap m_members;

  DISALLOW_COPY_AND_ASSIGN(JsonObject);
};


/**
 * @brief An array of JSON values.
 * Arrays in JSON can contain values of different types.
 */
class JsonArray: public JsonValue {
 public:
  JsonArray() : m_complex_type(false) {}
  ~JsonArray();

  JsonValue* LookupElementWithIter(JsonPointer::Iterator *iter);

  bool operator==(const JsonValue &other) const {
    return other.Equals(*this);
  }

  bool Equals(const JsonArray &other) const;

  /**
   * @brief Append a string value to the array
   * @param value the value to append
   */
  void Append(const std::string &value) {
    m_values.push_back(new JsonStringValue(value));
  }

  /**
   * @brief Append a string value to the array
   * @param value the value to append
   */
  void Append(const char *value) {
    m_values.push_back(new JsonStringValue(value));
  }

  /**
   * @brief Append an unsigned int value to the array
   * @param i the value to append
   */
  void Append(unsigned int i) {
    m_values.push_back(new JsonUIntValue(i));
  }

  /**
   * @brief Append an int value to the array
   * @param i the value to append
   */
  void Append(int i) {
    m_values.push_back(new JsonIntValue(i));
  }

  /**
   * @brief Append a bool value to the array
   * @param value the value to append
   */
  void Append(bool value) {
    m_values.push_back(new JsonBoolValue(value));
  }

  /**
   * @brief Append a null value to the array
   */
  void Append() {
    m_values.push_back(new JsonNullValue());
  }

  /**
   * @brief Append a JsonValue. Takes ownership of the pointer.
   */
  void Append(JsonValue *value) {
    m_values.push_back(value);
  }

  /**
   * @brief Append a JsonObject to the array
   * @returns the new JsonObject. Ownership is not transferred and the
   * pointer is valid for the lifetime of this JsonArray.
   */
  JsonObject* AppendObject() {
    JsonObject *obj = new JsonObject();
    m_values.push_back(obj);
    m_complex_type = true;
    return obj;
  }

  /**
   * @brief Append a JsonArray to the array
   * @returns the new JsonArray. Ownership is not transferred and the
   * pointer is valid for the lifetime of this JsonArray.
   */
  JsonArray* AppendArray() {
    JsonArray *array = new JsonArray();
    m_values.push_back(array);
    m_complex_type = true;
    return array;
  }

  /**
   * @brief Append a JsonValue to the array.
   * @param value the JsonValue to append, ownership is transferred.
   */
  void AppendValue(JsonValue *value) {
    m_values.push_back(value);
  }

  /**
   * @brief Append a raw value to the array
   */
  void AppendRaw(const std::string &value) {
    m_values.push_back(new JsonRawValue(value));
  }

  void Accept(JsonValueVisitorInterface *visitor) const;

  JsonValue* Clone() const;

  /**
   * @brief Check if there are elements within the array.
   * @returns true if the array is empty, false if there are elements.
   */
  bool IsEmpty() const { return m_values.empty(); }

  /**
   * @brief Return the number of elements in the array.
   */
  unsigned int Size() const { return m_values.size(); }

  /**
   * @brief Return the element at index i.
   * @returns The JsonValue at i, of NULL if the index is out of range.
   */
  const JsonValue *ElementAt(unsigned int i) const;

  /**
   * @brief Return true if this array contains nested arrays or objects.
   */
  bool IsComplexType() const { return m_complex_type; }

 private:
  typedef std::vector<JsonValue*> ValuesVector;
  ValuesVector m_values;

  // true if this array contains a nested object or array
  bool m_complex_type;

  DISALLOW_COPY_AND_ASSIGN(JsonArray);
};


class JsonObjectPropertyVisitor {
 public:
  virtual ~JsonObjectPropertyVisitor() {}

  virtual void VisitProperty(const std::string &property,
                             const JsonValue &value) = 0;
};

/**
 * @brief The interface for the JsonValueVisitor class.
 *
 * An implementation of a JsonValueVisitorInterface can be passed to the
 * Visit() method of a JsonValue. This provides traversal of a json tree in a
 * type safe manner.
 */
class JsonValueVisitorInterface {
 public:
  virtual ~JsonValueVisitorInterface() {}

  virtual void Visit(const JsonStringValue &value) = 0;
  virtual void Visit(const JsonBoolValue &value) = 0;
  virtual void Visit(const JsonNullValue &value) = 0;
  virtual void Visit(const JsonRawValue &value) = 0;
  virtual void Visit(const JsonObject &value) = 0;
  virtual void Visit(const JsonArray &value) = 0;
  virtual void Visit(const JsonUIntValue &value) = 0;
  virtual void Visit(const JsonUInt64Value &value) = 0;
  virtual void Visit(const JsonIntValue &value) = 0;
  virtual void Visit(const JsonInt64Value &value) = 0;
  virtual void Visit(const JsonDoubleValue &value) = 0;
};
/**@}*/

// operator<<
std::ostream& operator<<(std::ostream &os, const JsonStringValue &value);
std::ostream& operator<<(std::ostream &os, const JsonUIntValue &value);
std::ostream& operator<<(std::ostream &os, const JsonIntValue &value);
std::ostream& operator<<(std::ostream &os, const JsonUInt64Value &value);
std::ostream& operator<<(std::ostream &os, const JsonInt64Value &value);
std::ostream& operator<<(std::ostream &os, const JsonDoubleValue &value);
std::ostream& operator<<(std::ostream &os, const JsonBoolValue &value);
std::ostream& operator<<(std::ostream &os, const JsonNullValue &value);
std::ostream& operator<<(std::ostream &os, const JsonRawValue &value);

// JsonValue::NewNumberValue implementations.
template <>
inline JsonNumberValue* JsonValue::NewNumberValue<uint32_t>(
    const uint32_t &value) {
  return new JsonUIntValue(value);
}

template <>
inline JsonNumberValue* JsonValue::NewNumberValue<int32_t>(
    const int32_t &value) {
  return new JsonIntValue(value);
}

template <>
inline JsonNumberValue* JsonValue::NewNumberValue<uint64_t>(
    const uint64_t &value) {
  return new JsonUInt64Value(value);
}

template <>
inline JsonNumberValue* JsonValue::NewNumberValue<int64_t>(
    const int64_t &value) {
  return new JsonInt64Value(value);
}

template <>
inline JsonNumberValue* JsonValue::NewNumberValue<double>(
    const double &value) {
  return new JsonDoubleValue(value);
}

// JsonValue::NewValue implementations.
template <>
inline JsonValue* JsonValue::NewValue<std::string>(const std::string &value) {
  return new JsonStringValue(value);
}

template <>
inline JsonValue* JsonValue::NewValue<uint32_t>(const uint32_t &value) {
  return NewNumberValue(value);
}

template <>
inline JsonValue* JsonValue::NewValue<int32_t>(const int32_t &value) {
  return NewNumberValue(value);
}

template <>
inline JsonValue* JsonValue::NewValue<uint64_t>(const uint64_t &value) {
  return NewNumberValue(value);
}

template <>
inline JsonValue* JsonValue::NewValue<int64_t>(const int64_t &value) {
  return NewNumberValue(value);
}

template <>
inline JsonValue* JsonValue::NewValue<double>(const double &value) {
  return NewNumberValue(value);
}

template <>
inline JsonValue* JsonValue::NewValue<bool>(const bool &value) {
  return new JsonBoolValue(value);
}
/**@}*/
}  // namespace web
}  // namespace ola
#endif  // INCLUDE_OLA_WEB_JSON_H_
