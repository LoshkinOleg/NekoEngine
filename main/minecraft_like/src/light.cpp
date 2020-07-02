#include <light.h>
#include <imgui.h>

namespace neko
{
Light::Light(MinecraftLikeEngine& engine) {
	
}
void Light::InitLight() const{
	lightShader_.Bind();
}

void Light::Init() {
	const auto config = BasicEngine::GetInstance()->config;
	lightShader_.LoadFromFile(
		config.dataRootPath + "shaders/minecraft_like/light.vert",
		config.dataRootPath + "shaders/minecraft_like/light.frag");
}

void Light::Update(seconds dt) {
	time_ += dt.count();
	/*lightPos_ = Vec3f(
		Cos(radian_t(time_)),
		Sin(radian_t(time_)),
		0.0f) * lightDist_;*/


	GizmosLocator::get().DrawCube(lightPos_, Vec3f(.5f), Color4(0,0,1,1));
}


void Light::Destroy() {
	lightShader_.Destroy();
}

void Light::DisplayLight(Mat4f& model, Mat4f& view, Mat4f& projection, Vec3f pos){

	lightShader_.SetMat4("model", model);
	lightShader_.SetMat4("view", view);
	lightShader_.SetMat4("projection", projection);
	lightShader_.SetVec3("light.color", lightColor_);
	lightShader_.SetVec3("light.direction", lightDirection_.Normalized());
	lightShader_.SetVec3("viewPos", pos);
	lightShader_.SetInt("objectMaterial.diffuse", 0);
	lightShader_.SetInt("objectMaterial.specular", 1);
	lightShader_.SetInt("objectMaterial.shininess", specularPow_);

	lightShader_.SetFloat("ambientStrength", ambientStrength_);
	lightShader_.SetFloat("diffuseStrength", diffuseStrength_);
	lightShader_.SetFloat("specularStrength", specularStrength_);

	const auto inverseTransposeModel = model.Inverse().Transpose();
	lightShader_.SetMat4("inverseTransposeModel", inverseTransposeModel);

}
	
}
