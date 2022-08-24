//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

#define WINDOWS

# include <memory>
#include <cstddef>

namespace Platform {

# ifdef WINDOWS

	template <size_t Alignment>
	[[nodiscard]]
	inline void *AlignedMalloc(size_t size) noexcept{
		return ::_aligned_malloc(size, Alignment);
	}
	inline void AlignedFree(void *const p) noexcept{
		::_aligned_free(p);
	}

# else

	inline constexpr size_t PointerSize = sizeof(void*);
	inline constexpr size_t MinAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

	template <size_t Alignment>
	[[nodiscard]]
	inline void *AlignedMalloc(size_t size) noexcept{
		if(Alignment > Platform::MinAlignment){
			void* p = nullptr;
			if(::posix_memalign(&p, Alignment, size) != 0){
				return nullptr;
			}
			return p;
		}
		else{
			return std::malloc(size);
		}
	}
	inline void AlignedFree(void* const p) noexcept{
		std::free(p);
	}

# endif

} // namespace Platform

template <class Type, size_t Alignment>
inline Type *AlignedMalloc(const size_t n){
	return static_cast<Type*>(Platform::AlignedMalloc<Alignment>(sizeof(Type) * n));
}
inline void AlignedFree(void *const p){
	Platform::AlignedFree(p);
}
