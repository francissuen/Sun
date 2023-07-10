/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_VARIANT_H
#define FS_SUN_VARIANT_H

#if __cplusplus >= 201703L
#include <variant>
#else

#include <cstring>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>

#include "ns.h"
#include "string.h"
#include "utility.h"

FS_SUN_NS_BEGIN
template <typename... Ts>
class Variant;

namespace variant {

#ifdef _MSC_VER
#ifdef max
#undef max
#endif
#endif
using Index = std::uint8_t;
static constexpr Index npos = std::numeric_limits<Index>::max();

template <typename T>
struct CopyCtor {
  template <typename... Ts>
  void operator()(Variant<Ts...> *dst, const Variant<Ts...> &src);
};

template <typename T>
struct MoveCtor {
  template <typename... Ts>
  void operator()(Variant<Ts...> *dst, Variant<Ts...> &&src);
};

template <typename T>
struct Dtor {
  template <typename... Ts>
  void operator()(Variant<Ts...> *dst);
};

template <typename T>
struct Swap {
  template <typename... Ts>
  void operator()(Variant<Ts...> &a, Variant<Ts...> &b);
};

template <typename T>
struct ToString {
  template <typename... Ts>
  void operator()(const Variant<Ts...> *src, std::string &ret);
};
}  // namespace variant
/**
 *  \brief A safe union, and needs no RTTI
 */
template <typename... Ts>
class Variant {
  static_assert(sizeof...(Ts) > 1u, "Sizeof...(Ts) should be at least 2");
  static_assert(sizeof...(Ts) < variant::npos,
                "Too many Ts for variant::Index.");

  template <typename T>
  friend struct variant::CopyCtor;

  template <typename T>
  friend struct variant::MoveCtor;

  template <typename T>
  friend struct variant::Dtor;

  template <typename T>
  friend struct variant::Swap;

  template <typename T>
  friend struct variant::ToString;

 private:
  template <typename T>
  struct IsValidType {
    static constexpr bool value =
        ((!std::is_const<T>::value) && (!std::is_reference<T>::value));
  };

  static_assert(StaticAnd<IsValidType, Ts...>::value,
                "not all types in Ts are valid type");

  typedef typename std::aligned_storage<
      StaticMax<std::size_t, sizeof(Ts)...>::value,
      StaticMax<std::size_t, alignof(Ts)...>::value>::type RawData;

 public:
  Variant() : raw_data_{}, index_{variant::npos} {}

  Variant(const Variant &other) {
    Invoke<variant::CopyCtor>::template ForTypeIn<Ts...>::With2(other.index_,
                                                                this, other);
  }

  Variant(Variant &&other) noexcept {
    Invoke<variant::MoveCtor>::template ForTypeIn<Ts...>::With2(
        other.index_, this, std::move(other));
  }

 private:
  class RightCtorTag {};
  class WrongCtorTag {};
  template <typename T>
  class DetermineTag {
   public:
    using Tag = std::conditional<
        IsType<typename remove_cvref<T>::type>::template In<Ts...>::value,
        RightCtorTag, WrongCtorTag>::type;
  };

  template <typename T>
  Variant(T &&other, RightCtorTag)
      : index_(IndexOf<typename remove_cvref<T>::type>::template In<
               Ts...>::value) {
    new (&(raw_data_)) typename remove_cvref<T>::type(std::forward<T>(other));
  }

 public:
  template <typename T>
  Variant(T &&other)
      : Variant(std::forward<T>(other), typename DetermineTag<T>::Tag{}) {}

  ~Variant() {
    Invoke<variant::Dtor>::template ForTypeIn<Ts...>::With2(index_, this);
  }

 public:
  friend void swap(Variant &a, Variant &b) {
    if (a.index_ != variant::npos)
      Invoke<variant::Swap>::template ForTypeIn<Ts...>::With2(a.index_, a, b);
    else {
      Invoke<variant::MoveCtor>::template ForTypeIn<Ts...>::With2(
          b.index_, &a, std::move(b));
    }
  }

