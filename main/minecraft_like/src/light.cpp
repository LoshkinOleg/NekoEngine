#include <light.h>


namespace neko
{
Light::Light(MinecraftLikeEngine& engine) {
	
}
void Light::InitLight() {
	lightShader_.Bind();
	phongShader_.Bind();
}
	
void Light::Init() {
	const auto config = BasicEngine::GetInstance()->config;
	lightShader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/lamp.vert",
		config.dataRootPath + "shaders/minecraft_like/lamp.frag");
	phongShader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/light.vert",
		config.dataRootPath + "shaders/minecraft_like/light.frag");
}

void Light::Update(seconds dt) {
	time_ += dt.count();
	lightPos_ = Vec3f(
		Cos(radian_t(time_)),
		Sin(radian_t(time_)),
		0.0f) * lightDist_;
	//lightPos_ = Vec3f(0, 5, 0) * lightDist_;
}

void Light::Destroy() {
	lightShader_.Destroy();
	phongShader_.Destroy();
}

void Light::BindShader(Mat4f& model, Mat4f& view, Mat4f& projection, Vec3f pos) {

	lightShader_.SetMat4("model", model);
	lightShader_.SetMat4("view", view);
	lightShader_.SetMat4("projection", projection);

	lightShader_.SetVec3("lightColor", lightColor_);


	phongShader_.SetMat4("model", model);
	phongShader_.SetMat4("view", view);
	phongShader_.SetMat4("projection", projection);
	phongShader_.SetMat4("inverseTransposeModel", model.Inverse().Transpose());
	phongShader_.SetVec3("lightPos", lightPos_);
	phongShader_.SetVec3("viewPos", pos);
	phongShader_.SetVec3("objectColor", objectColor_);
	phongShader_.SetVec3("lightColor", lightColor_);
	phongShader_.SetFloat("ambientStrength", ambientStrength_);
	phongShader_.SetFloat("diffuseStrength", diffuseStrength_);
	phongShader_.SetFloat("specularStrength", specularStrength_);
	phongShader_.SetInt("objectMaterial.diffuse", 0);
	phongShader_.SetInt("objectMaterial.specular", 1);
	phongShader_.SetInt("objectMaterial.shininess", specularPow_);
}

}
