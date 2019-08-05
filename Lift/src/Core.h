#pragma once

#ifdef LF_DEBUG
#define LF_ENABLE_ASSERTS
#endif

#ifdef LF_ENABLE_ASSERTS
#define LF_ASSERT(x, ...) { if(!(x)) { LF_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
#define LF_CORE_ASSERT(x, ...) { if(!(x)) { LF_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else
	#define LF_ASSERT(x, ...)
	#define LF_CORE_ASSERT(x, ...)
#endif

//#define BIT(x) (1 << (x))
template <typename T>
constexpr T Bit(T x) {
	return 1 << x;
}

#define LF_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)


#ifdef LF_DEBUG
#define OPENGL_CALL(x) \
		(x); \
		while (GLenum error = glGetError()) { \
			LF_CORE_ERROR("[OpenGL Error] {0}", error);	\
			LF_CORE_ERROR("\tFile: {0}", __FILE__);	\
			LF_CORE_ERROR("\tLine: {0}", __LINE__);	\
		}
#else
	#define OPENGL_CALL(x) (x); 
#endif

#ifdef LF_DEBUG
#define OPTIX_CHECK( call ) {                                                                   \
    OptixResult res = call;                                                                     \
    if( res != OPTIX_SUCCESS ) {                                                                \
        LF_CORE_ERROR("Optix call {0} failed with code {1} (line {2})", #call, res, __LINE__ ); \
        exit( 2 );                                                                              \
    }																							\
}
#else
	#define OPTIX_CHECK( call ) ( call );
#endif
