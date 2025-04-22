// Copyright Gradientspace Corp. All Rights Reserved.

// llvm function_ref
// source: 
// https://github.com/llvm-mirror/llvm/blob/master/include/llvm/ADT/STLExtras.h#L104
//
// original license from that file:
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "GradientspacePlatform.h"
#include <type_traits>  // std::enable_if, std::is_same
#include <utility>		// std::forward

namespace GS
{

// This is intended to be a lightweight "function reference", similar to 
// std::function but without the overhead of "owning" the function. So basically
// it can be used to pass lambdas to functions, eg you declare a function like
// 
//      void ProcessAllThings( FunctionRef<void(const Thing&)> ProcessingFunc );
// 
// and the implementation can call ProcessingFunc(ThingInstance), and the function
// can be called like ProcessAllThings( [](const Thing& ThingInst) { // do stuff with thing } );
//
// This adds a bit of overhead compared to the template<T> ProcessAllThings(T ProcessingFunc) option,
// but it means all the code doesn't have to go in a header file, and it helpfully
// documents the required arguments / return-type / etc.
//
// Ideally this will be similar in cost to Unreal Engine's TFunctionRef, which is very efficient.
// The current implementation here is from LLVM. Another option is the function_view code from
// https://github.com/Naios/function2, however there it is part of a much more complex template
// (have not tested performance though). C++26 adds a function_ref similar to this (https://en.cppreference.com/w/cpp/utility/functional/function_ref)
// 
// Note that 
// (1) the lambda passed to calls must stay alive for the duration of the call. So (eg) if
// 	   using this in a multi-threaded context, something has to keep the lambda alive, FunctionRef won't do it.
// 	   (eg caller must store it in a std::function, etc)
// (2) basically due to the above you should never have a class member or variable of type FunctionRef.
// 	   It's only meant for use in this typed-lambda-argument context.
//

template<typename Fn> class FunctionRef;

template<typename Ret, typename ...Params>
class FunctionRef<Ret(Params...)> {
    Ret(*callback)(intptr_t callable, Params ...params) = nullptr;
    intptr_t callable;

    template<typename Callable>
    static Ret callback_fn(intptr_t callable, Params ...params) {
        return (*reinterpret_cast<Callable*>(callable))(
            std::forward<Params>(params)...);
    }

public:
	FunctionRef() = default;
	FunctionRef(std::nullptr_t) {}

    template <typename Callable>
	FunctionRef(Callable&& callable,
        typename std::enable_if<
        !std::is_same<typename std::remove_reference<Callable>::type,
		FunctionRef>::value>::type* = nullptr)
        : callback(callback_fn<typename std::remove_reference<Callable>::type>),
        callable(reinterpret_cast<intptr_t>(&callable)) {}

    Ret operator()(Params ...params) const {
        return callback(callable, std::forward<Params>(params)...);
    }

    operator bool() const { return callback; }
};


}
