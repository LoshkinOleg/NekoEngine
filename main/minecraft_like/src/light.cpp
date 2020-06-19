#include <light.h>


namespace neko
{
Light::Light(MinecraftLikeEngine& engine) {
	
}

	
void Light::Init() {
	const auto config = BasicEngine::GetInstance()->config;
	lightShader_.LoadFromFile(
		config.dataRootPath + "shaders/07_hello_light/lamp.vert",
		config.dataRootPath + "shaders/07_hello_light/lamp.frag");
	phongShader_.LoadFromFile(
		config.dataRootPath + "shaders/07_hello_light/light.vert",
		config.dataRootPath + "shaders/07_hello_light/light.frag");
	std::cout << "INIT" << std::endl;
}

void Light::Update(seconds dt) {
	lightPos_ = Vec3f(
		Cos(radian_t(time_)),
		0.0f,
		Sin(radian_t(time_))) * lightDist_;
}

void Light::Destroy() {
	lightShader_.Destroy();
	phongShader_.Destroy();
}
}
