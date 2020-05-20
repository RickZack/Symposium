/*
 * Copyright (c) 2019 Riccardo Zaccone, Ksenia Del Conte Akimova, Cristian Gianetto, Martina Bellissimo
 *
 * This file is part of Symposium.
 * Symposium is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symposium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Symposium.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   counter.h
 * Project: Symposium
  * Authors: 
 *          Riccardo Zaccone <riccardo.zaccone at studenti.polito.it>
 *          Ksenia Del Conte Akimova <s256669 at studenti.polito.it>
 *          Cristian Gianetto <s265387 at studenti.polito.it>
 *          Martina Bellissimo <s257307 at studenti.polito.it>
 *
 * Created on 01 gennaio 2020, 13:38
 */
#ifndef SYMPOSIUM_COUNTER_H
#define SYMPOSIUM_COUNTER_H

#include <cstdint>
#include <type_traits>
#include <limits>
#include <boost/serialization/access.hpp>

template <typename Source, typename Dest, std::enable_if_t<std::is_integral<Source>::value && std::is_integral<Dest>::value, int> =0>
struct type_not_narrow{
    static constexpr bool value= std::numeric_limits<Source>::max() <= std::numeric_limits<Dest>::max();
};

template<typename T, T N, T E=std::numeric_limits<T>::max(), std::enable_if_t<std::is_integral<T>::value, int> =0>
class basic_counter{
    T cnt;
    basic_counter(T initializer): cnt(initializer){}

    struct iterator{
        basic_counter cnt;
        iterator(basic_counter& c): cnt(c){}
        const basic_counter& operator*(){
            return cnt;
        }

        bool operator==(const iterator &rhs) const {
            return cnt == rhs.cnt;
        }

        bool operator!=(const iterator &rhs) const {
            return !(rhs == *this);
        }
    };

protected:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int){
        ar & cnt;
    }

public:
    static constexpr T start_val=N;
    static constexpr T end_val=E;
    typedef T type;
    basic_counter(): cnt(N){}



    struct forward_iterator: public iterator{
        forward_iterator(basic_counter& c): iterator(c){}
        forward_iterator begin(){
            return *this;
        }
        forward_iterator end(){
            basic_counter end_cnt(E);
            return forward_iterator(end_cnt);
        }
        forward_iterator& operator++(){
            iterator::cnt++;
            return *this;
        }
        forward_iterator operator++(int){
            auto old=*this;
            cnt++;
            return old;
        }
    };

    struct reverse_iterator: public iterator{
        reverse_iterator(basic_counter& c):iterator(c){}
        reverse_iterator begin(){
            return *this;
        }
        reverse_iterator end(){
            basic_counter start_cnt(E);
            return reverse_iterator(start_cnt);
        }
        reverse_iterator& operator++(){
            iterator::cnt--;
            return *this;
        }
        reverse_iterator operator++(int){
            auto old=*this;
            cnt--;
            return old;
        }
    };

    //pre-increment operator (++a)
    basic_counter& operator++(){
        cnt+=1;
        return *this;
    }
    //post-increment operator (a++)
    basic_counter operator++(int){
        basic_counter old=*this;
        operator++();
        return old;
    }

    template <typename T1, T1 N1, typename T2, T2 N2>
    friend basic_counter<typename std::common_type<T1,T2>::type, N1> operator+(const basic_counter<T1, N1>& lhs,
                                                                               const basic_counter<T2, N2>& rhs);

    template <typename X, typename U, U M, std::enable_if_t<type_not_narrow<X, U>::value, int>>
    friend basic_counter<U,M> operator+(const basic_counter<U, M>& lhs, const X step);

    template <typename T2, T2 N2>
    basic_counter<typename std::common_type<T,T2>::type, N>& operator+=(const basic_counter<T2, N2>& rhs){
        this->cnt+=rhs.cnt;
        return *this;
    }

    template <typename X, std::enable_if_t<type_not_narrow<X, T>::value, int> =0>
    basic_counter<typename std::common_type<T,X>::type, N>& operator+=(const X rhs){
        this->cnt+=rhs;
        return *this;
    }



    template <typename T1, T1 N1, typename T2, T2 N2>
    friend basic_counter<typename std::common_type<T1,T2>::type, N1> operator-(const basic_counter<T1, N1>& lhs,
                                                                               const basic_counter<T2, N2>& rhs);

    template <typename X, typename U, U M, std::enable_if_t<type_not_narrow<X, U>::value, int>>
    friend basic_counter<U,M> operator-(const basic_counter<U, M>& lhs, const X step);

    template <typename T2, T2 N2>
    basic_counter<typename std::common_type<T,T2>::type, N>& operator-=(const basic_counter<T2, N2>& rhs){
        this->cnt-=rhs.cnt;
        return *this;
    }

    template <typename X, std::enable_if_t<type_not_narrow<X, T>::value, int> =0>
    basic_counter<typename std::common_type<T,X>::type, N>& operator-=(const X rhs){
        this->cnt-=rhs;
        return *this;
    }

    //pre-decrement operator (--a)
    basic_counter& operator--(){
        cnt-=1;
        return *this;
    }
    //post-decrement operator (a--)
    basic_counter operator--(int){
        basic_counter old=*this;
        operator--();
        return old;
    }
    void reset(){
        cnt=N;
    }

    operator T() const{
        return T{cnt};
    }

    forward_iterator begin(){
        static_assert(N<E, "Not possible to use this counter as in range expression, see make_range<>() function");
        return forward_iterator(*this);
    }

    forward_iterator end(){
        basic_counter end_cnt(E);
        return forward_iterator(end_cnt);
    }

    reverse_iterator rbegin(){
        static_assert(N>E, "Not possible to use this counter as in range expression, see make_range<>() function");
        return reverse_iterator(*this);
    }

    reverse_iterator rend(){
        basic_counter end_cnt(E);
        return reverse_iterator(end_cnt);
    }
};
template <typename T1, T1 N1, typename T2, T2 N2>
basic_counter<typename std::common_type<T1,T2>::type, N1> operator+(const basic_counter<T1, N1>& lhs,
                                                                    const basic_counter<T2, N2>& rhs){
    return {lhs.cnt+rhs.cnt};
}
template<typename X, typename T, T N, std::enable_if_t<type_not_narrow<X, T>::value, int> =0>
basic_counter<T,N> operator+(const basic_counter<T, N>& lhs, const X step){
    basic_counter<T,N> c=lhs;
    c.cnt+=step;
    return c;
}

