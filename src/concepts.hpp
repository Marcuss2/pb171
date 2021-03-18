#pragma once

#include "../lib/type_traits.h"

template <class T, class U>
inline constexpr bool is_same_v = std::is_same<T, U>::value;

template <class T, class U>
concept same_as_impl = is_same_v<T, U>;  // exposition only

template <class T, class U>
concept same_as = same_as_impl<T, U>&& same_as_impl<U, T>;

template <typename T>
concept DigitalWriteable = requires(const T& t) {
    {t.digitalWrite(bool{})};
};

template <typename T>
concept DigitalReadable = requires(const T& t) {
    { t.digitalRead() }
    ->same_as<bool>;
};

template <typename T>
concept IODigitalPin = DigitalWriteable<T>&& DigitalReadable<T>&& requires(T& t) {
    {t.setInputMode()};
    {t.setOutputMode()};
};
