#include <optix_device.h>

#include "launch_parameters.cuh"
#include "vector_functions.hpp"
#include "random.cuh"

namespace lift {
	extern "C" __constant__ LaunchParameters optix_launch_parameters;

	enum { SURFACE_RAY_TYPE = 0, RAY_TYPE_COUNT };

	static __forceinline__ __device__
	void* unpack_pointer(uint32_t i0, uint32_t i1) {
		const uint64_t uptr = static_cast<uint64_t>(i0) << 32 | i1;
		void* ptr = reinterpret_cast<void*>(uptr);
		return ptr;
	}

	static __forceinline__ __device__
	void pack_pointer(void* ptr, uint32_t& i0, uint32_t& i1) {
		const uint64_t uptr = reinterpret_cast<uint64_t>(ptr);
		i0 = uptr >> 32;
		i1 = uptr & 0x00000000ffffffff;
	}

	template <typename T>
	static __forceinline__ __device__ T* get_payload() {
		const uint32_t u0 = optixGetPayload_0();
		const uint32_t u1 = optixGetPayload_1();
		return reinterpret_cast<T*>(unpack_pointer(u0, u1));
	}

	extern "C" __global__ void __closesthit__radiance() {
		const int prim_id = optixGetPrimitiveIndex();
		vec3& payload = *(vec3*)get_payload<vec3>();
		payload = random_color(prim_id);
	}

	extern "C" __global__ void __anyhit__radiance() {
		/*! for this simple example, this will remain empty */
	}

	extern "C" __global__ void __miss__radiance() {
		auto& payload = *static_cast<vec3*>(get_payload<vec3>());
		payload = vec3(1.0f);
	}


	//------------------------------------------------------------------------------
	// ray gen program - the actual rendering happens in here
	//------------------------------------------------------------------------------
	extern "C" __global__ void __raygen__render_frame() {
		const int ix = optixGetLaunchIndex().x;
		const int iy = optixGetLaunchIndex().y;

		const auto& camera = optix_launch_parameters.camera;

		vec3 pixel_color_payload(0.0f);

		uint32_t u0, u1;
		pack_pointer(&pixel_color_payload, u0, u1);

		const vec2 screen(vec2(ix + 0.5f, iy + 0.5f) / vec2(optix_launch_parameters.frame.size));

		vec3 ray_dir = normalize(camera.direction +
			(screen.x - 0.5f) * camera.horizontal +
			(screen.y - 0.5f) * camera.vertical);

		optixTrace(optix_launch_parameters.traversable,
				   make_float3(camera.position.x, camera.position.y, camera.position.z),
				   make_float3(ray_dir.x, ray_dir.y, ray_dir.z),
				   0.0f,
				   1e20f,
				   0.0f,
				   OptixVisibilityMask(255),
				   OPTIX_RAY_FLAG_DISABLE_ANYHIT, //OPTIX_RAY_FLAG_NONE,
				   SURFACE_RAY_TYPE, // SBT offset
				   RAY_TYPE_COUNT, // SBT stride
				   SURFACE_RAY_TYPE, // missSBTIndex 
				   u0, u1);

		const int r = int(255.99f * pixel_color_payload.r);
		const int g = int(255.99f * pixel_color_payload.g);
		const int b = int(255.99f * pixel_color_payload.b);

		const uint32_t rgba = 0xff000000
			| (r << 0) | (g << 8) | (b << 16);
		//
		//
		const uint32_t fb_index = ix + iy * optix_launch_parameters.frame.size.x;
		optix_launch_parameters.frame.color_buffer[fb_index] = rgba;
	}
}