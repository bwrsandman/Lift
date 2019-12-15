#pragma once

#include "SceneList.h"
#include "UserSettings.h"
#include "vulkan/rayTracing/Application.hpp"

class RayTracer final : public Vulkan::RayTracing::Application
{
public:

	VULKAN_NON_COPIABLE(RayTracer)

	RayTracer(const UserSettings& userSettings, const Vulkan::WindowConfig& windowConfig, bool vsync);
	~RayTracer();

protected:

	[[nodiscard]] const Assets::Scene& GetScene() const override { return *scene_; }
	[[nodiscard]] Assets::UniformBufferObject GetUniformBufferObject(VkExtent2D extent) const override;

	void OnDeviceSet() override;
	void CreateSwapChain() override;
	void DeleteSwapChain() override;
	void DrawFrame() override;
	void Render(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

	void OnKey(int key, int scancode, int action, int mods) override;
	void OnCursorPosition(double xpos, double ypos) override;
	void OnMouseButton(int button, int action, int mods) override;

private:

	void LoadScene(uint32_t sceneIndex);
	void CheckAndUpdateBenchmarkState(double prevTime);
	void CheckFramebufferSize() const;

	uint32_t sceneIndex_{};
	UserSettings userSettings_{};
	UserSettings previousSettings_{};
	SceneList::CameraInitialSate cameraInitialSate_{};

	std::unique_ptr<const Assets::Scene> scene_;
	std::unique_ptr<class UserInterface> userInterface_;

	double mouseX_{};
	double mouseY_{};
	bool mouseLeftPressed_{};

	float cameraX_{};
	float cameraY_{};
	double time_{};

	uint32_t totalNumberOfSamples_{};
	uint32_t numberOfSamples_{};
	bool resetAccumulation_{};

	// Benchmark stats
	double sceneInitialTime_{};
	double periodInitialTime_{};
	uint32_t periodTotalFrames_{};
};