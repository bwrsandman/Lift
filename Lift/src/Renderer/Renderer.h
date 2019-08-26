#pragma once
#include "RendererAPI.h"
#include "CudaBuffer.h"
#include <cuda/launch_parameters.cuh>


namespace lift {
	class Scene;

	class Renderer {
	public:
		Renderer();

		void LaunchSubframe(const Scene& scene, LaunchParameters& params);
		void DownloadFrame(uint32_t pixels[], CudaBuffer<uint32_t> buffer);

		static void Submit(const std::shared_ptr<VertexArray>& vertex_array);
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		
		CudaBuffer<LaunchParameters> d_params_;
		LaunchParameters params_;
	};
}