  template <typename T, typename... TArgs>
  T &Emplace(TArgs &&...args) {
    return operator=(T(std::forward<TArgs>(args)...));
  }

  Variant &operator=(Variant other) {
    swap(*this, other);

    return *this;
  }

  template <typename T>
  Variant &operator=(T &&other) {
    static_assert(
        IsType<typename remove_cvref<T>::type>::template In<Ts...>::value,
        "T is not one of Ts");

    Variant tmp(std::forward<T>(other));
    swap(*this, tmp);

    return *this;
  }

  variant::Index Index() const { return index_; }

  template <typename T>
  bool Is() const {
    static_assert(IsType<T>::template In<Ts...>::value, "T is not one of Ts");
    static constexpr variant::Index idx =
        IndexOf<T>::template In<Ts...>::value;
    return index_ == idx;
  }

  /**
   *  \warining No guarantee for a correct value.
   */
  template <typename T>
  T &RawGet() &noexcept {
    static_assert(IsType<T>::template In<Ts...>::value, "T is not one of Ts");
    return reinterpret_cast<T &>(raw_data_);
  }

  /**
   *  \warining No guarantee for a correct value.
   */
  template <typename T>
  T &&RawGet() &&noexcept {
    static_assert(IsType<T>::template In<Ts...>::value, "T is not one of Ts");
    return std::move(reinterpret_cast<T &>(raw_data_));
  }

  /**
   *  \warining No guarantee for a correct value.
   */
  template <typename T>
  const T &RawGet() const &noexcept {
    static_assert(IsType<T>::template In<Ts...>::value, "T is not one of Ts");
    return reinterpret_cast<const T &>(raw_data_);
  }

  template <typename T>
  T &Get() & {
    if (Is<T>())
      return RawGet<T>();
    else
      throw std::bad_cast();
  }

  template <typename T>
  T &&Get() && {
    if (Is<T>())
      return (std::move(*this)).template RawGet<T>();
    else
      throw std::bad_cast();
  }

  template <typename T>
  const T &Get() const & {
    if (Is<T>())
      return RawGet<T>();
    else
      throw std::bad_cast();
  }

  friend std::string to_string(const Variant &value) {
    std::string ret;
    Invoke<variant::ToString>::template ForTypeIn<Ts...>::With2(value.index_,
                                                                &value, ret);
    return ret;
  }

 private:
  RawData raw_data_{};
  variant::Index index_{variant::npos};
};

namespace variant {
template <typename T>
template <typename... Ts>
void CopyCtor<T>::operator()(Variant<Ts...> *dst, const Variant<Ts...> &src) {
  new (&(dst->raw_data_)) T(src.template Get<T>());
  dst->index_ = src.index_;
}

template <typename T>
template <typename... Ts>
void MoveCtor<T>::operator()(Variant<Ts...> *dst, Variant<Ts...> &&src) {
  new (&(dst->raw_data_)) T(std::move(src).template Get<T>());
  dst->index_ = src.index_;
}

template <typename T>
template <typename... Ts>
void Dtor<T>::operator()(Variant<Ts...> *dst) {
  reinterpret_cast<T *>(&(dst->raw_data_))->~T();
  dst->index_ = npos;
}

template <typename T>
template <typename... Ts>
void Swap<T>::operator()(Variant<Ts...> &a, Variant<Ts...> &b) {
  Variant<Ts...> tmp(std::move(a));

  /** a = b */
  Invoke<MoveCtor>::template ForTypeIn<Ts...>::With2(b.index_, &a,
                                                     std::move(b));

  /** b = tmp */
  Invoke<MoveCtor>::template ForTypeIn<Ts...>::With2(tmp.index_, &b,
                                                     std::move(tmp));
}

template <typename T>
template <typename... Ts>
void ToString<T>::operator()(const Variant<Ts...> *src, std::string &ret) {
  ret = string::ToString(src->template Get<T>());
}

}  // namespace variant

FS_SUN_NS_END

#endif

#endif  // FS_SUN_VARIANT_H
