#if defined(SOKOL_IMPL) && !defined(SOKOL_NANOVG_IMPL)
#define SOKOL_NANOVG_IMPL
#endif
#ifndef SOKOL_NANOVG_INCLUDED
/*
    sokol_nanovg.h -- NanoVG implementation using Sokol

*/

#define SOKOL_NANOVG_INCLUDED (1)
// std headers approx here?
#include <stddef.h> // size_t
#include <stdint.h>
#include <stdbool.h>

#if !defined(SOKOL_GFX_INCLUDED)
#error "Please include sokol_gfx.h before sokol_nanovg.h"
#endif

#if defined(SOKOL_API_DECL) && !defined(SOKOL_NANOVG_API_DECL)
#define SOKOL_NANOVG_API_DECL SOKOL_API_DECL
#endif
#ifndef SOKOL_NANOVG_API_DECL
#if defined(_WIN32) && defined(SOKOL_DLL) && defined(SOKOL_NANOVG_IMPL)
#define SOKOL_NANOVG_API_DECL __declspec(dllexport)
#elif defined(_WIN32) && defined(SOKOL_DLL)
#define SOKOL_NANOVG_API_DECL __declspec(dllimport)
#else
#define SOKOL_NANOVG_API_DECL extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct snvg_context { uint32_t id; } snvg_context;
/*
So there's potentially two layers here:

1. Sokolization of the NanoVG API (while having the right interface that nanovg is expecting) (or does that API *need* to be sokolized? )
2. Implementing the drawing APIs using sokol_gfx

See https://github.com/floooh/sokol/blob/sokol-spine/util/sokol_spine.h for some ideas about sokolization/hiding the real API
and https://github.com/floooh/sokol/blob/sokol-spine/tests/functional/sokol_spine_test.c for usage of that sokolized API
*/

/*
    snvg_allocator_t
    Used in snvg_desc_t to provide custom memory-alloc and -free functions
    to sokol_nanovg.h. If memory management should be overridden, both the
    alloc and free function must be provided (e.g. it's not valid to
    override one function but not the other).
*/
typedef struct snvg_allocator_t {
    void* (*alloc)(size_t size, void* user_data);
    void (*free)(void* ptr, void* user_data);
    void* user_data;
} snvg_allocator_t;

struct NVGcontext;

typedef struct snvg_desc_t {

} snvg_desc_t;

typedef struct snvg_frame_desc_t {
    float width;
    float height;
    float dpi_scale;
} snvg_frame_desc_t;

/*
If a single context, hide the NVGcontext away in _snanovg or not? Simpler interface (no need to pass pointer to each nanovg call),
but then need to wrap all API calls to retrieve 
*/
SOKOL_NANOVG_API_DECL void snvg_setup(const snvg_desc_t* desc);
SOKOL_NANOVG_API_DECL void snvg_begin_frame(const snvg_frame_desc_t* desc);
SOKOL_NANOVG_API_DECL void snvg_cancel_frame(void);
SOKOL_NANOVG_API_DECL void snvg_end_frame(void);
SOKOL_NANOVG_API_DECL snvg_context snvg_setup_context(int flags); // a la nvgCreate**; like sokol-gfx contexts
SOKOL_NANOVG_API_DECL void snvg_activate_context(snvg_context ctx);
SOKOL_NANOVG_API_DECL NVGcontext* snvg_get_current_context(void); // Make the person retrieve this & use the current API, or keep hidden internally & wrap existing?
SOKOL_NANOVG_API_DECL void snvg_discard_context(snvg_context ctx); // a la nvgDelete**
SOKOL_NANOVG_API_DECL void snvg_shutdown(void);

/* thinking through wrapped interface (could honestly be auto-generated)
// composite operation
SOKOL_NANOVG_API_DECL void snvg_global_composite_operation(int op); //nvgGlobalCompositeOperation
SOKOL_NANOVG_API_DECL void snvg_global_composite_blend_func(int sfactor, int dfactor);//nvgGlobalCompositeBlendFunc
SOKOL_NANOVG_API_DECL void snvg_global_composite_blend_func_separate(int srcRGB, int dstRGB, int srcAlpha, int dstAlpha); // nvgGlobalCompositeBlendFuncSeparate
// (skipping color utils, no need to wrap?)
// state handling
SOKOL_NANOVG_API_DECL void snvg_save(void); //nvgSave
SOKOL_NANOVG_API_DECL void snvg_restore(void); //nvgRestore
SOKOL_NANOVG_API_DECL void snvg_reset(void); //nvgReset
// render styles
*/

#ifdef __cplusplus
} /* extern "C" */

/* reference-based equivalents for C++ */
inline void snvg_setup(const snvg_desc_t& desc) { return snvg_setup(&desc); }
inline void snvg_begin_frame(const snvg_frame_desc_t& desc) { return snvg_begin_frame(&desc); }

#endif
#endif // SOKOL_NANOVG_INCLUDED

/* IMPLEMENTATION ---------------------------------------- */
#ifdef SOKOL_NANOVG_IMPL
#define SOKOL_NANOVG_IMPL_INCLUDED (1)

#if !defined(NANOVG_H)
#error "Please include nanovg.h before the sokol_nanovg.h implementation"
#endif

#ifndef SOKOL_API_IMPL
#define SOKOL_API_IMPL
#endif
#ifndef SOKOL_DEBUG
    #ifndef NDEBUG
        #define SOKOL_DEBUG (1)
    #endif
#endif
#ifndef SOKOL_ASSERT
    #include <assert.h>
    #define SOKOL_ASSERT(c) assert(c)
#endif
#ifndef _SOKOL_PRIVATE
    #if defined(__GNUC__) || defined(__clang__)
        #define _SOKOL_PRIVATE __attribute__((unused)) static
    #else
        #define _SOKOL_PRIVATE static
    #endif
#endif

/* helper macros */
#define _snvg_def(val, def) (((val) == 0) ? (def) : (val))

// shaders about here

// TODO: need a SKNVGcontext, approx like GLNVGcontext
// see e.g. https://github.com/memononen/nanovg/blob/master/src/nanovg_gl.h#L231
// https://github.com/bkaradzic/bgfx/blob/master/examples/common/nanovg/nanovg_bgfx.cpp#L132


typedef struct {
    snvg_desc_t desc;

} _snvg_state_t;
static _snvg_state_t _snanovg;

SOKOL_API_IMPL void snvg_setup(const snvg_desc_t* desc) {
    SOKOL_ASSERT(desc);
    memset(&_snanovg, 0, sizeof(_snanovg));
    _snanovg.desc = *desc;
}


#endif // SOKOL_NANOVG_IMPL
