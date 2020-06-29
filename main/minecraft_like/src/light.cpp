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
}

void Light::Update(seconds dt) {
	/*lightPos_ = Vec3f(
		Cos(radian_t(time_)),
		0.0f,
		Sin(radian_t(time_))) * lightDist_;*/
	lightPos_ = Vec3f(0, 5, 0) * lightDist_;
}

void Light::Destroy() {
	lightShader_.Destroy();
	phongShader_.Destroy();
}

void Light::BindShader(Mat4f& model, Mat4f& view, Mat4f& projection, Vec3f pos, gl::RenderCuboid& cube) {
	lightShader_.Bind();
	lightShader_.SetMat4("model", model);
	lightShader_.SetMat4("view", view);
	lightShader_.SetMat4("projection", projection);

	lightShader_.SetVec3("lightColor", lightColor_);
	cube.Draw();
	
	phongShader_.SetMat4("model", model);
	phongShader_.SetMat4("view", view);
	phongShader_.SetMat4("projection", projection);
	phongShader_.SetVec3("lightPos", lightPos_);
	phongShader_.SetVec3("viewPos", pos);
	phongShader_.SetVec3("objectColor", objectColor_);
	phongShader_.SetVec3("lightColor", lightColor_);
	phongShader_.SetFloat("ambientStrength", ambientStrength_);
	phongShader_.SetFloat("diffuseStrength", diffuseStrength_);
	phongShader_.SetFloat("specularStrength", specularStrength_);
	phongShader_.SetInt("specularPow", specularPow_);

	cube.Draw();
}

}