template <typename T1, T1 N1, typename T2, T2 N2>
basic_counter<typename std::common_type<T1,T2>::type, N1> operator-(const basic_counter<T1, N1>& lhs,
                                                                    const basic_counter<T2, N2>& rhs){
    return {lhs.cnt-rhs.cnt};
}

template<typename X, typename T, T N, std::enable_if_t<type_not_narrow<X, T>::value, int> =0>
basic_counter<T,N> operator-(const basic_counter<T, N>& lhs, const X step){
    basic_counter<T,N> c=lhs;
    c.cnt-=step;
    return c;
}


template <unsigned V>
constexpr long int overflow_safe_xint8() {
    return V;
}

template <unsigned V, char C, char... Cs>
constexpr long int overflow_safe_xint8() {
    return overflow_safe_xint8<10*V + C - '0', Cs...>();
}

template <char... Cs> uint8_t operator "" _ui8(){
    constexpr unsigned int n=(overflow_safe_xint8<0,Cs...>());
    constexpr unsigned int limit=std::numeric_limits<uint8_t>::max();
    static_assert(n<=limit, "This literal can't be represented with a uint8_t");
    return static_cast<uint8_t>(n);
}

template <char... Cs> int8_t operator "" _i8(){
    constexpr long int n=(overflow_safe_xint8<0,Cs...>());
    constexpr unsigned int limit=std::numeric_limits<int8_t>::max();
    static_assert(n<=limit, "This literal can't be represented with a int8_t");
    return static_cast<int8_t>(n);
}

template<typename T, T start_val, T end_val, std::enable_if_t<(start_val<end_val),int> =0>
constexpr typename basic_counter<T, start_val, end_val>::forward_iterator make_range(){
    basic_counter<T, start_val, end_val> range_cnt;
    return typename basic_counter<T, start_val, end_val>::forward_iterator(range_cnt);
}

template<typename T, T start_val, T end_val, std::enable_if_t<(start_val>end_val),int> =0>
constexpr typename basic_counter<T, start_val, end_val>::reverse_iterator make_range(){
    basic_counter<T, start_val, end_val> range_cnt;
    return typename basic_counter<T, start_val, end_val>::reverse_iterator(range_cnt);
}

template<typename T, T start_val, T end_val>
constexpr auto make_reverse_range() -> decltype(make_range<T, end_val-1, start_val-1>()){
    return make_range<T, end_val-1, start_val-1>();
}



template<int8_t N>
struct byte_counter : public basic_counter<int8_t , N>{};
template<int N>
struct int_counter : public basic_counter<int, N>{};
template<long N>
struct long_counter : public basic_counter<long int, N>{};
template<unsigned N>
struct uint_counter : public basic_counter<unsigned , N>{};

template <typename T, std::enable_if_t<std::is_unsigned<T>::value, int> =0>
struct positive_cnt : private basic_counter<T, 1>{
    positive_cnt(): basic_counter<T, 1>(){}
    using typename basic_counter<T, 1>::type;
    using basic_counter<T, 1>::start_val;
    using basic_counter<T, 1>::operator++;
    using basic_counter<T, 1>::operator T;
    using basic_counter<T, 1>::serialize;
};

using byte_positive_cnt=positive_cnt<uint8_t>;
using uint_positive_cnt=positive_cnt<unsigned>;
using ulong_positive_cnt=positive_cnt<unsigned long>;

#endif //SYMPOSIUM_COUNTER_H
