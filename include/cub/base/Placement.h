//
// Created by Darwin Yuan on 2021/4/21.
//

#ifndef TRANS_DSL_2_PLACEMENT_H
#define TRANS_DSL_2_PLACEMENT_H

#include <cub/cub_ns.h>
#include <utility>
#include <new>

CUB_NS_BEGIN

template <typename T>
struct Placement {
    auto GetObject() -> T* {
        return reinterpret_cast<T*>(buff);
    }

    auto GetObject() const -> T const* {
        return reinterpret_cast<T const*>(buff);
    }

    auto GetRef() -> T& {
        return *GetObject();
    }

    auto GetRef() const -> T const& {
        return *GetObject();
    }

    auto operator->() -> T* {
        return GetObject();
    }

    auto operator->() const -> T const* {
        return GetObject();
    }

    auto operator*() -> T& {
        return GetRef();
    }

    auto operator*() const -> T const& {
        return GetRef();
    }

    auto Destroy() {
        GetObject()->~T();
    }

    template<typename ... ARGS>
    auto Emplace(ARGS&& ... args) -> T* {
        return new (buff) T{std::forward<ARGS>(args)...};
    }

    template<typename ... ARGS>
    auto Replace(ARGS&& ... args) -> void {
        Destroy();
        Emplace(std::forward<ARGS>(args)...);
    }

private:
    alignas(T) char buff[sizeof(T)];
};

CUB_NS_END

#endif //TRANS_DSL_2_PLACEMENT_H
