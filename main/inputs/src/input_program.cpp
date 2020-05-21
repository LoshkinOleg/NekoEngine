//
// Created by efarhan on 27.01.20.
//

#include <engine/engine.h>
#include <input/input_program.h>
#include <imgui.h>

namespace neko
{

    void InputProgram::Init()
    {
        const auto& config = BasicEngine::GetInstance()->config;
    }

    void InputProgram::Update(seconds dt)
    {
    }
    void InputProgram::Destroy()
    {
    }

    void InputProgram::Render()
    {

    }

    void InputProgram::DrawImGui()
    {

        ImGui::Begin("Input");

        ImGui::Text("-----------");
        ImGui::Text("KeyBoard");
        for (unsigned int keyIndex = 0;
            keyIndex != static_cast<unsigned>(sdl::KeyCode::KEYBOARD_SIZE);
            keyIndex++) {
            if (sdl::InputLocator::get().IsKeyDown(static_cast<sdl::KeyCode>(keyIndex)))
            {
                std::string text = sdl::InputLocator::get().PcInputsEnumToString(static_cast<sdl::KeyCode>(keyIndex)) + " IsKeyDown";
                ImGui::Text(text.c_str());
            }
            if (sdl::InputLocator::get().IsKeyHeld(static_cast<sdl::KeyCode>(keyIndex)))
            {
                std::string text = sdl::InputLocator::get().PcInputsEnumToString(static_cast<sdl::KeyCode>(keyIndex)) + " IsKeyHeld";
                ImGui::Text(text.c_str());
            }
            if (sdl::InputLocator::get().IsKeyUp(static_cast<sdl::KeyCode>(keyIndex)))
            {
                std::string text = sdl::InputLocator::get().PcInputsEnumToString(static_cast<sdl::KeyCode>(keyIndex)) + " IsKeyUp";
                ImGui::Text(text.c_str());
            }
        }
        ImGui::Text("-----------");
        ImGui::Text("Mouse");
        for (unsigned int mouseIndex = 0;
            mouseIndex != static_cast<unsigned>(sdl::MouseButtonCode::LENGTH);
            mouseIndex++) {
            if (sdl::InputLocator::get().IsMouseButtonDown(static_cast<sdl::MouseButtonCode>(mouseIndex)))
            {
                std::string text = sdl::InputLocator::get().PcInputsEnumToString(static_cast<sdl::MouseButtonCode>(mouseIndex)) + " IsMouseButtonDown";
                ImGui::Text(text.c_str());
            }
            if (sdl::InputLocator::get().IsMouseButtonHeld(static_cast<sdl::MouseButtonCode>(mouseIndex)))
            {
                std::string text = sdl::InputLocator::get().PcInputsEnumToString(static_cast<sdl::MouseButtonCode>(mouseIndex)) + " IsMouseButtonHeld";
                ImGui::Text(text.c_str());
            }
            if (sdl::InputLocator::get().IsMouseButtonUp(static_cast<sdl::MouseButtonCode>(mouseIndex)))
            {
                std::string text = sdl::InputLocator::get().PcInputsEnumToString(static_cast<sdl::MouseButtonCode>(mouseIndex)) + " IsMouseButtonUp";
                ImGui::Text(text.c_str());
            }
        }
        std::string mouseText = "Mouse Position : " + std::to_string((sdl::InputLocator::get().GetMousePosition().x)) + ", " + std::to_string((sdl::InputLocator::get().GetMousePosition().y)) + ")";
        ImGui::Text(mouseText.c_str());
        mouseText = "Mouse Relative Position : " + std::to_string((sdl::InputLocator::get().GetRelativeMousePosition().x)) + ", " + std::to_string((sdl::InputLocator::get().GetRelativeMousePosition().y)) + ")";
        ImGui::Text(mouseText.c_str());
        mouseText = "Mouse Scroll : " + std::to_string((sdl::InputLocator::get().GetMouseScroll().x)) + ", " + std::to_string((sdl::InputLocator::get().GetMouseScroll().y)) + ")";
        ImGui::Text(mouseText.c_str());
        ImGui::Text("-----------");
        ImGui::Text("Controller");
        for (unsigned int controllerIndex = 0;
            controllerIndex != static_cast<unsigned>(sdl::ControllerInputs::LENGTH);
            controllerIndex++) {
            if (sdl::InputLocator::get().IsControllerDown(static_cast<sdl::ControllerInputs>(controllerIndex)))
            {
                std::string text = sdl::InputLocator::get().ControllerInputsEnumToString(static_cast<sdl::ControllerInputs>(controllerIndex)) + " IsControllerDown";
                ImGui::Text(text.c_str());
            }
            if (sdl::InputLocator::get().IsControllerHeld(static_cast<sdl::ControllerInputs>(controllerIndex)))
            {
                std::string text = sdl::InputLocator::get().ControllerInputsEnumToString(static_cast<sdl::ControllerInputs>(controllerIndex)) + " IsControllerHeld";
                ImGui::Text(text.c_str());
            }
            if (sdl::InputLocator::get().IsControllerUp(static_cast<sdl::ControllerInputs>(controllerIndex)))
            {
                std::string text = sdl::InputLocator::get().ControllerInputsEnumToString(static_cast<sdl::ControllerInputs>(controllerIndex)) + " IsControllerUp";
                ImGui::Text(text.c_str());
            }
        }
        for (unsigned int controllerAxisIndex = 0;
            controllerAxisIndex != static_cast<unsigned>(sdl::ControllerAxis::LENGTH);
            controllerAxisIndex++) {

            std::string text = sdl::InputLocator::get().ControllerAxisEnumToString(static_cast<sdl::ControllerAxis>(controllerAxisIndex)) + " : " + std::to_string(sdl::InputLocator::get().GetJoystickAxis(static_cast<sdl::ControllerAxis>(controllerAxisIndex)));
            ImGui::Text(text.c_str());
        }
#ifdef NN_NINTENDO_SDK
        ImGui::Text("-----------");
        ImGui::Text("Switch");
        for (unsigned int switchIndex = 0;
            switchIndex != static_cast<unsigned>(sdl::SwitchInputs::LENGTH);
            switchIndex++) {
            if (sdl::InputLocator::get().IsSwitchButtonDown(static_cast<sdl::SwitchInputs>(switchIndex)))
            {
                std::string text = sdl::InputLocator::get().SwitchInputsEnumToString(static_cast<sdl::SwitchInputs>(switchIndex)) + " IsSwitchButtonDown";
                ImGui::Text(text.c_str());
            }
            if (sdl::InputLocator::get().IsSwitchButtonHeld(static_cast<sdl::SwitchInputs>(switchIndex)))
            {
                std::string text = sdl::InputLocator::get().SwitchInputsEnumToString(static_cast<sdl::SwitchInputs>(switchIndex)) + " IsSwitchButtonHeld";
                ImGui::Text(text.c_str());
            }
            if (sdl::InputLocator::get().IsSwitchButtonUp(static_cast<sdl::SwitchInputs>(switchIndex)))
            {
                std::string text = sdl::InputLocator::get().SwitchInputsEnumToString(static_cast<sdl::SwitchInputs>(switchIndex)) + " IsSwitchButtonUp";
                ImGui::Text(text.c_str());
            }
        }
#endif
        ImGui::Text("-----------");
        ImGui::Text("InputAction");
        for (unsigned int inputIndex = 0;
            inputIndex != static_cast<unsigned>(sdl::InputAction::LENGTH);
            inputIndex++) {
            if (sdl::InputLocator::get().IsActionDown(static_cast<sdl::InputAction>(inputIndex)))
            {
                std::string text = sdl::InputLocator::get().ActionEnumToString(static_cast<sdl::InputAction>(inputIndex)) + " IsActionDown";
                ImGui::Text(text.c_str());
            }
            if (sdl::InputLocator::get().IsActionHeld(static_cast<sdl::InputAction>(inputIndex)))
            {
                std::string text = sdl::InputLocator::get().ActionEnumToString(static_cast<sdl::InputAction>(inputIndex)) + " IsActionHeld";
                ImGui::Text(text.c_str());
            }
            if (sdl::InputLocator::get().IsActionUp(static_cast<sdl::InputAction>(inputIndex)))
            {
                std::string text = sdl::InputLocator::get().ActionEnumToString(static_cast<sdl::InputAction>(inputIndex)) + " IsActionUp";
                ImGui::Text(text.c_str());
            }
        }
        ImGui::End();
    }

    void InputProgram::OnEvent(const SDL_Event& event)
    {

    }
}
